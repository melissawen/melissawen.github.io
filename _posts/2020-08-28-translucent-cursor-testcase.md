---
layout: post
title: "False-positive for alpha blending"
date: 2020-08-28 07:00:00 -0300
categories: randomness
--- 

I recently
[posted](https://melissawen.github.io/randomness/2020/08/19/let-vkms-blend-it.html)
about a [feature](https://patchwork.freedesktop.org/series/80823/) I developed
for VKMS to consider the alpha channel in the composition of the cursor plane
with the primary plane. This process took a little longer than expected, and
now I can identify a few reasons:
* Beginner: I had little knowledge of computer graphics and its operations
* Maybe cliché: I did not consider that in the subsystem itself, there was
  already material to aid coding.
* The unexpected: the test cases that checked the composition of a cursor
  considering the alpha channel were successful, even with a defect in the
implementation.

IGT GPU Tools has two test cases in
[kms\_cursor\_crc](https://drm.pages.freedesktop.org/igt-gpu-tools/igt-kms-tests.html#kms_cursor_crc)
to check the cursor blend in the primary plane: the cursor-alpha-opaque and the
cursor-alpha-transparent.  These two cases are structured in the same way,
changing only the value of the alpha channel - totally opaque 0xFF or totally
transparent 0x00. 

In a brief description, the test checks the composition as follows:
1. Creates a XRGB primary plane framebuffer with black background
2. Creates a ARGB framebuffer for cursor plane with white color and a given
alpha value (0xFF or 0x00)
3. Enables the cursor on hardware and captures the plane's CRC after
composition. (hardware)
4. Disables the cursor on hardware, draws a cursor directly on the primary
plane and capture the CRC (software)
5. Compares the two CRCs captured: they must have equal values.

After implementing alpha blending using the straight alpha formula in VKMS,
both tests were successful. However, the equation was not correct.

To paint, IGT uses the library [Cairo](https://www.cairographics.org/manual/)
and the CAIRO\_FORMAT\_ARGB32 formats for the cursor and CAIRO\_FORMAT\_RGB24
for the primary plane. According to
[documentation](https://www.cairographics.org/manual/cairo-Image-Surfaces.html#cairo-format-t),
this format stores the pixel color in the pre-multiplied alpha representation:

```
CAIRO_FORMAT_ARGB32

each pixel is a 32-bit quantity, with alpha in the upper 8 bits, then red, then
green, then blue. The 32-bit quantities are stored native-endian.
Pre-multiplied alpha is used. (That is, 50% transparent red is 0x80800000, not
0x80ff0000.) (Since 1.0)
```

In a brief dialogue with Pekka about endianness, he showed me information from
the DRM documentation that I didn't know about. According to the documentation,
DRM converges with the representation used by Cairo:

```

Current DRM assumption is that alpha is premultiplied, and old userspace can
break if the property defaults to anything else.

```

It is also possible to find information about
[DRM's Plane Composition Properties](https://www.kernel.org/doc/html/v5.1/gpu/drm-kms.html#plane-composition-properties),
such as the existence of `pixel blend mode` to `add a blend mode for alpha
blending equation selection, describing how the pixels from the current plane
are composited with the background`.

Finally, you can find there the pre-multiplied alpha blending equation:

`out.rgb = plane_alpha * fg.rgb + (1 - (plane_alpha * fg.alpha)) * bg.rgb`

From this information, we see that both IGT and DRM use the same
representation, but the current test cases of kms\_cursor\_crc do not show the
defect in using the straight-alpha formula. With this in mind, I think of
refactoring the test cases so that they could validate translucent cursors and
"remove some zeros" from the equation. After shared thoughts with my mentor,
Siqueira, I decided to combine the two testcases (cursor-alpha-opaque and
cursor-alpha-transparent) into one and refactor them so that the testcase
verifies not only extreme alpha values, but also translucent values. 

Therefore, the submitted test case proposal follows this steps:
1. Creates a XRGB primary plane framebuffer with black background
2. Create a ARGB framebuffer for cursor plane and enables the cursor on
   hardware.
3. Paints the cursor with white color and a range of alpha value (from 0xFF to
   0x00)
3. For each alpha value, captures the plane's CRC after composition in a array
   of CRC's. (hardware)
4. Disables the cursor on hardware.
5. Draws cursor directly on the primary plane following the same range of alpha
   values (software)
6. Captures the CRC and compares it with the CRC in the array of hardware
   CRC's: they must have equal values.
7. Clears primary plane and go to step 5.

The code sent: [tests/kms\_cursor\_crc: refactoring cursor-alpha subtests](https://patchwork.freedesktop.org/series/81060/)

CI checks did not show any problems, so I am expecting some human feedback :)
