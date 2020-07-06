---
layout: post
title: "GSoC First Phase - Achievements"
date: 2020-07-06 10:00:00 -0300
categories: randomness
--- 

The first round just passed so fast, and what I did?

### The case: the IGT test kms\_cursor\_crc

#### Being acquainted with a IGT test

1. **Anatomy of the test**  
*What:* study, read the code, dive into each function, and describe its
structure, steps, and functionality.  
*To:* be well aware of the code construction and, consequently, able to
figure out and deal with problems.  
2. **Patches to improve test documentation**  
*What:* During the study of the kms\_cursor\_crc test, I realized that the
subtests had no description that would help a newcomer to perceive the purpose
of that subtest. To improve this documentation and some code comments, I sent
some patches to IGT with what I was able to understand from my inspection.  
  * [test/kms\_cursor\_crc: update subtests descriptions and some comments](https://patchwork.freedesktop.org/patch/372944/?series=78814&rev=1)
  * [lib/igt\_fb: change comments with fd description](https://patchwork.freedesktop.org/patch/372943/?series=78814&rev=1)
3. **Refactoring function with parameters never used.**  
Outside the context of exploring and becoming acquainted with the case,
examining the anatomy of the kms\_cursor\_crc I caught useless parameters in a
general IGT function, i.e., it requires two parameters, but never uses them
within its code. I checked the author (git blame) and asked him on IRC about
the need for these additional parameters, but I didn't get a response (or maybe
I missed the reply due to disconnection). Then, I sent an RFC patch to the
mailing list and also nothing. Finally, my mentor took a look, and he agreed
that the parameters seem useless and can be removed. He asked me to resend as
a normal patch.  

Maybe you can also take a look:
[lib/igt\_fb: remove extra parameters from igt\_put\_caito\_ctx](https://patchwork.freedesktop.org/patch/372945/?series=78815&rev=1)

### Solving some problems

I have also sent a [patchset to treat to reported problems in kms\_cursor\_crc](https://patchwork.freedesktop.org/series/78813/):
1. Access to debugfs data file is blocked
2. Unstable behaviour in sequential subtests

You can check out more about that in my [previous posts](https://melissawen.github.io/randomness/2020/06/15/status-update.html).
Reviewing the logs, I saw that these problems in prepare\_crtc were introduced
more recently since some Haneen commits report that subtests were working
before. I noticed that waiting for the blank is a solution used before and
removed by another commit that not seems to treat this issue.

Sadly, my patches also received no feedback at all. So, if you are
interested in them or can make an interesting comment, go ahead! :)

### Preparing for the second round

In the last few days, I surveyed the materials and tasks needed to execute the
second round of my project. This is a kind of roadmap with useful links.

As far as I know, I have to deal with three issues that seem currently not
supported by VKMS:

1. Enhance the blend function to provide alpha composing  
To solve warning message in pipe-%s-cursor-alpha-transparent
* SIMD <http://ermig1979.github.io/Simd/help/group__drawing.html>
2. Provide DPMS  
To solve failure in pipe-%s-cursor-dpms and pipe-%s-cursor-suspend
* <https://en.wikipedia.org/wiki/VESA_Display_Power_Management_Signaling>
* <https://www.kernel.org/doc/html/latest/gpu/drm-kms.html#c.drm_connector_funcs>
* <https://www.kernel.org/doc/html/latest/gpu/drm-kms.html#standard-connector-properties>
* <https://www.kernel.org/doc/html/latest/gpu/drm-kms-helpers.html#c.drm_helper_connector_dpms>
* <https://www.kernel.org/doc/html/latest/gpu/drm-client.html#c.drm_client_modeset_dpms>
3. Cursor size other than 64x64
* <https://drmdb.emersion.fr/devices/3a5c55e30723>
* <https://drmdb.emersion.fr/capabilities>
* See drivers/gpu/drm/drm\_ioctl.c
* See drivers/gpu/drm/vkms/vkms\_drv.c (drm\_getCap)
```
drivers/gpu/drm/vkms/vkms_drv.c:131:    dev->mode_config.funcs = &vkms_mode_funcs;
drivers/gpu/drm/vkms/vkms_drv.c:132:    dev->mode_config.min_width = XRES_MIN;
drivers/gpu/drm/vkms/vkms_drv.c:133:    dev->mode_config.min_height = YRES_MIN;
drivers/gpu/drm/vkms/vkms_drv.c:134:    dev->mode_config.max_width = XRES_MAX;
drivers/gpu/drm/vkms/vkms_drv.c:135:    dev->mode_config.max_height = YRES_MAX;
drivers/gpu/drm/vkms/vkms_drv.c:136:    dev->mode_config.preferred_depth = 24;
drivers/gpu/drm/vkms/vkms_drv.c:137:    dev->mode_config.helper_private = &vkms_mode_config_helpers;
```
