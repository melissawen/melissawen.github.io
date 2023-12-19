---
layout: page
title: "Draft - The Rainbow Treasure Map: Advanced color management on Linux with AMD/Steam Deck."
date: 2023-12-19 09:25:00 -0100
draft: draft
permalink: /draft-xdc2023-color-talk-slide-notes
---

Last week marked a major milestone for me: the
[AMD driver-specific color management properties](https://lore.kernel.org/amd-gfx/20231116195812.906115-1-mwen@igalia.com/)
arrived in [linux-next](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/drivers/gpu/drm/amd/display?id=9342a9ae54ef299ffe5e4ce3d0be6a4da5edba0e)!

And to celebrate, I'm happy to share the
[slides](https://indico.freedesktop.org/event/4/contributions/186/attachments/138/218/xdc2023-TheRainbowTreasureMap-MelissaWen.pdf)
notes from my 2023 XDC talk, "The Rainbow Treasure Map" along with the
[individual recording](https://www.youtube.com/embed/voI0HxhFzbI) that just
dropped last week on youtube – talk about happy coincidences!

<iframe width="560" height="315" src="https://www.youtube.com/embed/voI0HxhFzbI?si=FklKLs1MJCCF-9y-" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>

## Steam Deck Rainbow: Treasure Map & Magic Frogs

While I may be bubbly and chatty in everyday life, the stage isn't exactly my
comfort zone (hallway talks are more my speed). But the journey of developing
the AMD color management properties was so full of discoveries that I simply
had to share the experience. Witnessing the fantastic work of Jeremy and Joshua
bring it all to life on the Steam Deck OLED was like uncovering magical
ingredients and whipping up something truly enchanting.

For XDC 2023, we split our Rainbow journey into two talks. My focus, "The
Rainbow Treasure Map," explored the new color features we added to the Linux
kernel driver, diving deep into the hardware capabilities of AMD/Steam Deck.
Joshua then followed with "The Rainbow Frogs" and showed the breathtaking color
magic released on Gamescope thanks to the power unlocked by the kernel driver's
Steam Deck color properties.

## Packing a Rainbow into 15 Minutes

I had so much to tell, but a half-slot talk meant crafting a concise
presentation. To squeeze everything into 15 minutes (and calm my pre-talk
jitters a bit!), I drafted and practiced those slides and notes countless
times.

So grab your map, and let's embark on the Rainbow journey together!

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-1.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-1.png" alt="Slide 1: The Rainbow Treasure Map - Advanced Color Management on Linux with AMD/SteamDeck" width="750"></a>

Intro: Hi, I'm Melissa from Igalia and welcome to the Rainbow Treasure Map, a
talk about advanced color management on Linux with AMD/SteamDeck.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-2.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-2.png" alt="Slide 2: List useful links for this technical talk" width="750"></a>

Useful links: First of all, if you are not used to the topic, you may find
these links useful.

1. [XDC 2022 - I'm not an AMD expert, but... - Melissa Wen](https://www.youtube.com/watch?v=CMm-yhsMB7U)
2. [XDC 2022 - Is HDR Harder? - Harry Wentland](https://www.youtube.com/watch?v=nDnbWaIMJJA)
3. [XDC 2022 Lightning - HDR Workshop Summary - Harry Wentland](https://www.youtube.com/watch?v=BFNkoNnzYAA)
4. [Color management and HDR documentation for FOSS graphics - Pekka Paalanen et al.](https://gitlab.freedesktop.org/pq/color-and-hdr#color-management-and-hdr-documentation-for-foss-graphics)
5. [Cinematic Color - 2012 SIGGRAPH course notes - Jeremy Selan](https://github.com/jeremyselan/cinematiccolor/blob/master/siggraph2012/cinematic_color.pdf)
6. [AMD Driver-specific Properties for Color Management on Linux (Part 1) - Melissa Wen](https://melissawen.github.io/blog/2023/08/21/amd-steamdeck-colors)

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-3.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-3.png" alt="Slide 3: Why do we need advanced color management on Linux?" width="750"></a>

Context: When we talk about colors in the graphics chain, we should keep in
mind that we have a wide variety of source content colorimetry, a variety of
output display devices and also the internal processing. Users expect
consistent color reproduction across all these devices.

The userspace can use GPU-accelerated color management to get it. But this also
requires an interface with display kernel drivers that is currently missing
from the DRM/KMS framework.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-4.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-4.png" alt="Slide 4: Describe our work on AMD driver-specific color properties" width="750"></a>

Since April, I've been bothering the DRM community by sending patchsets from
the work of me and Joshua to add driver-specific color properties to the AMD
display driver. In parallel, discussions on defining a generic color management
interface are still ongoing in the community. Moreover, we are still not clear
about the diversity of color capabilities among hardware vendors.

To bridge this gap, we defined a color pipeline for Gamescope that fits the
latest versions of AMD hardware. It delivers advanced color management features
for gamut mapping, HDR rendering, SDR on HDR, and HDR on SDR.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-5.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-5.png" alt="Slide 5: Describe the AMD/SteamDeck - our hardware" width="750"></a>

AMD/Steam Deck hardware: AMD frequently releases new GPU and APU generations.
Each generation comes with a DCN version with display hardware improvements.
Therefore, keep in mind that this work uses the AMD Steam Deck hardware and its
kernel driver.  The Steam Deck is an APU with a DCN3.01 display driver, a DCN3
family.

It's important to have this information since newer AMD DCN drivers inherit
implementations from previous families but aldo each generation of AMD hardware
may introduce new color capabilities. Therefore I recommend you to familiarize
yourself with the hardware you are working on.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-6.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-6.png" alt="Slide 6: Diagram with the three layers of the AMD display driver on Linux" width="750"></a>

The AMD display driver in the kernel space: It consists of three layers, (1)
the DRM/KMS framework, (2) the AMD Display Manager, and (3) the AMD Display
Core. We extended the color interface exposed to userspace by leveraging
existing DRM resources and connecting them using driver-specific functions for
color property management.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-7.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-7.png" alt="Slide 7: Three-layers diagram highlighting AMD Display Manager, DM - the layer that connects DC and DRM" width="750"></a>

Bridging DC color capabilities and the DRM API required significant changes in
the color management of AMD Display Manager - the Linux-dependent part that
connects the AMD DC interface to the DRM/KMS framework.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-8.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-8.png" alt="Slide 8: Three-layers diagram highlighting AMD Display Core, DC - the shared code" width="750"></a>

The AMD DC is the OS-agnostic layer. Its code is shared between platforms and
DCN versions. Examining this part helps us understand the AMD color pipeline
and hardware capabilities, since the machinery for hardware settings and
resource management are already there.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-9.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-9.png" alt="Slide 9: Diagram of the AMD Display Core Next architecture with main elements and data flow" width="750"></a>

The newest architecture for AMD display hardware is the AMD Display Core Next.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-10.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-10.png" alt="Slide 10: Diagram of the AMD Display Core Next where only DPP and MPC blocks are highlighted" width="750"></a>

In this architecture, two blocks have the capability to manage colors:
* Display Pipe and Plane (DPP) - for pre-blending adjustments;
* Multiple Pipe/Plane Combined (MPC) - for post-blending color transformations.

Let's see what we have in the DRM API for pre-blending color management.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-11.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-11.png" alt="Slide 11: Blank slide with no content only a title 'Pre-blending: DRM plane'" width="750"></a>

DRM plane color properties:

This is the DRM color management API before blending.

Nothing!

Except two basic DRM plane properties: `color_encoding` and `color_range` for
the input colorspace conversion, that is not covered by this work.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-12.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-12.png" alt="Slide 12: Diagram with color capabilities and structures in AMD DC layer without any DRM plane color interface (before blending), only the DRM CRTC color interface for post blending" width="750"></a>

In case you're not familiar with AMD shared code, what we need to do is
basically draw a map and navigate there!

We have some DRM color properties after blending, but nothing before blending
yet. But much of the hardware programming was already implemented in the AMD DC
layer, thanks to the shared code.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-13.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-13.png" alt="Slide 13: Previous Diagram with a rectangle to highlight the empty space in the DRM plane interface that will be filled by AMD plane properties" width="750"></a>

Still both the DRM interface and its connection to the shared code were
missing. That's when the search begins!

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-14.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-14.png" alt="Slide 14: Color Pipeline Diagram with the plane color interface filled by AMD plane properties but without connections to AMD DC resources" width="750"></a>

AMD driver-specific color pipeline:

Looking at the color capabilities of the hardware, we arrive at this initial
set of properties. The path wasn't exactly like that. We had many iterations
and discoveries until reached to this pipeline.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-15.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-15.png" alt="Slide 15: Color Pipeline Diagram connecting AMD plane degamma properties, LUT and TF, to AMD DC resources" width="750"></a>

The Plane Degamma is our first driver-specific property before blending. It's
used to linearize the color space from encoded values to light linear values.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-16.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-16.png" alt="Slide 16: Describe plane degamma properties and hardware capabilities" width="750"></a>

We can use a pre-defined transfer function or a user lookup table (in short,
LUT) to linearize the color space.

Pre-defined transfer functions for plane degamma are hardcoded curves that go
to a specific hardware block called DPP Degamma ROM. It supports the following
transfer functions: sRGB EOTF, BT.709 inverse OETF, PQ EOTF, and pure power
curves Gamma 2.2, Gamma 2.4 and Gamma 2.6.

We also have a one-dimensional LUT. This 1D LUT has four thousand ninety six
(4096) entries, the usual 1D LUT size in the DRM/KMS. It's an array of
`drm_color_lut` that goes to the DPP Gamma Correction block.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-17.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-17.png" alt="Slide 17: Color Pipeline Diagram connecting AMD plane CTM property to AMD DC resources" width="750"></a>

We also have now a color transformation matrix (CTM) for color space
conversion.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-18.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-18.png" alt="Slide 18: Describe plane CTM property and hardware capabilities" width="750"></a>

It's a 3x4 matrix of fixed points that goes to the DPP Gamut Remap Block.

Both pre- and post-blending matrices were previously gone to the same color
block. We worked on detaching them to clear both paths.

Now each CTM goes on its own way.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-19.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-19.png" alt="Slide 19: Color Pipeline Diagram connecting AMD plane HDR multiplier property to AMD DC resources" width="750"></a>

Next, the HDR Multiplier. HDR Multiplier is a factor applied to the color
values of an image to increase their overall brightness.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-20.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-20.png" alt="Slide 20: Describe plane HDR mult property and hardware capabilities" width="750"></a>

This is useful for converting images from a standard dynamic range (SDR) to a
high dynamic range (HDR).  As it can range beyond [0.0, 1.0] subsequent
transforms need to use the PQ(HDR) transfer functions.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-21.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-21.png" alt="Slide 21: Color Pipeline Diagram connecting AMD plane shaper properties, LUT and TF, to AMD DC resources" width="750"></a>

And we need a 3D LUT. But 3D LUT has a limited number of entries in each
dimension, so we want to use it in a colorspace that is optimized for human
vision. It means in a non-linear space.  To deliver it, userspace may need one
1D LUT before 3D LUT to delinearize content and another one after to linearize
content again for blending.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-22.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-22.png" alt="Slide 22: Describe plane shaper properties and hardware capabilities" width="750"></a>

The pre-3D-LUT curve is called Shaper curve. Unlike Degamma TF, there are no
hardcoded curves for shaper TF, but we can use the AMD color module in the
driver to build the following shaper curves from pre-defined coefficients. The
color module combines the TF and the user LUT values into the LUT that goes to
the DPP Shaper RAM block.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-23.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-23.png" alt="Slide 23: Color Pipeline Diagram connecting AMD plane 3D LUT property to AMD DC resources" width="750"></a>

Finally, our rockstar, the 3D LUT. 3D LUT is perfect for complex color
transformations and adjustments between color channels.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-24.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-24.png" alt="Slide 24: Describe plane 3D LUT property and hardware capabilities" width="750"></a>

3D LUT is also more complex to manage and requires more computational
resources, as a consequence, its number of entries is usually limited.  To
overcome this restriction, the array contains samples from the approximated
function and values between samples are estimated by tetrahedral interpolation.
AMD supports 17 and 9 as the size of a single-dimension. Blue is the outermost
dimension, red the innermost.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-25.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-25.png" alt="Slide 25: Color Pipeline Diagram connecting AMD plane blend properties, LUT and TF, to AMD DC resources" width="750"></a>

As mentioned, we need a post-3D-LUT curve to linearize the color space before
blending. This is done by Blend TF and LUT.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-26.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-26.png" alt="Slide 26: Describe plane blend properties and hardware capabilities" width="750"></a>

Similar to shaper TF, there are no hardcoded curves for Blend TF. The
pre-defined curves are the same as the Degamma block, but calculated by the
color module. The resulting LUT goes to the DPP Blend RAM block.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-27.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-27.png" alt="Slide 27: Color Pipeline Diagram  with all AMD plane color properties connect to AMD DC resources and links showing the conflict between plane and CRTC degamma" width="750"></a>

Now we have everything connected before blending. As a conflict between plane
and CRTC Degamma was inevitable, our approach doesn't accept that both are set
at the same time.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-28.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-28.png" alt="Slide 28: Color Pipeline Diagram connecting AMD CRTC gamma TF property to AMD DC resources" width="750"></a>

We also optimized the conversion of the framebuffer to wire encoding by adding
support to pre-defined CRTC Gamma TF.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-29.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-29.png" alt="Slide 29: Describe CRTC gamma TF property and hardware capabilities" width="750"></a>

Again, there are no hardcoded curves and TF and LUT are combined by the AMD
color module.  The same types of shaper curves are supported. The resulting LUT
goes to the MPC Gamma RAM block.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-30.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-30.png" alt="Slide 30: Color Pipeline Diagram with all AMD driver-specific color properties connect to AMD DC resources" width="750"></a>

Finally, we arrived in the final version of DRM/AMD driver-specific color
management pipeline. With this knowledge, you’re ready to better enjoy the
rainbow treasure of AMD display hardware and the world of graphics computing.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-31.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-31.png" alt="Slide 31: SteamDeck/Gamescope Color Pipeline Diagram with rectangles labeling each block of the pipeline with the related AMD color property" width="750"></a>

With this work, Gamescope/Steam Deck embraces the color capabilities of the AMD
GPU. We highlight here how we map the Gamescope color pipeline to each AMD
color block.

<a href="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-32.png"><img src="https://raw.githubusercontent.com/melissawen/melissawen.github.io/master/img/xdc-2023-colors-talk/rainbow-treasure-xdc-2023-32.png" alt="Slide 32: Final slide. Thank you!" width="750"></a>

Future works:
The search for the rainbow treasure is not over! The Linux DRM subsystem
contains many hidden treasures from different vendors. We want more complex
color transformations and adjustments available on Linux. We also want to
expose all GPU color capabilities from all hardware vendors to the Linux
userspace.

Thanks Joshua and Harry for this joint work and the Linux DRI community for all feedback and reviews.

The amazing part of this work comes next with Joshua and The Rainbow Frogs!
Stay tuned!

Any questions?

---

References:

1. [Slides of the talk The Rainbow Treasure Map](https://indico.freedesktop.org/event/4/contributions/186/attachments/138/218/xdc2023-TheRainbowTreasureMap-MelissaWen.pdf).
2. [Youtube video of the talk The Rainbow Treasure Map](https://www.youtube.com/embed/voI0HxhFzbI).
3. [Patch series for AMD driver-specific color management properties](https://lore.kernel.org/amd-gfx/20231116195812.906115-1-mwen@igalia.com/) (upstream Linux 6.8v).
4. [SteamDeck/Gamescope color management pipeline](https://github.com/ValveSoftware/gamescope/blob/master/src/docs/Steam%20Deck%20Display%20Pipeline.png)
5. [XDC 2023 website](https://indico.freedesktop.org/event/4/page/21-overview).
6. [Igalia website](https://www.igalia.com/).
