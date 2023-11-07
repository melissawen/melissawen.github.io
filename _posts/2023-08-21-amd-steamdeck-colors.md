---
layout: post
title: "AMD Driver-specific Properties for Color Management on Linux (Part 1)"
date: 2023-08-21 10:13:00 -0100
categories: igalia
---

### TL;DR:

Color is a visual perception. Human eyes can detect a broader range of colors
than any devices in the graphics chain. Since each device can generate, capture
or reproduce a specific subset of colors and tones, color management controls
color conversion and calibration across devices to ensure a more consistent
color reproduction. We can expose a GPU-accelerated display color management
pipeline to support this process and enhance results, and this is what we are
doing on Linux to improve color management on `Gamescope/SteamDeck`. Even with
the challenges of being external developers, we have been working on mapping
`AMD GPU color capabilities` to `the Linux kernel color management interface`,
which is a combination of DRM and AMD driver-specific color properties. This
more extensive color management pipeline includes `pre-defined Transfer
Functions`, `1-Dimensional LookUp Tables (1D LUTs)`, and `3D LUTs` before and
after the plane composition/blending.

---

The study of color is well-established and has been explored for many years.
Color science and research findings have also guided technology innovations. As
a result, color in Computer Graphics is a very complex topic that I'm putting a
lot of effort into becoming familiar with. I always find myself rereading all
the materials I have collected about color space and operations since I started
this journey (about one year ago). I also understand how hard it is to find
consensus on some color subjects, as exemplified by all explanations around the
2015 online viral phenomenon of [The Black and Blue
Dress](https://en.m.wikipedia.org/wiki/The_dress). Have you heard about it?
What is the color of the dress for you?

So, taking into account my skills with colors and building consensus, this blog
post only focuses on GPU hardware capabilities to support color management :-D
If you want to learn more about color concepts and color on Linux, you can find
[useful links](#useful-links) at the end of this blog post.

## Linux Kernel, show me the colors ;D

DRM color management interface only exposes a small set of post-blending color
properties. Proposals to enhance the DRM color API from different vendors have
landed the subsystem mailing list over the last few years. On one hand, we got
some suggestions to extend DRM post-blending/CRTC color API: [DRM CRTC 3D LUT
for R-Car (2020
version)](https://lore.kernel.org/all/20201221015730.28333-1-laurent.pinchart+renesas@ideasonboard.com/);
[DRM CRTC 3D LUT for Intel (draft -
2020)](https://github.com/vsyrjala/linux/commit/4d28e8ddf2a076f30f9e5bdc17cbb4656fe23e69);
[DRM CRTC 3D LUT for AMD by Igalia (v2 -
2023)](https://lore.kernel.org/dri-devel/20230109143846.1966301-1-mwen@igalia.com/);
[DRM CRTC 3D LUT for R-Car (v2 -
2023)](https://lore.kernel.org/dri-devel/20230621081031.7876-1-jacopo.mondi@ideasonboard.com/).
On the other hand, some proposals to extend DRM pre-blending/plane API: [DRM
plane colors for Intel (v2 -
2021)](https://lore.kernel.org/dri-devel/20210906213904.27918-1-uma.shankar@intel.com/);
[DRM plane API for AMD (v3 -
2021)](https://lore.kernel.org/dri-devel/20210730204134.21769-1-harry.wentland@amd.com/);
[DRM plane 3D LUT for AMD -
2021](https://lore.kernel.org/dri-devel/20221004211451.1475215-1-alex.hung@amd.com/).
Finally, Simon Ser sent the latest proposal in May 2023: [Plane color pipeline KMS
uAPI](https://lore.kernel.org/dri-devel/QMers3awXvNCQlyhWdTtsPwkp5ie9bze_hD5nAccFW7a_RXlWjYB7MoUW_8CKLT2bSQwIXVi5H6VULYIxCdgvryZoAoJnC5lZgyK1QWn488=@emersion.fr/),
from discussions in the [2023 Display/HDR
Hackfest](https://wiki.gnome.org/action/login/Hackfests/ShellDisplayNext2023),
and it is still under evaluation by the Linux Graphics community.

All previous proposals seek a generic solution for expanding the API, but many
seem to have stalled due to the uncertainty of matching well the hardware
capabilities of all vendors. Meanwhile, the use of AMD color capabilities on
Linux remained limited by the DRM interface, as the `DCN 3.0 family color caps
and mapping` diagram below shows the Linux/DRM color interface without
driver-specific color properties [\*]:

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/dcn301_wo_private.svg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/dcn301_wo_private.svg "The DRM interface on the right. AMD display driver color capabilities on the left.")

Bearing in mind that we need to know the variety of color pipelines in the
subsystem to be clear about a generic solution, we decided to approach the
issue from a different perspective and worked on enabling a set of
`Driver-Specific Color Properties for AMD Display Drivers`. As a result, I
recently sent another round of the [AMD driver-specific color mgmt
API](https://lore.kernel.org/amd-gfx/20230810160314.48225-1-mwen@igalia.com).

For those who have been following the AMD driver-specific proposal since the
beginning (see
[\[RFC\]](https://lore.kernel.org/dri-devel/20230423141051.702990-1-mwen@igalia.com)[\[V1\]](https://lore.kernel.org/dri-devel/20230523221520.3115570-1-mwen@igalia.com)),
the main new features of the latest version
[\[v2\]](https://lore.kernel.org/amd-gfx/20230810160314.48225-1-mwen@igalia.com)
are the addition of `pre-blending Color Transformation Matrix (plane CTM)` and
the differentiation of `Pre-defined Transfer Functions (TF)` supported by color
blocks. For those who just got here, I will recap this work in two blog posts.
This one describes the current status of the AMD display driver in the Linux
kernel/DRM subsystem and what changes with the driver-specific properties. In
the next post, we go deeper to describe the features of each color block and
provide a better picture of what is available in terms of color management for
Linux.

## The Linux kernel color management API and AMD hardware color capabilities

Before discussing colors in the Linux kernel with AMD hardware, consider
accessing the Linux kernel documentation (version 6.5.0-rc5). In the AMD
Display documentation, you will find my previous work documenting AMD hardware
color capabilities and the [Color Management
Properties](https://docs.kernel.org/gpu/amdgpu/display/display-manager.html#color-management-properties).
It describes how `AMD Display Manager (DM)` intermediates requests between the
`AMD Display Core component (DC)` and the `Linux/DRM kernel` interface for
color management features. It also describes the relevant function to call the
AMD color module in building curves for content space transformations.

A subsection also describes hardware color capabilities and how they evolve
between versions. This subsection, [DC Color Capabilities between DCN
generations](https://docs.kernel.org/gpu/amdgpu/display/display-manager.html#dc-color-capabilities-between-dcn-generations),
is a good starting point to understand what we have been doing on the kernel
side to provide a broader color management API with AMD driver-specific
properties.

## Why do we need more kernel color properties on Linux?

Blending is the process of combining multiple planes (framebuffers abstraction)
according to their mode settings. Before blending, we can manage the colors of
various planes separately; after blending, we have combined those planes in
only one output per CRTC. Color conversions after blending would be enough in a
single-plane scenario or when dealing with planes in the same color space on
the kernel side. Still, it cannot help to handle the blending of multiple
planes with different color spaces and luminance levels. With plane color
management properties, userspace can get better representation of
colors to deal with the diversity of color profiles of devices in the graphics
chain, bring a `wide color gamut (WCG)`, convert `High-Dynamic-Range (HDR)`
content to `Standard-Dynamic-Range (SDR)` content (and vice-versa). With a
GPU-accelerated display color management pipeline, we can use hardware blocks
for color conversions and color mapping and support advanced color management.

The current DRM color management API enables us to perform some color
conversions after blending, but there is no interface to calibrate input space
by planes. Note that here I'm not considering some workarounds in the AMD
display manager mapping of DRM CRTC de-gamma and DRM CRTC CTM property to
pre-blending DC de-gamma and gamut remap block, respectively. So, in more
detail, it only exposes three post-blending features:
* *DRM CRTC de-gamma:* used to convert the framebuffer's colors to linear gamma;
* *DRM CRTC CTM:* used for color space conversion;
* *DRM CRTC gamma:* used to convert colors to the gamma space of the connected screen.

## AMD driver-specific color management interface

We can compare the Linux color management API with and without the
driver-specific color properties. From now, we denote driver-specific
properties with the AMD prefix and generic properties with the DRM prefix. For
visual comparison, I bring the `DCN 3.0 family color caps and mapping` diagram
closer and present it here again:

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/dcn301_wo_private.svg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/dcn301_wo_private.svg "DRM color management on AMD HW without driver-specific properties")

Mixing AMD driver-specific color properties with DRM generic color properties,
we have a broader Linux color management system with the following features
exposed by properties in the plane and CRTC interface, as summarized by this
updated diagram:

[![](https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/dcn301_private.svg)](https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/dcn301_private.svg "DRM color management on AMD HW with driver-specific properties")

The blocks highlighted by `red lines` are the `new properties` in the
driver-specific interface developed by me (Igalia) and Joshua (Valve). The `red
dashed lines` are new `links between API and AMD driver components` implemented by
us to connect the Linux/DRM interface to AMD hardware blocks, mapping
components accordingly. In short, we have the following color management
properties exposed by the DRM/AMD display driver:
- **Pre-blending - AMD Display Pipe and Plane (DPP):**
   - ***AMD plane de-gamma:*** 1D LUT and pre-defined transfer functions; used to linearize the input space of a plane;
   - ***AMD plane CTM:*** 3x4 matrix; used to convert plane color space;
   - ***AMD plane shaper:*** 1D LUT and pre-defined transfer functions; used to delinearize and/or normalize colors before applying 3D LUT;
   - ***AMD plane 3D LUT:*** 17x17x17 size with 12 bit-depth; three dimensional lookup table used for advanced color mapping;
   - ***AMD plane blend/out gamma:*** 1D LUT and pre-defined transfer functions; used to linearize back the color space after 3D LUT for blending.
- **Post-blending - AMD Multiple Pipe/Plane Combined (MPC):**
   - ***DRM CRTC de-gamma:*** 1D LUT *(can't be set together with plane de-gamma)*;
   - ***DRM CRTC CTM:*** 3x3 matrix *(remapped to post-blending matrix)*;
   - ***DRM CRTC gamma:*** 1D LUT + AMD CRTC gamma TF; added to take advantage of driver pre-defined transfer functions;

Note: You can find more about AMD display blocks in the [Display Core Next
(DCN) - Linux kernel
documentation](https://dri.freedesktop.org/docs/drm/gpu/amdgpu/display/dcn-overview.html),
provided by Rodrigo Siqueira (Linux/AMD display developer) in a
[2021-documentation
series](https://lore.kernel.org/dri-devel/20211207145350.286037-1-Rodrigo.Siqueira@amd.com/).
In the next post, I’ll revisit this topic, explaining display and color blocks
in detail.

## How did we get a large set of color features from AMD display hardware?

So, looking at AMD hardware color capabilities in the first diagram, we can see
no post-blending (MPC) de-gamma block in any hardware families. We can also see
that the AMD display driver maps CRTC/post-blending CTM to pre-blending (DPP)
gamut\_remap, but there is post-blending (MPC) gamut\_remap (DRM CTM) from newer
hardware versions that include SteamDeck hardware. You can find more details
about hardware versions in the [Linux kernel documentation/AMDGPU Product
Information](https://docs.kernel.org/gpu/amdgpu/driver-misc.html#gpu-product-information).

I needed to rework these two mappings mentioned above to provide
pre-blending/plane de-gamma and CTM for SteamDeck. I changed the DC mapping to
detach `stream gamut remap` matrixes from the `DPP gamut remap` block. That
means mapping AMD plane CTM directly to DPP/pre-blending gamut remap block and
DRM CRTC CTM to MPC/post-blending gamut remap block. In this sense, I also
limited plane CTM properties to those hardware versions with MPC/post-blending
gamut\_remap capabilities since older versions cannot support this feature
without clashes with DRM CRTC CTM.

Unfortunately, I couldn't prevent conflict between AMD plane de-gamma and DRM
plane de-gamma since post-blending de-gamma isn't available in any AMD hardware
versions until now. The fact is that a post-blending de-gamma makes little
sense in the AMD color pipeline, where plane blending works better in a linear
space, and there are enough color blocks to linearize content before blending.
To deal with this conflict, the driver now rejects atomic commits if users try
to set both AMD plane de-gamma and DRM CRTC de-gamma simultaneously.

Finally, we had no other clashes when enabling other AMD driver-specific color
properties for our use case, Gamescope/SteamDeck. Our main work for the
remaining properties was understanding the data flow of each property, the
hardware capabilities and limitations, and how to shape the data for
programming the registers - AMD color block capabilities (and limitations) are
the topics of the next blog post. Besides that, we fixed some driver bugs along
the way since it was the first Linux use case for most of the new color
properties, and some behaviors are only exposed when exercising the engine.

Take a look at the [Gamescope/Steam Deck Color
Pipeline](https://github.com/ValveSoftware/gamescope/blob/master/src/docs/Steam%20Deck%20Display%20Pipeline.png)[\*\*],
and see how Gamescope uses the new API to manage color space conversions
and calibration (please click on the image for a better view):

[![](https://github.com/melissawen/melissawen.github.io/blob/master/img/Steam_Deck_Display_Pipeline_dcn.png?raw=true)](https://github.com/melissawen/melissawen.github.io/blob/master/img/Steam_Deck_Display_Pipeline_dcn.png?raw=true)

In the next blog post, I'll describe the implementation and technical details
of each pre- and post-blending color block/property on the AMD display driver.

\* Thank Harry Wentland for helping with diagrams, color concepts and AMD capabilities.

\*\* Thank Joshua Ashton for providing and explaining Gamescope/Steam Deck color pipeline.

\*\*\* Thanks to the Linux Graphics community - explicitly Harry, Joshua,
Pekka, Simon, Sebastian, Siqueira, Alex H. and Ville - to all the learning
during this Linux DRM/AMD color journey. Also, Carlos and Tomas for organizing
the 2023 Display/HDR Hackfest where we have a great and immersive opportunity
to discuss Color & HDR on Linux.

## Useful Links

1. [Cinematic Color - 2012 SIGGRAPH course notes by Jeremy
   Selan](https://github.com/jeremyselan/cinematiccolor/blob/master/siggraph2012/cinematic_color.pdf):
an introduction to color science, concepts and pipelines.
2. [Color management and HDR documentation for FOSS graphics by Pekka
   Paalanen](https://gitlab.freedesktop.org/pq/color-and-hdr#color-management-and-hdr-documentation-for-foss-graphics):
documentation and useful links on applying color concepts to the Linux graphics
stack.
3. [HDR in Linux by Jeremy
   Cline](https://github.com/jeremycline/jcline.org/blob/main/_posts/2021-04-30-hdr-in-linux-p1.md):
a blog post exploring color concepts for HDR support on Linux.
4. [Methods for conversion of high dynamic range content to standard dynamic
   range content and vice-versa by
ITU-R](https://www.itu.int/dms_pub/itu-r/opb/rep/R-REP-BT.2446-2019-PDF-E.pdf):
guideline for conversions between HDR and SDR contents.
5. [Using Lookup Tables to Accelerate Color Transformations by Jeremy
   Selan](https://developer.nvidia.com/gpugems/gpugems2/part-iii-high-quality-rendering/chapter-24-using-lookup-tables-accelerate-color):
Nvidia blog post about Lookup Tables on color management.
6. [The Importance of Being Linear by Larry Gritz and Eugene
   d'Eon](https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-24-importance-being-linear):
Nvidia blog post about gamma and color conversions.
