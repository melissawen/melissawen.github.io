---
layout: post
title: "Multiple syncobjs support for V3D(V) (Part 1)"
date: 2022-05-10 07:00:00 -0100
categories: igalia
---

As you may already know, we at
[Igalia](https://www.igalia.com/technology/graphics) have been working on
several improvements to the 3D rendering drivers of Broadcom Videocore GPU,
found in Raspberry Pi 4 devices. One of our recent works focused on improving
V3D(V) drivers adherence to Vulkan submission and synchronization framework. We
had to cross various layers from the Linux Graphics stack to add support for
multiple syncobjs to V3D(V), from the Linux/DRM kernel to the Vulkan driver. We
have delivered bug fixes, a generic gate to extend job submission
interfaces, and a more direct sync mapping of the Vulkan framework. These
changes did not impact the performance of the tested games and brought greater
precision to the synchronization mechanisms. Ultimately, support for multiple
syncobjs opened the door to new features and other improvements to the V3DV
submission framework.

## DRM Syncobjs

But, first, what are [DRM sync objs](https://dri.freedesktop.org/docs/drm/gpu/drm-mm.html?highlight=syncobjs#drm-sync-objects)?

```
* DRM synchronization objects (syncobj, see struct &drm_syncobj) provide a
* container for a synchronization primitive which can be used by userspace
* to explicitly synchronize GPU commands, can be shared between userspace
* processes, and can be shared between different DRM drivers.
* Their primary use-case is to implement Vulkan fences and semaphores.
[...]
* At it's core, a syncobj is simply a wrapper around a pointer to a struct
* &dma_fence which may be NULL.
```

And [Jason Ekstrand](https://www.jlekstrand.net/jason/blog/) well-summarized
dma\_fence features in [a talk at the Linux Plumbers Conference
2021](https://youtu.be/ch-N11yDCq0?t=13508):

> A struct that represents a (potentially future) event:
> - Has a boolean "signaled" state
> - Has a bunch of useful utility helpers/concepts, such as refcount, callback wait mechanisms, etc.

> Provides two guarantees:
> - One-shot: once signaled, it will be signaled forever
> - Finite-time: once exposed, is guaranteed signal in a reasonable amount of time

## What does multiple semaphores support mean for Raspberry Pi 4 GPU drivers?

For our main purpose, the multiple syncobjs support means that V3DV can submit
jobs with more than one wait and signal semaphore. In the kernel space, wait
semaphores become explicit job dependencies to wait on before executing the
job. Signal semaphores (or post dependencies), in turn, work as fences to be
signaled when the job completes its execution, unlocking following jobs that
depend on its completion.

The multisync support development comprised of many decision-making points and
steps summarized as follow:

- added to the v3d kernel-driver capabilities to handle multiple syncobj;
- exposed multisync capabilities to the userspace through a generic extension;
  and
- reworked synchronization mechanisms of the V3DV driver to benefit from this
  feature
- enabled simulator to work with multiple semaphores
- tested on Vulkan games to verify the correctness and possible performance
  enhancements.

We decided to refactor parts of the V3D(V) submission design in kernel-space
and userspace during this development. We improved job scheduling on V3D-kernel
and the V3DV job submission design. We also delivered more accurate
synchronizing mechanisms and further updates in the Broadcom Vulkan driver
running on Raspberry Pi 4. Therefore, we summarize here changes in the kernel
space, describing the previous state of the driver, taking decisions, side
improvements, and fixes.

### From single to multiple binary in/out syncobjs:

Initially, V3D was very limited in the numbers of syncobjs per job
submission. V3D job interfaces (CL, CSD, and TFU) only supported one syncobj
(in\_sync) to be added as an execution dependency and one syncobj (out\_sync) to
be signaled when a submission completes. Except for CL submission, which
accepts two in\_syncs: one for binner and another for render job, it didn't
change the limited options.

Meanwhile in the userspace, the V3DV driver followed alternative paths to meet
Vulkan's synchronization and submission framework. It needed to handle multiple
wait and signal semaphores, but the V3D kernel-driver interface only accepts
one in\_sync and one out\_sync. In short, V3DV had to fit multiple semaphores
into one when submitting every GPU job.

### Generic ioctl extension

The first decision was how to extend the V3D interface to accept multiple in
and out syncobjs. We could extend each ioctl with two entries of syncobj arrays
and two entries for their counters. We could create new ioctls with multiple
in/out syncobj. But after examining other drivers solutions to extend their
submission's interface, we decided to extend V3D ioctls (v3d\_cl\_submit\_ioctl,
v3d\_csd\_submit\_ioctl, v3d\_tfu\_submit\_ioctl) by a generic ioctl extension.

I found a curious commit message when I was examining how other developers
handled the issue in the past:

```
Author: Chris Wilson <chris@chris-wilson.co.uk>
Date:   Fri Mar 22 09:23:22 2019 +0000

    drm/i915: Introduce the i915_user_extension_method
    
    An idea for extending uABI inspired by Vulkan's extension chains.
    Instead of expanding the data struct for each ioctl every time we need
    to add a new feature, define an extension chain instead. As we add
    optional interfaces to control the ioctl, we define a new extension
    struct that can be linked into the ioctl data only when required by the
    user. The key advantage being able to ignore large control structs for
    optional interfaces/extensions, while being able to process them in a
    consistent manner.
    
    In comparison to other extensible ioctls, the key difference is the
    use of a linked chain of extension structs vs an array of tagged
    pointers. For example,
    
    struct drm_amdgpu_cs_chunk {
    	__u32		chunk_id;
        __u32		length_dw;
        __u64		chunk_data;
    };
[...]
```

So, inspired by `amdgpu_cs_chunk` and `i915_user_extension`, we opted to extend
the V3D interface through a generic interface. After applying some suggestions
from [Iago Toral (Igalia)](https://blogs.igalia.com/itoral) and [Daniel
Vetter](https://blog.ffwll.ch),  we reached the following struct:

```
struct drm_v3d_extension {
	__u64 next;
	__u32 id;
#define DRM_V3D_EXT_ID_MULTI_SYNC		0x01
	__u32 flags; /* mbz */
};
```

This generic extension has an id to identify the feature/extension we are
adding to an ioctl (that maps the related struct type), a pointer to the next
extension, and flags (if needed). Whenever we need to extend the V3D interface
again for another specific feature, we subclass this generic extension into the
specific one instead of extending ioctls indefinitely.

### Multisync extension

For the multiple syncobjs extension, we define a `multi_sync` extension struct
that subclasses the generic extension struct. It has arrays of in and out
syncobjs, the respective number of elements in each of them, and a `wait_stage`
value used in CL submissions to determine which job needs to wait for syncobjs
before running.

```
struct drm_v3d_multi_sync {
	struct drm_v3d_extension base;
	/* Array of wait and signal semaphores */
	__u64 in_syncs;
	__u64 out_syncs;

	/* Number of entries */
	__u32 in_sync_count;
	__u32 out_sync_count;

	/* set the stage (v3d_queue) to sync */
	__u32 wait_stage;

	__u32 pad; /* mbz */
};
```

And if a multisync extension is defined, the V3D driver ignores the previous
interface of single in/out syncobjs.

Once we had the interface to support multiple in/out syncobjs, v3d
kernel-driver needed to handle it. As V3D uses the DRM scheduler for job
executions, changing from single syncobj to multiples is quite straightforward.
V3D copies from userspace the in syncobjs and uses `drm_syncobj_find_fence()+
drm_sched_job_add_dependency()` to add all `in_syncs` (wait semaphores) as job
dependencies, i.e. syncobjs to be checked by the scheduler before running the
job. On CL submissions, we have the bin and render jobs, so V3D follows the
value of `wait_stage` to determine which job depends on those `in_syncs` to start
its execution.

When V3D defines the last job in a submission, it replaces `dma_fence` of
`out_syncs` with the `done_fence` from this last job. It uses `drm_syncobj_find() +
drm_syncobj_replace_fence()` to do that. Therefore, when a job completes its
execution and signals `done_fence`, all `out_syncs` are signaled too. 

### Other improvements to v3d kernel driver

This work also made possible some improvements in the original implementation.
Following Iago's suggestions, we refactored the job's initialization code to
allocate memory and initialize a job in one go. With this, we started to clean
up resources more cohesively, clearly distinguishing cleanups in case of
failure from job completion. We also fixed the resource cleanup when a job is
aborted before the DRM scheduler arms it - at that point, `drm_sched_job_arm()`
had recently been introduced to job initialization. Finally, we prepared the
semaphore interface to implement timeline syncobjs in the future.

### Going Up

The patchset that adds multiple syncobjs support and improvements to V3D is
available [here](https://patchwork.freedesktop.org/series/93388/) and comprises
four patches:

* drm/v3d: decouple adding job dependencies steps from job init
* drm/v3d: alloc and init job in one shot
* drm/v3d: add generic ioctl extension
* drm/v3d: add multiple syncobjs support

After extending the V3D kernel interface to accept multiple syncobjs, we worked
on V3DV to benefit from V3D multisync capabilities. In the next post, I will
describe a little of this work.
