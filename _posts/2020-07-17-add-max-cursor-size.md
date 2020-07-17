---
layout: post
title: "Increasing test coverage in VKMS - max square cursor size"
date: 2020-07-17 08:00:00 -0300
categories: randomness
--- 

To develop my GSoC project proposal, I inspected the coverage of kms_cursor_crc on VKMS.
Using piglit, the following table shows the evolution of this coverage:

| Result    | GSoC start| Only accepted patches | Fixes under development | 
| ----------|:-------:|-------:|-------:|
|       pass|        7|22|24|
|       warn|        1|0|1|
|       fail|        2|3|0|
|       skip|      236|221|221|
|      crash|        0|0|0|
|    timeout|        0|0|0|
| incomplete|        0|0|0|
| dmesg-warn|        0|0|0|
| dmesg-fail|        0|0|0|
|    changes|        0|0|0|
|      fixes|        0|0|0|
|regressions|        0|0|0|
|      total|      246|246|246|

**+ Instability in the sequential run of subtests**; ie, although the statistic
showing that 7 tests are passing in the beggining, this result was only clear after a
double-check because when running all subtests sequentially, 8 tests fails and
only 1 succeed;

**+ Warning or Failure ?** [My fix proposal for the pipe-A-cursor-alpha-transparent test
was passing with a warning (CRC all zero)][1]

As a first step, I decided to examine and solve issues that affected the test
results in a general way. the instability. Solving the instability first (or at
least identify what was going on) would make the work more consistent and
fluid, since I would no longer need to double-check each subtest result and, in
one running of the entire kms_cursor_crc test I could check the absent features
or errors.  However, in this investigation, some problems were more linked to
IGT and others to VKMS. Identifying who is the "guilty" was not simple, and
some false charges happened. 

This is a little long story and **deserves another post focused on the issue**.
The effective solution has not yet been found, but it has already been realized
that the proper bug-fix achieves:
* The stability of the sequential execution of subtests;
* The success of the following subtests: pipe-A-cursor-dpms pipe-A-cursor-suspend

As we don't have that yet, well, I'll focus on this post in describing the way
to allow testing of different cursor sizes in vkms.

## The maximum cursor size

In 2018, one of the Haneen contributions to vkms was [adding support for the
cursor plane][2]. In this implementation, the cursor has a standard maximum
supported size 64x64, which limited the coverage of the kms\_cursor\_crc to
only cursor subtests with this size.

The IGT tests cursor sizes from 64 to 512 (powers of 2), but how to enable
cursor larger than the standard?

Initially, I thought I needed to develop some functionality from scratch, maybe
do this by drawing in larger sizes... as a good beginner :) So I started to
read some materials and check the codes of other drivers to find out how to do
this.

During this stage, for some kind of universe coincidence (mystic), I came
across a conversation on the IRC on the topic "cursor sizes." This
conversation gave me some references that led me to the right work path.

### Ctags-driven investigation

I tend to investigate Linux trough keywords (like a ctags on the head?). This
approach helps me to narrow the scope of reading and attempts. The Linux kernel
is frighteningly large, and I know that I still need many years to understand
things as a whole (if possible).

Therefore, these are the references that helped me to find the keywords and
information related to my problem "the cursor size":

1. Comparing an AMD device with VKMS device: https://drmdb.emersion.fr/devices
(keyword 1: DRM_CAP_CURSOR_HEIGHT)

2. Checking max cursor sizes per driver:
https://drmdb.emersion.fr/capabilities (keyword 2: capabilities)

3. Finding where is the DRM_CAP_CURSOR_HEIGHT value:
`drivers/gpu/drm/drm_ioctl.c`
(How to define `dev->mode_config.cursor_height` in vkms?)

```
/*
 * Get device/driver capabilities
 */
static int drm_getcap(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	struct drm_get_cap *req = data;
	struct drm_crtc *crtc;

	req->value = 0;
[..]
	case DRM_CAP_CURSOR_WIDTH:
		if (dev->mode_config.cursor_width)
			req->value = dev->mode_config.cursor_width;
		else
			req->value = 64;
		break;
	case DRM_CAP_CURSOR_HEIGHT:
		if (dev->mode_config.cursor_height)
			req->value = dev->mode_config.cursor_height;
		else
			req->value = 64;
		break;
[..]
}
```

4. More information: `include/uapi/drm/drm.h`
```
/ *
 * The CURSOR_WIDTH and CURSOR_HEIGHT capabilities return a valid widthxheight
 * combination for the hardware cursor. The intention is that a hardware
 * agnostic userspace can query a cursor plane size to use.
 *
 * Note that the cross-driver contract is to merely return a valid size;
 * drivers are free to attach another meaning on top, eg. i915 returns the
 * maximum plane size.
 * /
#define DRM_CAP_CURSOR_WIDTH 0x8
#define DRM_CAP_CURSOR_HEIGHT 0x9
```

5. Check the documentation for cursor_width:  
``` 
**struct drm_mode_config**  
Mode configuration control structure  
  
*cursor_width*: hint to userspace for max cursor width  
*cursor_height*: hint to userspace for max cursor height  
```

6. So, where is this `mode_config` defined in vkms?  
Here: `drivers/gpu/drm/vkms/vkms_drv.c`
``
static int vkms_modeset_init(struct vkms_device *vkmsdev)
{
	struct drm_device *dev = &vkmsdev->drm;

	drm_mode_config_init(dev);
	dev->mode_config.funcs = &vkms_mode_funcs;
	dev->mode_config.min_width = XRES_MIN;
	dev->mode_config.min_height = YRES_MIN;
	dev->mode_config.max_width = XRES_MAX;
	dev->mode_config.max_height = YRES_MAX;
	dev->mode_config.preferred_depth = 24;
	dev->mode_config.helper_private = &vkms_mode_config_helpers;

	return vkms_output_init(vkmsdev, 0);
}
``
There is nothing about cursor here, so we need to assign maximum values to not
take the default.

7. I also found that, on my intel computer, the maximum cursor is 256. Why do
tests include 512?

8. Also, there are subtests in kms_cursor_crc for non-square cursors, but these
tests are restricted to i915 devices. Why are they here?

9. Finally, I develop a [simple patch][3] that increases the coverage rate by
15 subtests. Considering the current drm-misc-next, my project state is:

| Name      | Results | 
| ----------|:-------:|
|   **pass**|   **22**|
|       fail|        3|
|       skip|      221|
|      crash|        0|
|    timeout|        0|
|       warn|        0|
| incomplete|        0|
| dmesg-warn|        0|
| dmesg-fail|        0|
|    changes|        0|
|      fixes|        0|
|regressions|        0|
|      total|      246|

#### Cheer up!

It is also possible to consider those in which we have some idea of the problem
and a provisory solution exists (I mean, an optimistic view):

| Name      | Results | 
| ----------|:-------:|
|       pass|       24|
|       warn|        1|
|   **fail**|    **0**|
|       skip|      221|
|      crash|        0|
|    timeout|        0|
| incomplete|        0|
| dmesg-warn|        0|
| dmesg-fail|        0|
|    changes|        0|
|      fixes|        0|
|regressions|        0|
|      total|      246|

There is still the non-square cursors issue, which I'm not sure we should
handle. Subtests with non-square cursors mean 16 skips.

#### Lessons learned

1. For me, IRC conversations are inspiring and also show community pulsing.
Part of what I question in my master's research is the lack of interest in
academic studies in using IRC as a means of understanding the community
under investigation.  
One of the things I like about IRC is that, unlike other instant messengers
today, when we are there, we are usually sitting in front of the computer
(our work tool). I mean, we are not (I think) lying in a hammock on the
beach, for example. Ok, there could be exceptions. :)  
But to be honest, I hardly talk on a channel; I am usually just watching.

2. There are cases where the complexity lies in understanding what already
exists instead of in developing new features. I still don't know if it's
frustrating or satisfying.

3. I don't know if I could understand things without using ctags. The ctags
was a great tip from Siqueira even in FLUSP times.

[1]: https://patchwork.freedesktop.org/series/74939/
[2]: https://patchwork.kernel.org/patch/10590131/
[3]: https://patchwork.freedesktop.org/series/79358/
