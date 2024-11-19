---
layout: post
title: "Display/KMS Meeting at XDC 2024: Detailed Report"
date: 2024-11-19 10:00:00 -0300
categories: igalia
---

XDC 2024 in Montreal was another fantastic gathering for the Linux Graphics
community. It was again a great time to immerse in the world of graphics
development, engage in stimulating conversations, and learn from inspiring
developers.

Many [Igalia](https://igalia.com/) colleagues and I participated in the
conference again, delivering multiple talks about our work on the Linux
Graphics stack and also organizing the Display/KMS meeting. This blog post is a
detailed report on the Display/KMS meeting held during this XDC edition.

**Short on Time?**

1. Catch the lightning talk summarizing the meeting here (you can even speed up
   2x):

<iframe width="560" height="315" src="https://www.youtube.com/embed/ww3KlaeCfzY?si=fUGveICjtK9fmx5o&amp;start=209" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

2. For a quick written summary, scroll down to the TL;DR section.

## TL;DR

This meeting took 3 hours and tackled a variety of topics related to DRM/KMS
(Linux/DRM Kernel Modesetting):
- **Sharing Drivers Between V4L2 and KMS:** Brainstorming solutions for using a
  single driver for devices used in both camera capture and display pipelines.
- **Real-Time Scheduling:** Addressing issues with non-blocking page flips
  encountering sigkills under real-time scheduling.
- **HDR/Color Management:** Agreement on merging the current proposal, with
  NVIDIA implementing its special cases on VKMS and adding missing parts on top
of Harry Wentland's (AMD) changes.
- **Display Mux:** Collaborative design discussions focusing on compositor
  control and cross-sync considerations.
- **Better Commit Failure Feedback:** Exploring ways to equip compositors with
  more detailed information for failure analysis.

---

## Bringing together Linux display developers in the XDC 2024

While I didn't present a talk this year, I co-organized a Display/KMS meeting (with
Rodrigo Siqueira of AMD) to build upon the momentum from the [2024 Linux
Display Next hackfest](https://events.pages.igalia.com/linuxdisplaynexthackfest/).
The meeting was attended by around 30 people in person and 4 remote participants.

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/xdc2024/xdc2024-display-meeting-room-2.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/xdc2024/xdc2024-display-meeting-room-2.jpg "Display/KMS meeting room with around 30 in-person participants, a large screen with remote participants and Jonas Adahl on the stage.")

**Speakers:** Melissa Wen (Igalia) and Rodrigo Siqueira (AMD)

**Link:** https://indico.freedesktop.org/event/6/contributions/383/

**Topics:** Similar to the hackfest, the meeting agenda was built over the
first two days of the conference and mixed talks follow-up with new ideas and
ongoing community efforts.

The final agenda covered five topics in the scheduled order:
1. How to share drivers between V4L2 and DRM for bridge-like components (new
   topic);
2. Real-time Scheduling (problems encountered after the Display Next hackfest);
3. HDR/Color Management (ofc);
4. Display Mux (from Display hackfest and XDC 2024 talk, bringing AMD and
   NVIDIA together);
5. (Better) Commit Failure Feedback (continuing the last minute topic of the
   Display Next hackfest).

## Unpacking the Topics

Similar to the hackfest, the meeting agenda evolved over the conference.
During the 3 hours of meeting, I coordinated the room and discussion rounds,
and Rodrigo Siqueira took notes and also contacted key developers to provide a
detailed report of the many topics discussed.

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/xdc2024/xdc2024-display-meeting-summary.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/xdc2024/xdc2024-display-meeting-summary.jpg "Melissa Wen and Rodrigo Siqueira presents a lightning talk that summarizes the Display/KMS meeting - with some slides.")

From his notes, let's dive into the key discussions!

### How to share drivers between V4L2 and KMS for bridge-like components.

Led by *Laurent Pinchart*, we delved into the challenge of creating a unified
driver for hardware devices (like scalers) that are used in both camera capture
pipelines and display pipelines.
- **Problem Statement:** How can we design a single kernel driver to handle
  devices that serve dual purposes in both V4L2 and DRM subsystems?
- **Potential Solutions:**
  1. *Multiple Compatible Strings:* We could assign different compatible
strings to the device tree node based on its usage in either the camera or
display pipeline. However, this approach might raise concerns from device tree
maintainers as it could be seen as a layer violation.
  2. *Separate Abstractions:* A single driver could expose the device to both
DRM and V4L2 through separate abstractions: drm-bridge for DRM and V4L2 subdev
for video. While simple, this approach requires maintaining two different
abstractions for the same underlying device.
  3. *Unified Kernel Abstraction:* We could create a new, unified kernel
abstraction that combines the best aspects of drm-bridge and V4L2 subdev. This
approach offers a more elegant solution but requires significant design effort
and potential migration challenges for existing hardware.

### Real-Time Scheduling Challenges

We have discussed real-time scheduling during [this year Linux Display Next
hackfest](https://events.pages.igalia.com/linuxdisplaynexthackfest/) and,
during the XDC 2024, *Jonas Adahl* brought up issues uncovered while
progressing on this front.
- **Context:** Non-blocking page-flips can, on rare occasions, take a long time
  and, for that reason, get a sigkill if the thread doing the atomic commit is
a real-time schedule.
- **Action items**:
  - Explore alternative backtraces during the busy wait (e.g., ftrace).
  - Investigate the maximum thread time in busy wait to reproduce issues faced
    by compositors. Tools like RTKit (mutter) can be used for better control
(Michel Dänzer can help with this setup).

### HDR/Color Management

This is a well-known topic with ongoing effort on all layers of the Linux
Display stack and has been discussed online and in-person in conferences and
meetings over the last years.

Here's a breakdown of the key points raised at this meeting:
- **[Talk: Color operations for Linux color pipeline on AMD
  devices](https://www.youtube.com/watch?v=623tXWWz9lc)**: In the previous day,
  Alex Hung (AMD) presented the implementation of this API on AMD display driver.
- **NVIDIA Integration**: While they agree with the overall proposal, NVIDIA
  needs to add some missing parts. Importantly, they will implement these on
  top of Harry Wentland's (AMD) proposal. Their specific requirements will be
  implemented on VKMS (Virtual Kernel Mode Setting driver) for further
  discussion. This VKMS implementation can benefit compositor developers by
  providing insights into NVIDIA's specific needs.
- **Other vendors**: There is a version of the KMS API applied on Intel color
  pipeline. Apart from that, other vendors appear to be comfortable with the
  current proposal but lacks the bandwidth to implement it right now.
- **Upstream Patches**: The relevant upstream patches were can be found
  [here](https://lore.kernel.org/dri-devel/20241003200129.1732122-1-harry.wentland@amd.com/).
*[As humorously notes, this series is eagerly awaiting your "Acked-by"
(approval)]*
- **Compositor Side**: The compositor developers have also made significant
  progress.
  - KDE has already implemented and validated the API through [an experimental
    implementation in
Kwin](https://invent.kde.org/plasma/kwin/-/commits/work/zamundaaa/drm-colorop).
  - Gamescope currently uses a driver-specific implementation but has
    [a draft](https://github.com/ValveSoftware/gamescope/pull/1309) that utilizes
    the generic version. However, some work is still required to fully transition
    away from the driver-specific approach.
    *AP: work on porting gamescope to KMS generic API*
  - Weston has also begun exploring implementation, and we might see something
    from them by the end of the year.
- **Kernel and Testing**: The kernel API proposal is well-refined and meets the
  DRM subsystem requirements. Thanks to *Harry Wentland* effort, we already
  have [the API attached to two hardware vendors](https://gitlab.freedesktop.org/hwentland/igt-gpu-tools/-/merge_requests/1)
  and [IGT tests](https://gitlab.freedesktop.org/hwentland/linux/-/merge_requests/5), and,
  thanks to *Xaver Hugl*, a compositor implementation in place.

Finally, there was a strong sense of agreement that the current proposal for
HDR/Color Management is ready to be merged. In simpler terms, everything seems
to be working well on the technical side - all signs point to merging and
"shipping" the DRM/KMS plane color management API!

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/xdc2024/xdc2024-display-meeting-summary-2.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/xdc2024/xdc2024-display-meeting-summary-2.jpg "Melissa Wen summarizes the HDR/Color Management discussions in the Display/KMS meeting.")

### Display Mux

During the meeting, *Daniel Dadap* led a brainstorming session on the
design of the display mux switching sequence, in which the compositor would arm
the switch via sysfs, then send a modeset to the outgoing driver, followed by a
modeset to the incoming driver.

- **Context:**
  - During this year Linux Display Next hackfest, Mario Limonciello (AMD)
    introduced the topic and led a discussion on [Display Mux](https://cloud.igalia.com/s/7Z5xdAQiN5pz6GC).
  - Daniel Dadap (NVIDIA) retook this discussion with the
    [XDC 2024 talk: Dynamic Switching of Display Muxes on Hybrid GPU Systems](https://www.youtube.com/watch?v=gTTKmLa0urU).

- **Key Considerations:**
  - *HPD Handling:* There was a general consensus that disabling HPD can be
    part of the sequence for internal panels and we don't need to focus on it
    here.
  - *Cross-Sync:* Ensuring synchronization between the compositor and the
    drivers is crucial. The compositor should act as the "drm-master" to
    coordinate the entire sequence, but how can this be ensured?
  - *Future-Proofing:* The design should not assume the presence of a mux. In
    future scenarios, direct sharing over DP might be possible.

- **Action points:**
  - *Sharing DP AUX:* Explore the idea of sharing DP AUX and its implications.
  - *Backlight*: The backlight definition represents a problem in the mux
    switch context, so we should explore some of the current specs available
    for that.

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/xdc2024/xdc2024-display-meeting-room.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/xdc2024/xdc2024-display-meeting-room.jpg "The Display/KMS meeting room with Daniel Dadap and Harry Wentland discussing about Display Mux.")


### Towards Better Commit Failure Feedback

In the last part of the meeting, *Xaver Hugl* asked for better commit failure
feedback.

- **Problem description:** Compositors currently face challenges in collecting
  detailed information from the kernel about commit failures. This lack of
  granular data hinders their ability to understand and address the root causes
  of these failures.

To address this issue, we discussed **several potential improvements**:

- *Direct Kernel Log Access:* One idea is to directly load relevant kernel logs
  into the compositor. This would provide more detailed information about the
  failure and potentially aid in debugging.
- *Finer-Grained Failure Reporting:* We also explored the possibility of
  separating atomic failures into more specific categories. Not all failures
  are critical, and understanding the nature of the failure can help compositors
  take appropriate action.
- *Enhanced Logging:* Currently, the dmesg log doesn't provide enough
  information for user-space validation. Raising the log level to capture more
  detailed information during failures could be a viable solution.

By implementing these improvements, we aim to equip compositors with the
necessary tools to better understand and resolve commit failures, leading to a
more robust and stable display system.

## A Big Thank You!

Huge thanks to Rodrigo Siqueira for these detailed meeting notes. Also, Laurent
Pinchart, Jonas Adahl, Daniel Dadap, Xaver Hugl, and Harry Wentland for
bringing up interesting topics and leading discussions. Finally, thanks to all
the participants who enriched the discussions with their experience, ideas, and
inputs, especially Alex Goins, Antonino Maniscalco, Austin Shafer, Daniel
Stone, Demi Obenour, Jessica Zhang, Joan Torres, Leo Li, Liviu Dudau, Mario
Limonciello, Michel Dänzer, Rob Clark, Simon Ser and Teddy Li.

This collaborative effort will undoubtedly contribute to the continued
development of the Linux display stack.

**Stay tuned for future updates!**
