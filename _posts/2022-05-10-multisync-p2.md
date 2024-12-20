---
layout: post
title: "Multiple syncobjs support for V3D(V) (Part 2)"
date: 2022-05-10 08:00:00 -0100
categories: igalia
---

In the [previous post](https://melissawen.github.io/blog/2022/05/10/multisync-p1), I described how we enable multiple syncobjs capabilities
in the V3D kernel driver.  Now I will tell you what was changed on the
userspace side, where we reworked the V3DV sync mechanisms to use Vulkan
multiple wait and signal semaphores directly.  This change represents greater
adherence to the Vulkan submission framework.

I was not used to Vulkan concepts and the V3DV driver. Fortunately, I counted
on the guidance of the Igalia's Graphics team, mainly
[Iago Toral](https://blogs.igalia.com/itoral) (thanks!), to understand the
[Vulkan Graphics Pipeline](https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Introduction),
[sync scopes](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#synchronization-dependencies-scopes),
and [submission order](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#synchronization-submission-order).
Therefore, we changed the original V3DV implementation for
[vkQueueSubmit](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/vkQueueSubmit.html)
and all related functions to allow direct mapping of multiple semaphores from
V3DV to the V3D-kernel interface.

*Disclaimer:* Here's a brief and probably inaccurate background, which we'll go into more detail later on.

In Vulkan, GPU work submissions are described as command buffers.  These
command buffers, with GPU jobs, are grouped in a command buffer submission
batch, specified by
[vkSubmitInfo](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkSubmitInfo.html),
and submitted to a queue for execution.
[vkQueueSubmit](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/vkQueueSubmit.html)
is the command called to submit command buffers to a queue.  Besides command
buffers, vkSubmitInfo also specifies semaphores to wait before starting the
batch execution and semaphores to signal when all command buffers in the batch
are complete. Moreover, a fence in vkQueueSubmit can be signaled when all
command buffer batches have completed execution.

From this sequence, we can see some [implicit ordering
guarantees](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#synchronization-implicit).
Submission order defines the start order of execution between command buffers,
in other words, it is determined by the order in which
pSubmits appear in VkQueueSubmit and pCommandBuffers appear in VkSubmitInfo.
However, we don't have any completion guarantees for jobs submitted to
different GPU queue, which means they may overlap and complete out of order. Of
course, jobs submitted to the same GPU engine follow start and finish order.
A fence is ordered after all semaphores signal operations for signal operation
order. In addition to implicit sync, we also have some explicit sync resources,
such as semaphores, fences, and events.

Considering these implicit and explicit sync mechanisms, we rework the V3DV
implementation of queue submissions to better use multiple syncobjs
capabilities from the kernel. In this merge request, you can find this work:
[v3dv: add support to multiple wait and signal
semaphores](https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/13178).
In this blog post, we run through each scope of change of this merge request
for a V3D driver-guided description of the multisync support implementation.

#### Groundwork and basic code clean-up:

As the original V3D-kernel interface allowed only one semaphore, V3DV
resorted to booleans to "translate" multiple semaphores into one.
Consequently, if a command buffer batch had at least one semaphore, it needed
to wait on all jobs submitted complete before starting its execution. So,
instead of just boolean, we created and changed structs that store semaphores
information to accept the actual list of wait semaphores.

- [v3dv: drop unused variable on handle\_set\_event\_cpu\_job](https://gitlab.freedesktop.org/mesa/mesa/-/commit/09991fc47bf2588dbb89a9e54644aa19a9170163)
- [v3dv: wrap wait semaphores info in v3dv\_submit\_info\_semaphores](https://gitlab.freedesktop.org/mesa/mesa/-/commit/d148379edf34d39a50bc7cb1c5398499acbfe670)
- [v3dv: store wait semaphores in event\_wait\_cpu\_job\_info](https://gitlab.freedesktop.org/mesa/mesa/-/commit/d5bd18fbb3002293853780b3988e5cb9313d226e)

#### Expose multisync kernel interface to the driver:

In the two commits below, we basically updated the
[DRM V3D interface](https://cgit.freedesktop.org/drm/drm-misc/tree/include/uapi/drm/v3d_drm.h)
from that one defined in the kernel and verified if the multisync capability
is available for use.

- [drm-uapi/v3d: extend interface for multiple semaphores support](https://gitlab.freedesktop.org/mesa/mesa/-/commit/264dedf690f47b5bad9f73cf71e4a051c88075e5)
- [v3dv: check multiple semaphores capability](https://gitlab.freedesktop.org/mesa/mesa/-/commit/85c49db10dd7d427cc91508327467431d9f25956)

#### Handle multiple semaphores for all GPU job types:

At this point, we were only changing the submission design to consider multiple
wait semaphores. Before supporting multisync, V3DV was waiting for the last job
submitted to be signaled when at least one wait semaphore was defined, even
when serialization wasn’t required. V3DV handle GPU jobs according to the GPU
queue in which they are submitted:

* Control List (CL) for binning and rendering
* Texture Formatting Unit (TFU)
* Compute Shader Dispatch (CSD)

Therefore, we changed their submission setup to do jobs submitted to any
GPU queues able to handle more than one wait semaphores.

- [v3dv: enable multiple semaphores on cl submission](https://gitlab.freedesktop.org/mesa/mesa/-/commit/ff8586c3452453527ae4c4d794edcb81ad65e661)
- [v3dv: enable multiple semaphores for tfu job](https://gitlab.freedesktop.org/mesa/mesa/-/commit/ad09e50129e8a46ef465fe3123e0ce87b5a11c79)
- [v3dv: enable multiple semaphores for csd job](https://gitlab.freedesktop.org/mesa/mesa/-/commit/a7052dcf2cfb54aa22f7c028420abc6a9ee44d78)
- [v3dv: enable GPU jobs to signal multiple semaphores](https://gitlab.freedesktop.org/mesa/mesa/-/commit/fd973218a6c95d09cad8558712756a772ae78f66)

These commits created all mechanisms to set arrays of wait and signal
semaphores for GPU job submissions:
* Checking the conditions to define the wait\_stage.
* Wrapping them in a multisync extension.
* According to the kernel interface (described in the previous blog post),
  configure the generic extension as a multisync extension. 

Finally, we extended the ability of GPU jobs to handle multiple signal
semaphores, but at this point, no GPU job is actually in charge of signaling
them. With this in place, we could rework part of the code that tracks CPU and
GPU job completions by verifying the GPU status and threads spawned by Event
jobs.

#### Rework the QueueWaitIdle mechanism to track the syncobj of the last job submitted in each queue:

As we had only single in/out syncobj interfaces for semaphores, we used a
single `last_job_sync` to synchronize job dependencies of the previous
submission. Although the DRM scheduler guarantees the order of starting to
execute a job in the same queue in the kernel space, the order of completion
isn’t predictable. On the other hand, we still needed to use syncobjs to follow
job completion since we have event threads on the CPU side. Therefore, a more
accurate implementation requires last\_job syncobjs to track when each engine
(CL, TFU, and CSD) is idle. We also needed to keep the driver working on
previous versions of v3d kernel-driver with single semaphores, then we kept
tracking ANY `last_job_sync` to preserve the previous implementation.

- [v3dv: track submitted jobs by GPU queue type](https://gitlab.freedesktop.org/mesa/mesa/-/commit/03a6a8274042af376b86c82ff6bbff9826a924b2)

#### Rework synchronization and submission design to let the jobs handle wait and signal semaphores:

With multiple semaphores support, the conditions for waiting and signaling
semaphores changed accordingly to the particularities of each GPU job (CL, CSD,
TFU) and CPU job restrictions (Events, CSD indirect, etc.). In this sense, we
redesigned V3DV semaphores handling and job submissions for command buffer
batches in
[vkQueueSubmit](https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/vkQueueSubmit.html).

We scrutinized possible scenarios for submitting command buffer batches to
change the original implementation carefully. It resulted in three commits
more:

- [v3dv: handle wait semaphores in the first job by queue](https://gitlab.freedesktop.org/mesa/mesa/-/commit/0ab98612ef6860751f6a5115229e561eab7bb890)

We keep track of whether we have submitted a job to each GPU queue (CSD, TFU,
CL) and a CPU job for each command buffer. We use syncobjs to track the last
job submitted to each GPU queue and a flag that indicates if this represents
the beginning of a command buffer.

The first GPU job submitted to a GPU queue in a command buffer should wait on
wait semaphores. The first CPU job submitted in a command buffer should call
v3dv\_QueueWaitIdle() to do the waiting and ignore semaphores (because it is
waiting for everything).

If the job is not the first but has the serialize flag set, it should wait on
the completion of all last job submitted to any GPU queue before running. In
practice, it means using syncobjs to track the last job submitted by queue and
add these syncobjs as job dependencies of this serialized job.

- [v3dv: process signal semaphores in the very last job](https://gitlab.freedesktop.org/mesa/mesa/-/commit/bce77e758acbb168de802d30fd0a6d6b7ea1d1a4)

If this job is the last job of a command buffer batch, it may be used to signal
semaphores if this command buffer batch has only one type of GPU job (because
we have guarantees of execution ordering). Otherwise, we emit a no-op job just
to signal semaphores. It waits on the completion of all last jobs submitted to any
GPU queue and then signal semaphores. *Note: We changed this approach to
correctly deal with ordering changes caused by event threads at some point.
Whenever we have an event job in the command buffer, we cannot use the last job
in the last command buffer assumption. We have to wait all event threads
complete to signal*

- [v3dv: signal fence when all submitted jobs complete execution](https://gitlab.freedesktop.org/mesa/mesa/-/commit/9319ffb53d1e9d54e05aaba250853f12a1562c5f)

After submitting all command buffers, we emit a no-op job to wait on all last
jobs by queue completion and signal fence. *Note: at some point, we changed
this approach to correct deal with ordering changes caused by event threads, as
mentioned before.*

### Final considerations

With many changes and many rounds of reviews, the patchset was merged. After
more validations and code review, we polished and fixed the implementation
together with external contributions:
- [v3dv: fix double free error when releasing sems\_info resources](https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/14736)
- [v3dv: enable multisync in the simulator](https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/14768)
- [v3dv: Add missing unlocks on errors.](https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/14663)
- [v3dv: don't submit noop job if there is nothing to wait on or signal](https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/14802)

Also, multisync capabilities enabled us to add new features to V3DV and
switch the driver to the common synchronization and submission framework:

- [v3dv: expose support for semaphore
  imports](https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/15342)
> This was waiting for multisync support in the v3d kernel, which is already
> available. Exposing this feature however enabled a few more CTS tests that
> exposed pre-existing bugs in the user-space driver so we fix those here
> before exposing the feature.

- [v3dv: Switch to the common submit
  framework](https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/15704)
> This should give you emulated timeline semaphores for free and
> kernel-assisted sharable timeline semaphores for cheap once you have the
> kernel interface wired in.

We used a set of games to ensure no performance regression in the new
implementation.  For this, we used
[GFXReconstruct](https://github.com/LunarG/gfxreconstruct) to capture Vulkan
API calls when playing those games. Then, we compared results with and without
multisync caps in the kernelspace and also enabling multisync on v3dv. We
didn't observe any compromise in performance, but improvements when replaying
scenes of vkQuake game.
