---
layout: post
title: "A Look at the Latest Linux KMS Color API Developments on AMD and Intel"
date: 2025-05-19 18:05:00 -0300
categories: igalia
---

This week, I reviewed the [last available version of the Linux KMS Color
API](https://lore.kernel.org/dri-devel/20250430011115.223996-1-alex.hung@amd.com/).
Specifically, I explored the proposed API by Harry Wentland and Alex Hung
(AMD), their implementation for the AMD display driver and tracked the parallel
[efforts of Uma Shankar and Chaitanya Kumar Borah
(Intel)](https://lore.kernel.org/dri-devel/20250312072425.3099205-1-uma.shankar@intel.com/)
in bringing this plane color management to life. With this API in place,
compositors will be able to provide better HDR support and advanced color
management for Linux users.

To get a hands-on feel for the API's potential, I developed a fork of
`drm_info` compatible with the new color properties. This allowed me to
visualize the display hardware color management capabilities being exposed. If
you're curious and want to peek behind the curtain, you can find my exploratory
work on the
[drm\_info/kms\_color branch](https://gitlab.freedesktop.org/mwen/drm_info/-/commits/kms_color).
The README there will guide you through the simple compilation and installation
process.

*Note: You will need to update libdrm to match the proposed API. You can find
an updated version in my personal repository
[here](https://gitlab.freedesktop.org/mwen/drm/-/tree/kms_color). To avoid
potential conflicts with your official `libdrm` installation, you can compile
and install it in a local directory. Then, use the following command: `export
LD_LIBRARY_PATH="/usr/local/lib/"`*

In this post, I invite you to familiarize yourself with the new API that is
about to be released. You can start doing as I did below: just deploy a custom
kernel with the necessary patches and visualize the interface with the help of
`drm_info`.  Or, better yet, if you are a userspace developer, you can start
developing user cases by experimenting with it.

The more eyes the better.

### KMS Color API on AMD

The great news is that AMD's driver implementation for plane color operations
is being developed right alongside their Linux KMS Color API proposal, so it's
easy to apply to your kernel branch and check it out. You can find details of
their progress in
[the AMD's series](https://lore.kernel.org/dri-devel/20250430011115.223996-1-alex.hung@amd.com/).

I just needed to compile a custom kernel with this series applied,
intentionally leaving out the  `AMD_PRIVATE_COLOR` flag. The
`AMD_PRIVATE_COLOR` flag guards driver-specific color plane properties, which
experimentally expose hardware capabilities while we don't have the generic KMS
plane color management interface available.

If you don't know or don't remember the details of AMD driver specific color
properties, you can learn more about this work in my blog posts
[\[1\]](https://melissawen.github.io/blog/2023/08/21/amd-steamdeck-colors)
[\[2\]](https://melissawen.github.io/blog/2023/11/07/amd-steamdeck-colors-p2)
[\[3\]](https://melissawen.github.io/blog/2023/12/20/xdc2023-colors-talk).
As driver-specific color properties and KMS colorops are redundant, the driver
only advertises one of them, as you can see in
[AMD workaround patch 24](https://lore.kernel.org/dri-devel/20250430011115.223996-25-alex.hung@amd.com/).

So, with the custom kernel image ready, I installed it on a system powered by
AMD DCN3 hardware (i.e. my Steam Deck). Using
[my custom drm\_info](https://gitlab.freedesktop.org/mwen/drm_info/-/tree/kms_color),
I could clearly see the Plane Color Pipeline with eight color operations as
below:

```
└───"COLOR_PIPELINE" (atomic): enum {Bypass, Color Pipeline 258} = Bypass
    ├───Bypass
    └───Color Pipeline 258
        ├───Color Operation 258
        │   ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, Multiplier, 3D LUT} = 1D Curve
        │   ├───"BYPASS" (atomic): range [0, 1] = 1
        │   └───"CURVE_1D_TYPE" (atomic): enum {sRGB EOTF, PQ 125 EOTF, BT.2020 Inverse OETF} = sRGB EOTF
        ├───Color Operation 263
        │   ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, Multiplier, 3D LUT} = Multiplier
        │   ├───"BYPASS" (atomic): range [0, 1] = 1
        │   └───"MULTIPLIER" (atomic): range [0, UINT64_MAX] = 0
        ├───Color Operation 268
        │   ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, Multiplier, 3D LUT} = 3x4 Matrix
        │   ├───"BYPASS" (atomic): range [0, 1] = 1
        │   └───"DATA" (atomic): blob = 0
        ├───Color Operation 273
        │   ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, Multiplier, 3D LUT} = 1D Curve
        │   ├───"BYPASS" (atomic): range [0, 1] = 1
        │   └───"CURVE_1D_TYPE" (atomic): enum {sRGB Inverse EOTF, PQ 125 Inverse EOTF, BT.2020 OETF} = sRGB Inverse EOTF
        ├───Color Operation 278
        │   ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, Multiplier, 3D LUT} = 1D LUT
        │   ├───"BYPASS" (atomic): range [0, 1] = 1
        │   ├───"SIZE" (atomic, immutable): range [0, UINT32_MAX] = 4096
        │   ├───"LUT1D_INTERPOLATION" (immutable): enum {Linear} = Linear
        │   └───"DATA" (atomic): blob = 0
        ├───Color Operation 285
        │   ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, Multiplier, 3D LUT} = 3D LUT
        │   ├───"BYPASS" (atomic): range [0, 1] = 1
        │   ├───"SIZE" (atomic, immutable): range [0, UINT32_MAX] = 17
        │   ├───"LUT3D_INTERPOLATION" (immutable): enum {Tetrahedral} = Tetrahedral
        │   └───"DATA" (atomic): blob = 0
        ├───Color Operation 292
        │   ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, Multiplier, 3D LUT} = 1D Curve
        │   ├───"BYPASS" (atomic): range [0, 1] = 1
        │   └───"CURVE_1D_TYPE" (atomic): enum {sRGB EOTF, PQ 125 EOTF, BT.2020 Inverse OETF} = sRGB EOTF
        └───Color Operation 297
            ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, Multiplier, 3D LUT} = 1D LUT
            ├───"BYPASS" (atomic): range [0, 1] = 1
            ├───"SIZE" (atomic, immutable): range [0, UINT32_MAX] = 4096
            ├───"LUT1D_INTERPOLATION" (immutable): enum {Linear} = Linear
            └───"DATA" (atomic): blob = 0

```

Note that Gamescope is currently using
[AMD driver-specific color properties](https://melissawen.github.io/blog/2023/12/20/xdc2023-colors-talk)
implemented by me, Autumn Ashton and Harry Wentland. It doesn't use this KMS
Color API, and therefore `COLOR_PIPELINE` is set to `Bypass`. Once the API is
accepted upstream, all users of the driver-specific API (including Gamescope)
should switch to the KMS generic API, as this will be the official plane color
management interface of the Linux kernel.

### KMS Color API on Intel

On the Intel side, the driver implementation available upstream was built upon
an earlier iteration of the API. This meant I had to apply a few tweaks to
bring it in line with the latest specifications. You can explore their latest
work
[here](https://lore.kernel.org/dri-devel/20250312072425.3099205-1-uma.shankar@intel.com/).
For a more simplified handling, combining the V9 of the Linux Color API,
Intel's contributions, and my necessary adjustments, check out
[my dedicated branch](https://gitlab.freedesktop.org/mwen/drm-misc/-/tree/intel_kms_color).

I then compiled a kernel from this integrated branch and deployed it on a
system featuring Intel TigerLake GT2 graphics. Running
[my custom drm\_info](https://gitlab.freedesktop.org/mwen/drm_info/-/tree/kms_color)
revealed a Plane Color Pipeline with three color operations as follows:

```
├───"COLOR_PIPELINE" (atomic): enum {Bypass, Color Pipeline 480} = Bypass
│   ├───Bypass
│   └───Color Pipeline 480
│       ├───Color Operation 480
│       │   ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, 1D LUT Mult Seg, 3x3 Matrix, Multiplier, 3D LUT} = 1D LUT Mult Seg
│       │   ├───"BYPASS" (atomic): range [0, 1] = 1
│       │   ├───"HW_CAPS" (atomic, immutable): blob = 484
│       │   └───"DATA" (atomic): blob = 0
│       ├───Color Operation 487
│       │   ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, 1D LUT Mult Seg, 3x3 Matrix, Multiplier, 3D LUT} = 3x3 Matrix
│       │   ├───"BYPASS" (atomic): range [0, 1] = 1
│       │   └───"DATA" (atomic): blob = 0
│       └───Color Operation 492
│           ├───"TYPE" (immutable): enum {1D Curve, 1D LUT, 3x4 Matrix, 1D LUT Mult Seg, 3x3 Matrix, Multiplier, 3D LUT} = 1D LUT Mult Seg
│           ├───"BYPASS" (atomic): range [0, 1] = 1
│           ├───"HW_CAPS" (atomic, immutable): blob = 496
│           └───"DATA" (atomic): blob = 0

```

Observe that Intel's approach introduces additional properties like "HW\_CAPS"
at the color operation level, along with two new color operation types: 1D LUT
with Multiple Segments and 3x3 Matrix. It's important to remember that this
implementation is based on an earlier stage of the KMS Color API and is
awaiting review.

### A Shout-Out to Those Who Made This Happen

I'm impressed by the solid implementation and clear direction of the V9 of the
KMS Color API. It aligns with the many insightful discussions we've had over
the past years. A huge thank you to Harry Wentland and Alex Hung for their
dedication in bringing this to fruition!

Beyond their efforts, I deeply appreciate Uma and Chaitanya's commitment to
updating Intel's driver implementation to align with the freshest version of
the KMS Color API. The collaborative spirit of the AMD and Intel developers in
sharing their color pipeline work upstream is invaluable. We're now gaining a
much clearer picture of the color capabilities embedded in modern display
hardware, all thanks to their hard work, comprehensive documentation, and
engaging discussions.

Finally, thanks all the userspace developers, color science experts, and kernel
developers from various vendors who actively participate in the upstream
discussions, meetings, workshops, each iteration of this API and the crucial
code review process. I'm happy to be part of the final stages of this long
kernel journey, but I know that when it comes to colors, one step is completed
for new challenges to be unlocked.

Looking forward to meeting you in this year Linux Display Next hackfest,
organized by AMD in Toronto, to further discuss HDR, advanced color management,
and other display trends.
