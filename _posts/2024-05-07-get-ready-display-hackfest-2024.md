---
layout: post
title: "Get Ready to 2024 Linux Display Next Hackfest in A Coruña!"
date: 2024-05-07 11:33:00 -0300
categories: igalia
---

We're excited to announce the details of our upcoming 2024 Linux Display Next
Hackfest in the beautiful city of A Coruña, Spain!

This year's hackfest will be hosted by Igalia and will take place from May 14th
to 16th. It will be a gathering of minds from a diverse range of companies and
open source projects, all coming together to share, learn, and collaborate
outside the traditional conference format.

### Who's Joining the Fun?

We're excited to welcome participants from various backgrounds, including:
- GPU hardware vendors;
- Linux distributions;
- Linux desktop environments and compositors;
- Color experts, researchers and enthusiasts;

This diverse mix of backgrounds are represented by developers from several
companies working on the Linux display stack: AMD, Arm, BlueSystems, Bootlin,
Collabora, Google, GravityXR, Igalia, Intel, LittleCMS, Qualcomm, Raspberry Pi,
RedHat, SUSE, and System76. It'll ensure a dynamic exchange of perspectives and
foster collaboration across the  Linux Display community.

Please take a look at the
[list of participants](https://github.com/melissawen/2024linuxdisplayhackfest/wiki/List-of-participants)
for more info.

### What's on the Agenda?

The beauty of the hackfest is that the agenda is driven by participants!  As
this is a hybrid event, we decided to improve the experience for remote
participants by creating a dedicated space for them to propose topics and some
introductory talks in advance. From those inputs, we defined a schedule that
reflects the collective interests of the group, but is still open for
amendments and new proposals. Find the schedule details in [the official event
webpage](https://events.pages.igalia.com/linuxdisplaynexthackfest/).

Expect discussions on:

###### KMS Color/HDR
- Proposal with new DRM object type:
  - Brief presentation of GPU-vendor features;
  - Status update of plane color management pipeline per vendor on Linux;
- HDR/Color Use-cases:
  - HDR gainmap images and how should we think about HDR;
  - Google/ChromeOS GFX view about HDR/per-plane color management, VKMS and lessons learned;
- Post-blending Color Pipeline.
- Color/HDR testing/CI
  - VKMS status update;
  - Chamelium boards, video capture.
- Wayland protocols
  - color-management protocol status update;
  - color-representation and video playback.

###### Display control
  - HDR signalling status update;
  - backlight status update;
  - EDID and DDC/CI.

###### Strategy for video and gaming use-cases
- Multi-plane support in compositors
  - Underlay, overlay, or mixed strategy for video and gaming use-cases;
  - KMS Plane UAPI to simplify the plane arrangement problem;
  - Shared plane arrangement algorithm desired.
- HDR video and hardware overlay

###### Frame timing and VRR
- Frame timing:
  - Limitations of uAPI;
  - Current user space solutions;
  - Brainstorm better uAPI;
- Cursor/overlay plane updates with VRR;
- KMS commit and buffer-readiness deadlines;

###### Power Saving vs Color/Latency
- ABM (adaptive backlight management);
- PSR1 latencies;
- Power optimization vs color accuracy/latency requirements.

###### Content-Adaptive Scaling & Sharpening
- Content-Adaptive Scalers on display hardware;
- New drm\_colorop for content adaptive scaling;
- Proprietary algorithms.

###### Display Mux
- Laptop muxes for switching of the embedded panel between the integrated GPU and the discrete GPU;
- Seamless/atomic hand-off between drivers on Linux desktops.

###### Real time scheduling & async KMS API
- Potential benefits: lower latency input feedback, better VRR handling, buffer synchronization, etc.
- Issues around “async” uAPI usage and async-call handling.

### In-person, but also geographically-distributed event

This year Linux Display Next hackfest is a hybrid event, hosted onsite at the
Igalia offices and available for remote attendance. In-person participants will
find an environment for networking and brainstorming in our inspiring and
collaborative office space. Additionally, A Coruña itself is a gem waiting to
be explored, with stunning beaches, good food, and historical sites.

![](https://webengineshackfest.org/2014/img/lectures/web-engines-hackfest-2014.jpg "Igalia HQ: people, laptops, round tables and rooms")

### Semi-structured structure: how the 2024 Linux Display Next Hackfest will work

- **Agenda:** Participants proposed the topics and talks for discussing in
  sessions.
- **Interactive Sessions:** Discussions, workshops, introductory talks and
  brainstorming sessions lasting around 1h30. There is always a starting point
  for discussions and new ideas will emerge in real time.
- **Immersive experience:** We will have coffee-breaks between sessions and
  lunch time at the office for all in-person participants. Lunches and
  coffee-breaks are sponsored by Igalia. This will keep us sharing knowledge and
  in continuous interaction.
- **Spaces for all group sizes:** In-person participants will find different
  room sizes that match various group sizes at Igalia HQ. Besides that, there
  will be some devices for showcasing and real-time demonstrations.

### Social Activities: building connections beyond the sessions

To make the most of your time in A Coruña, we'll be organizing some social activities:

- **First-day Dinner:** In-person participants will enjoy a Galician dinner on
  Tuesday, after a first day of intensive discussions in the hackfest.
- **Getting to know a little of A Coruña:** Finding out a little about A Coruña
  and current local habits.


<a href="https://mundoestrellagalicia.es/dentro-de-mega/">
<img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/01_06_19_EstrellaGALICIA_MEGA_0468.jpg.webp" alt="Participants of a guided tour in one of the sectors of the Museum of Estrella Galicia (MEGA). Source: mundoestrellagalicia.es" style="display: inline;">
</a>

  - On Thursday afternoon, we will close the 2024 Linux Display Next hackfest
    with a guided tour of the Museum of Galicia's favorite beer brand, Estrella
    Galicia. The guided tour covers the eight sectors of the museum and ends with
    beer pouring and tasting. After this experience, a transfer bus will take us to
    the Maria Pita square.
  - At Maria Pita square we will see the charm of some historical landmarks of
    A Coruña, explore the casual and vibrant style of the city center and taste
    local foods while chatting with friends.

#### Sponsorship

Igalia sponsors lunches and coffee-breaks on hackfest days, Tuesday's dinner,
and the social event on Thursday afternoon for in-person participants.

![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/igalia-logo.svg "Igalia logo")

We can't wait to welcome hackfest attendees to A Coruña! Stay tuned for further
details and outcomes of this unconventional and unique experience. 
