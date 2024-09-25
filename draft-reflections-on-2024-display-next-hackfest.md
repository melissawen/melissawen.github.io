---
layout: page
title: "Draft - Reflections on 2024 Linux Display Next Hackfest"
date: 2024-09-25 09:50:00 -0300
draft: draft
permalink: /draft-reflections-2024-display-next-hackfest
---

Hey everyone! 

The [2024 Linux Display Next
hackfest](https://events.pages.igalia.com/linuxdisplaynexthackfest/) concluded
in May, and its outcomes continue to shape the Linux Display stack. Igalia
hosted this year's event in A Coruña, Spain, bringing together leading experts
in the field. [Samuel Iglesias](https://blogs.igalia.com/siglesias/) and I
organized this year's edition and this blog post summarize the experience and
its fruits.

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-0.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-0.jpg "Hackfest main room: preparation before starting the first day with organizers and some participants in the main room checking schedule and chatting.")

One of the highlights of this year's hackfest was the wide range of backgrounds
represented by our 40 participants (both on-site and remotely). Developers and
experts from various companies and open-source projects came together to
advance the Linux Display ecosystem. You can find the list of participants
[here](https://github.com/melissawen/2024linuxdisplayhackfest/wiki/list-of-participants).

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-1.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-1.jpg "Hackfest main room: in-person participants seated at tables arranged in a U shape, with microphones, laptops, projector and a large screen.")

The event covered a broad spectrum of topics affecting the development of Linux
projects, user experiences, and the future of display technologies on Linux.
From cutting-edge topics to long-term discussions, you can check the event
agenda
[here](https://github.com/melissawen/2024linuxdisplayhackfest/issues/11).

## Organization Highlights

The hackfest was marked by in-depth discussions and knowledge sharing among
Linux contributors, making everyone inspired, informed, and connected to the
community. Building on feedback from the previous year, we refined the
unconference format to enhance participant preparation and engagement.

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-2.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-2.jpg "Hackfest main room: laptop screen showing in-person participants in the hackfest room on Jitsi.")

**Structured Agenda and Timeboxes:** Each session had a defined scope, time
limit (1h20 or 2h10), and began with an introductory talk on the topic.
  - **Participant-Led Discussions:** We pre-selected in-person participants to
    lead discussions, allowing them to prepare introductions, resources, and
scope.
  - **Transparent Scheduling:** The schedule was shared in advance as GitHub
    issues, encouraging participants to review and prepare for sessions of
interest.

**Engaging Sessions:** The hackfest featured a variety of topics, including
presentations and discussions on how participants were addressing specific
subjects within their companies.
  - **No Breakout Rooms, No Overlaps:** All participants chose to attend all
    sessions, eliminating the need for separate breakout rooms. We also adapted
run-time schedule to keep everybody involved in the same topics.
  - **Real-time Updates:** We provided notifications and updates through
    dedicated emails and the event matrix room.

**Strengthening Community Connections:** The hackfest offered ample
opportunities for networking among attendees.
  - **Social Events:** Igalia sponsored coffee breaks, lunches, and a dinner at
    a local restaurant.
[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/artabria.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/artabria.jpg "First-day dinner at Artabria restaurant: large table with all participants.")

  - **Museum Visit:** Participants enjoyed a sponsored visit to the Museum of
    Estrela Galicia Beer (MEGA).
[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/mega-ppl.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/mega-ppl.jpg "Guided tour at MEGA: participants tasting beers in the end of the guided tour at Museum Estrela Galicia.")

## Fruitful Discussions and Follow-up

The structured agenda and breaks allowed us to cover multiple topics during the
hackfest. These discussions have led to new display feature development and
improvements, as evidenced by patches, merge requests, and implementations in
project repositories and mailing lists.

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-discussions.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-discussions.jpg "Hackfest main room: participants discussions with Alex Goins speaking")

With the KMS color management API taking shape, we discussed refinements and
best approaches to cover the variety of color pipeline from different
hardware-vendors. We are also investigating techniques for a performant
SDR\<-\>HDR content reproduction and reducing latency and power consumption when
using the color blocks of the hardware.

### Color Management/HDR

Color Management and HDR continued to be the hottest topic of the hackfest. We
had three sessions dedicated to discuss Color and HDR across Linux Display
stack layers.

#### Color/HDR (Kernel-Level)

Harry Wentland (AMD) led this session.

Here, kernel Developers shared the Color Management pipeline of AMD, Intel and
NVidia. We counted with diagrams and explanations from HW-vendors developers
that discussed differences, constraints and paths to fit them into the KMS
generic color management properties such as advertising modeset needs,
`IN\_FORMAT`, [segmented LUTs](https://github.com/ckborah/drm-tip-sandbox/commit/86441c236c9e16a430b629f0a278b444ec1960c8#diff-dab99b769a18136804a5e52ebb50648438500b97f6c42fcdb69fa0668bffff88R3814),
interpolation types, etc. Developers from Qualcomm and ARM also added
information regarding their hardware.

Upstream work related to this session:
- [KMS color management properties (new version - v5)](https://lore.kernel.org/dri-devel/20240819205714.316380-1-harry.wentland@amd.com/);
- [IGT Tests](https://lore.kernel.org/igt-dev/20240819205823.316656-1-harry.wentland@amd.com/);
- [drm\_info draft support of v4 DRM/KMS plane color properties](https://gitlab.freedesktop.org/mwen/drm_info/-/merge_requests/1);
- [gamescope draft support of v4 DRM/KMS plane color properties](https://github.com/ValveSoftware/gamescope/pull/1309);
- [Kwin WIP implementation of DRM/KMS plane color properties](https://lore.kernel.org/amd-gfx/CAFZQkGzLjCOSPvk0kYYXyJm8E6Szdw9PJUcUQzew-EBfQjzz_g@mail.gmail.com/).

#### Color/HDR (Compositor-Level)

Sebastian Wick (RedHat) led this session. 

It started with Sebastian's presentation covering Wayland color protocols and
compositor implementation. Also, an explanation of APIs provided by Wayland and
how they can be used to achieve better color management for applications and
discussions around ICC profiles and color representation metadata. There was
also an intensive Q&A about LittleCMS with Marti Maria.

Upstream work related to this session:
- [Wayland color management protocol](https://gitlab.freedesktop.org/wayland/wayland-protocols/-/merge_requests/14);
- [Wayland color representation protocol](https://gitlab.freedesktop.org/wayland/wayland-protocols/-/merge_requests/183);
- [HDR support merged on Mutter](https://gitlab.gnome.org/GNOME/mutter/-/merge_requests/3433);
- [Color management protocol on Mutter](https://gitlab.gnome.org/GNOME/mutter/-/merge_requests/3893);
- [Color management protocol on GTK](https://gitlab.gnome.org/GNOME/gtk/-/merge_requests/7444).

#### Color/HDR (Use Cases and Testing)

Christopher Cameron (Google) and Melissa Wen (Igalia) led this session.

In contrast to the other sessions, here we focused less on implementation and
more on brainstorming and reflections of real-world SDR and HDR transformations
(use and validation) and gainmaps. Christopher gave a nice presentation
explaining HDR gainmap images and how we should think of HDR. This presentation
and Q&A were important to put participants at the same page of how to
transition between SDR and HDR and somehow "emulating" HDR.

We also discussed on the usage of a kernel background color property.

Finally, we discussed a bit about
[Chamelium](https://www.chromium.org/chromium-os/developer-library/guides/hardware-schematics/chamelium/)
and the future of VKMS (future work and maintainership).

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-discussions-2.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-discussions-2.jpg "Hackfest main room: participants discussions with Chris Cameron speaking.")


### Power Savings vs Color/Latency

Mario Limonciello (AMD) led this session.

Mario gave an introductory presentation about AMD ABM (adaptive backlight
management) that is similar to Intel DPST. After some discussions, we agreed on
exposing a kernel property for power saving policy. This work was already
merged on kernel and the userspace support is under development.

Upstream work related to this session:
- [Kernel series: Add support for 'power saving policy' property (merged)](https://patchwork.freedesktop.org/series/135675/)
- [Mutter: issue: support for "power saving policy" property](https://gitlab.gnome.org/GNOME/mutter/-/issues/3589)
- [Kwin: MR Draft: backends/drm: add support for the "power saving policy" property](https://invent.kde.org/plasma/kwin/-/merge_requests/6028)

### Strategy for video and gaming use-cases

Leo Li (AMD) led this session.

Miguel Casas (Google) started this session with a presentation of Overlays in
Chrome/OS Video, explaining the main goal of power saving by switching off GPU
for accelerated compositing and the challenges of different colorspace/HDR for
video on Linux.

Then Leo Li presented different strategies for video and gaming and we
discussed the userspace need of more detailed feedback mechanisms to understand
failures when offloading. Also, creating a debugFS interface came up as a tool
for debugging and analysis.

### Real-time scheduling and async KMS API

Xaver Hugl (KDE/BlueSystems) led this session.

Compositor developers have exposed some issues with doing real-time scheduling
and async page flips. One is that the Kernel limits the lifetime of realtime
threads and if a modeset takes too long, the thread will be killed and thus the
compositor as well. Also, simple page flips take longer than expected and
drivers should optimize them.

Another issue is the lack of feedback to compositors about hardware programming
time and commit deadlines (the lastest possible time to commit). This is
difficult to predict from drivers, since it varies greatly with the type of
properties. For example, color management updates take much longer.

In this regard, we discusssed implementing a `hw_done` callback to timestamp
when the hardware programming of the last atomic commit is complete. Also an
API to pre-program color pipeline in a kind of A/B scheme. It may not be
supported by all drivers, but might be useful in different ways.

### VRR/Frame Limit, Display Mux, Display Control, and more... and beer

We also had sessions to discuss a new KMS API to mitigate headaches on **VRR
and Frame Limit** as different brightness level at different refresh rates,
abrupt changes of refresh rates, low frame rate compensation (LFC) and precise
timing in VRR more.

On **Display Control** we discussed features missing in the current KMS
interface for HDR mode, atomic backlight settings, source-based tone mapping,
etc. We also discussed the need of a place where compositor developers can post
TODOs to be developed by KMS people.

The **Content-adaptive Scaling and Sharpening** session focused on sharpening
and scaling filters. In the **Display Mux** session, we discussed proposals to
expose the capability of dynamic mux switching display signal between discrete
and integrated GPUs.

In the **last session of the 2024 Display Next Hackfest**, participants
representing different compositors summarized current and future work and built
a Linux Display "wish list", which includes: improvements to VTTY and HDR
switching, better dmabuf API for multi-GPU support, definition of tone mapping,
blending and scaling sematics, and wayland protocols for advertising to clients
which colorspaces are supported.

We closed this session with a status update on feature development by
compositors, including but not limited to: plane offloading (from libcamera to
output) / HDR video offloading (dma-heaps) / plane-based scrolling for web
pages, color management / HDR / ICC profiles support, addressing issues such as
flickering when color primaries don't match, etc.

After three days of intensive discussions, all in-person participants went to a
guided tour at the Museum of Extrela Galicia beer (MEGA), pouring and tasting
the most famous local beer.

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/mega-pouring.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/mega-pouring.jpg "Guided tour at MEGA: participants pouring beer")

## Feedback and Future Directions

Participants provided valuable feedback on the hackfest, including suggestions
for future improvements.

- **Schedule and Break-time Setup:** Having a pre-defined agenda and schedule
  provided a better balance between long discussions and mental refreshments,
preventing the fatigue caused by endless discussions.
- **Action Points:** Some participants recommended explicitly asking for action
  points at the end of each session and assigning people to follow-up tasks.
- **Remote Participation:** Remote attendees appreciated the inclusive setup
  and opportunities to actively participate in discussions.
- **Technical Challenges:** There were bandwidth and video streaming issues
  during some sessions due to the large number of participants.

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-feedback.jpg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/refs/heads/master/img/2024-ldnh/hackfest-room-feedback.jpg "Hackfest main room: laptop screen displays remote participants")

## Thank you for joining the 2024 Display Next Hackfest

We can't help but thank the 40 participants, who engaged in-person or virtually
on relevant discussions, for a collaborative evolution of the Linux display
stack and for building an insightful agenda.

A big thank you to the leaders and presenters of the nine sessions: Christopher
Cameron (Google), Harry Wentland (AMD), Leo Li (AMD), Mario Limoncello (AMD),
Sebastian Wick (RedHat) and Xaver Hugl (KDE/BlueSystems) for the effort in
preparing the sessions, explaining the topic and guiding discussions. My
acknowledge to the others in-person participants that made such an effort to
travel to A Coruña: Alex Goins (NVIDIA), David Turner (Raspberry Pi), Georges
Stavracas (Igalia), Joan Torres (SUSE), Liviu Dudau (Arm), Louis Chauvet
(Bootlin), Robert Mader (Collabora), Tian Mengge (GravityXR), Victor Jaquez
(Igalia) and Victoria Brekenfeld (System76). It was and awesome opportunity to
meet you and chat face-to-face.

Finally, thanks virtual participants who couldn't make it in person but
organized their days to actively participate in each discussion, adding
different perspectives and valuable inputs even remotely: Abhinav Kumar
(Qualcomm),  Chaitanya Borah (Intel), Christopher Braga (Qualcomm), Dor Askayo
(Red Hat), Jiri Koten (RedHat), Jonas Ådahl (Red Hat), Leandro Ribeiro
(Collabora), Marti Maria (Little CMS), Marijn Suijten, Mario Kleiner, Martin
Stransky (Red Hat), Michel Dänzer (Red Hat), Miguel Casas-Sanchez (Google),
Mitulkumar Golani (Intel), Naveen Kumar (Intel), Niels De Graef (Red Hat),
Pekka Paalanen (Collabora), Pichika Uday Kiran (AMD), Shashank Sharma (AMD),
Sriharsha PV (AMD), Simon Ser, Uma Shankar (Intel) and Vikas Korjani (AMD).

We look forward to another successful Display Next hackfest, continuing to
drive innovation and improvement in the Linux display ecosystem!
