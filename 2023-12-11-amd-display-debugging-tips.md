---
layout: page
title: "15 Tips for Debugging Issues in the AMD Display Kernel Driver"
date: 2023-12-11 11:25:00 -0100
draft: draft
permalink: /draft-amd-display-debugging-tips
---

This blog post provides insights and guidance for examining and debugging the
AMD display driver within the Linux kernel/DRM subsystem. These tips are based
on my experience as an external developer working on the driver, and are shared
with the goal of helping others navigate the process.

**Worth mentioning:** This blog post builds upon my talk,
["I'm not an AMD expert, but..."](https://www.youtube.com/watch?v=CMm-yhsMB7U)
presented at the 2022 XDC. It shares guidelines that helped me debug AMD
display issues as an external developer of the driver.

**Acknowledgments:** These tips were gathered thanks to the countless help
received from AMD developers during the driver development process. This list
was obtained by examining open source code, reviewing public documentation,
playing with tools, asking in public forums and also with the help of my former
GSoC mentor, [Rodrigo Siqueira](https://siqueira.tech/).

**Open Source Display Driver:** The Linux kernel/AMD display driver is open
source, allowing you to actively contribute by addressing issues listed in the
[official tracker](https://gitlab.freedesktop.org/drm/amd). Tackling existing
issues or resolving your own can be a rewarding learning experience and a
valuable contribution to the community. Additionally, the tracker serves as a
valuable resource for finding similar bugs, troubleshooting tips, and
suggestions from AMD developers. Finally, it's a platform for seeking help when
needed.

## Pre-Debugging Steps:

Before diving into debugging, it's crucial to perform two essential steps:

1. Check the latest changes: Ensure you're working with the latest AMD driver
   modifications located in the `amd-staging-drm-next` branch maintained by
   Alex Deucher. You may also find bug fixes for newer kernel versions
   on branches that have the name pattern `drm-fixes-<date>`.

2. Examine the issue tracker: Before diving into debugging, confirm that your
   issue isn't already documented and addressed in the AMD display driver issue
   tracker.

## Understanding the issue:

3. Is it an AMD kernel driver issue?: Identifying the source of the issue is
   essential regardless of the GPU vendor.
   Sometimes this can be challenging so here are some helpful tips:
   - Record the screen: Capture the screen using a recording app while
     experiencing the issue. If the bug appears in the capture, it's likely a
     userspace issue, not the kernel display driver.
   - Analyze the dmesg log: Look for error messages related to the display driver
     in the dmesg log. If the error message appears before
     `[drm] amdgpu kernel modesetting enabled`, it's not likely a display driver issue.

4. AMD Display Manager vs. AMD Display Core: The AMD display driver consists of
   two components:
   - Display Manager (DM): This component interacts directly with the Linux DRM
     infrastructure. Occasionally, issues can arise from misinterpretations of
     DRM properties or features. If the issue doesn't occur on other platforms
     with AMD hardware, i.e. only happens on Linux, it's more likely related
     to the DM.
   - Display Core (DC): This is the platform-agnostic part responsible for
     setting and programming hardware features. Modifications to the DC usually
     require validation on other platforms to avoid regressions.

5. Identify the DC HW family: Each AMD GPU has variations in its hardware
   architecture. Features and helpers differ between families, so determining
   the relevant code for your specific hardware is crucial.
   - Find GPU product information [in Linux/AMD GPU documenation](https://dri.freedesktop.org/docs/drm/gpu/amdgpu/driver-misc.html#gpu-product-information)
   - Check the dmesg log for the Display Core version (since [this commit](https://github.com/torvalds/linux/commit/bf7fda0b3736f93ac8b18e7147e1e7acd27e6a19)
     in Linux kernel 6.3v). For example:
     - `[drm] Display Core v3.2.241 initialized on DCN 2.1`
     - `[drm] Display Core v3.2.237 initialized on DCN 3.0.1`

## Investigating the relevant driver code:

6. Check code of a specific DC HW family: the relevant code resides in a
   directory named after the DC number. For example, the DCN 3.0.1 driver code
   is located at `drivers/gpu/drm/amd/display/dc/dcn301`.

7. Newer families may inherit code from older ones: you can find dcn301 using
   code from dcn30, dcn20, dcn10 files. It's crucial to verify which hooks and
   helpers your driver utilizes to investigate the right portion. You can leverage
   `ftrace` for supplemental validation. It was useful when updating DCN3 color
   mapping to correctly use their new post-blending color capabilities, such as:
   - https://lore.kernel.org/dri-devel/20230721132431.692158-1-mwen@igalia.com/

Additionally, you can use two different HW families to compare behaviours. If
you see the issue in one but not in the other, you can compare the code and
understand what has changed and if the implementation from a previous family
doesn't fit well the new HW resources or design. This helped me debug
[a 2-year-old issue](https://gitlab.freedesktop.org/drm/amd/-/issues/1513#note_2003082)
where the cursor gamma adjustment was incorrect in DCN3 hardware, but working
correctly for DCN2 family:
   - https://lore.kernel.org/amd-gfx/20230731083505.1500965-1-mwen@igalia.com/
   - https://lore.kernel.org/amd-gfx/20230824133810.10627-1-mwen@igalia.com/

8. Check the hardware capability screening in the driver: You can currently find a
   list of display hardware capabilities in the
   `drivers/gpu/drm/amd/display/dc/dcn*/dcn*_resource.c` file. More precisely in
   the `dcn*_resource_construct()` function.
   Using DCN301 for illustration, here is the list of its hardware caps:

```
	/*************************************************
	 *  Resource + asic cap harcoding                *
	 *************************************************/
	pool->base.underlay_pipe_index = NO_UNDERLAY_PIPE;
	pool->base.pipe_count = pool->base.res_cap->num_timing_generator;
	pool->base.mpcc_count = pool->base.res_cap->num_timing_generator;
	dc->caps.max_downscale_ratio = 600;
	dc->caps.i2c_speed_in_khz = 100;
	dc->caps.i2c_speed_in_khz_hdcp = 5; /*1.4 w/a enabled by default*/
	dc->caps.max_cursor_size = 256;
	dc->caps.min_horizontal_blanking_period = 80;
	dc->caps.dmdata_alloc_size = 2048;
	dc->caps.max_slave_planes = 2;
	dc->caps.max_slave_yuv_planes = 2;
	dc->caps.max_slave_rgb_planes = 2;
	dc->caps.is_apu = true;
	dc->caps.post_blend_color_processing = true;
	dc->caps.force_dp_tps4_for_cp2520 = true;
	dc->caps.extended_aux_timeout_support = true;
	dc->caps.dmcub_support = true;

	/* Color pipeline capabilities */
	dc->caps.color.dpp.dcn_arch = 1;
	dc->caps.color.dpp.input_lut_shared = 0;
	dc->caps.color.dpp.icsc = 1;
	dc->caps.color.dpp.dgam_ram = 0; // must use gamma_corr
	dc->caps.color.dpp.dgam_rom_caps.srgb = 1;
	dc->caps.color.dpp.dgam_rom_caps.bt2020 = 1;
	dc->caps.color.dpp.dgam_rom_caps.gamma2_2 = 1;
	dc->caps.color.dpp.dgam_rom_caps.pq = 1;
	dc->caps.color.dpp.dgam_rom_caps.hlg = 1;
	dc->caps.color.dpp.post_csc = 1;
	dc->caps.color.dpp.gamma_corr = 1;
	dc->caps.color.dpp.dgam_rom_for_yuv = 0;

	dc->caps.color.dpp.hw_3d_lut = 1;
	dc->caps.color.dpp.ogam_ram = 1;
	// no OGAM ROM on DCN301
	dc->caps.color.dpp.ogam_rom_caps.srgb = 0;
	dc->caps.color.dpp.ogam_rom_caps.bt2020 = 0;
	dc->caps.color.dpp.ogam_rom_caps.gamma2_2 = 0;
	dc->caps.color.dpp.ogam_rom_caps.pq = 0;
	dc->caps.color.dpp.ogam_rom_caps.hlg = 0;
	dc->caps.color.dpp.ocsc = 0;

	dc->caps.color.mpc.gamut_remap = 1;
	dc->caps.color.mpc.num_3dluts = pool->base.res_cap->num_mpc_3dlut; //2
	dc->caps.color.mpc.ogam_ram = 1;
	dc->caps.color.mpc.ogam_rom_caps.srgb = 0;
	dc->caps.color.mpc.ogam_rom_caps.bt2020 = 0;
	dc->caps.color.mpc.ogam_rom_caps.gamma2_2 = 0;
	dc->caps.color.mpc.ogam_rom_caps.pq = 0;
	dc->caps.color.mpc.ogam_rom_caps.hlg = 0;
	dc->caps.color.mpc.ocsc = 1;

	dc->caps.dp_hdmi21_pcon_support = true;

	/* read VBIOS LTTPR caps */
	if (ctx->dc_bios->funcs->get_lttpr_caps) {
		enum bp_result bp_query_result;
		uint8_t is_vbios_lttpr_enable = 0;

		bp_query_result = ctx->dc_bios->funcs->get_lttpr_caps(ctx->dc_bios, &is_vbios_lttpr_enable);
		dc->caps.vbios_lttpr_enable = (bp_query_result == BP_RESULT_OK) && !!is_vbios_lttpr_enable;
	}

	if (ctx->dc_bios->funcs->get_lttpr_interop) {
		enum bp_result bp_query_result;
		uint8_t is_vbios_interop_enabled = 0;

		bp_query_result = ctx->dc_bios->funcs->get_lttpr_interop(ctx->dc_bios, &is_vbios_interop_enabled);
		dc->caps.vbios_lttpr_aware = (bp_query_result == BP_RESULT_OK) && !!is_vbios_interop_enabled;
	}
```

Keep in mind that the documentation of color capabilities are available at:
https://dri.freedesktop.org/docs/drm/gpu/amdgpu/display/display-manager.html#dc-color-capabilities-between-dcn-generations

## Understanding the development history:

9. Pinpoint relevant commits: Use git log to identify commits targeting the
   code section you're interested in.

10. Track regressions: If you're examining the `amd-staging-drm-next` branch,
   check for regressions between DC release versions. These are defined by
   `DC_VER` in the `drivers/gpu/drm/amd/display/dc/dc.h` file.  You can also
   find a commit with this format `drm/amd/display: 3.2.221` that determines a
   display release. It's useful for bisecting. This information helps you
   understand how outdated your branch is and identify potential regressions.
   You can consider each `DC_VER` takes around one week to be bumped.

## Reducing the inspection area

11. Identify involved HW blocks: This helps isolate the issue. You can find
    more information about DCN HW blocks in the
    [DCN Overview documentation](https://dri.freedesktop.org/docs/drm/gpu/amdgpu/display/dcn-overview.html).
    In summary:
    - Plane issues are closer to HUBP and DPP.
    - Blending/Stream issues are closer to MPC, OPP and OPTC. They are related to
      DRM CRTC subjects.

This information was useful when debugging a hardware rotation issue where
[the cursor plane got clipped off in the middle of the screen](https://gitlab.freedesktop.org/drm/amd/-/issues/2247#note_1747639).
Finally, here are the two steps to solve the issue:
    - https://lore.kernel.org/amd-gfx/20221118125935.4013669-22-Brian.Chang@amd.com/
    - https://lore.kernel.org/amd-gfx/20230131160546.150611-1-mwen@igalia.com/

12. Issues around bandwidth (glitches) and clocks: May be affected by
    calculations done in these HW blocks and HW specific values. The
    recalculation equations are found in the DML folder. Finding some clk variables
    that affect device behavior may be a sign of it. It's hard for a external
    developer to debug this part, since it involves information from HW specs and
    firmware programming that we don't have access. The best option is to provide
    all relevant debugging information you have and ask AMD developers to check the
    values from your suspicions.
    - Tentative: if you suspect the issue is here. Try to change the power
      performance and see if it affects the system behavior:
    `sudo bash -c "echo high > /sys/class/drm/card0/device/power_dpm_force_performance_level"`

I learned it when debugging [glitches with hardware cursor rotation on Steam Deck](https://gitlab.freedesktop.org/drm/amd/-/issues/2247#note_1748842).
My first attempt was [changing  the clock calculation](https://lore.kernel.org/amd-gfx/20230207233235.513948-1-mwen@igalia.com).
In the end, Rodrigo Siqueira proposed the right solution targeting bandwidth in two steps:
    - [Patch series to create a new internal commit sequence](https://patchwork.freedesktop.org/series/114632/)
    - [Enabling pipe split on DCN301](https://patchwork.freedesktop.org/patch/526108/?series=114927&rev=1)

## Checking implicit programming and hardware limitations:

13. Implicit update type: It's one example. Check if the selected type for
    atomic update may affect your issue. The update type depends on the mode
    settings, since programming some modes demands more time for hardware
    processing.
    More information [source](https://cgit.freedesktop.org/drm/drm-misc/tree/drivers/gpu/drm/amd/display/dc/dc.h):

```
/* Surface update type is used by dc_update_surfaces_and_stream
 * The update type is determined at the very beginning of the function based
 * on parameters passed in and decides how much programming (or updating) is
 * going to be done during the call.
 *
 * UPDATE_TYPE_FAST is used for really fast updates that do not require much
 * logical calculations or hardware register programming. This update MUST be
 * ISR safe on windows. Currently fast update will only be used to flip surface
 * address.
 *
 * UPDATE_TYPE_MED is used for slower updates which require significant hw
 * re-programming however do not affect bandwidth consumption or clock
 * requirements. At present, this is the level at which front end updates
 * that do not require us to run bw_calcs happen. These are in/out transfer func
 * updates, viewport offset changes, recout size changes and pixel
depth changes.
 * This update can be done at ISR, but we want to minimize how often
this happens.
 *
 * UPDATE_TYPE_FULL is slow. Really slow. This requires us to recalculate our
 * bandwidth and clocks, possibly rearrange some pipes and reprogram
anything front
 * end related. Any time viewport dimensions, recout dimensions,
scaling ratios or
 * gamma need to be adjusted or pipe needs to be turned on (or
disconnected) we do
 * a full update. This cannot be done at ISR level and should be a rare event.
 * Unless someone is stress testing mpo enter/exit, playing with
colour or adjusting
 * underscan we don't expect to see this call at all.
 */

enum surface_update_type {
UPDATE_TYPE_FAST, /* super fast, safe to execute in isr */
UPDATE_TYPE_MED,  /* ISR safe, most of programming needed, no bw/clk change*/
UPDATE_TYPE_FULL, /* may need to shuffle resources */
};
```

## Using tools:

14. Use AMD tools to check hardware state and driver programming: help on
understanding your driver settings and checking the behavior when changing
those settings.
  - [DC Visual confirmation](https://dri.freedesktop.org/docs/drm/gpu/amdgpu/display/dc-debug.html#dc-visual-confirmation):
    Check multiple planes and pipe split policy.
  - [DTN logs](https://dri.freedesktop.org/docs/drm/gpu/amdgpu/display/dc-debug.html#dtn-debug):
    Check display hardware state, including rotation, size, format, underflow,
    blocks in use, color block values, etc.
  - [UMR](https://gitlab.freedesktop.org/tomstdenis/umr): Check ASIC info,
    register values, KMS state - links and elements (framebuffers,
planes, CRTCs, connectors).

15. Use generic DRM/KMS tools:
  - [IGT test tools](https://gitlab.freedesktop.org/drm/igt-gpu-tools): Use
    generic KMS tests or develop your own to isolate the issue in the kernel
    space. Compare results across different GPU vendors to understand their
    implementations and find potential solutions. Here AMD also has specific IGT
    tests for its GPUs that is expect to work without failures on any
AMD GPU. You
    can check results of HW-specific tests using different display hardware
    families or you can compare expected differences between the
generic workflow
    and AMD workflow.
  - [drm\_info](https://github.com/ascent12/drm_info): This tool summarizes the
    current state of a display driver (capabilities, properties and formats)
    per element of the DRM/KMS workflow. Output can be helpful when
reporting bugs.

## Don't give up:

Debugging issues in the AMD display driver can be challenging, but by following
these tips and leveraging available resources, you can significantly improve
your chances of success. Remember, contributing to the open source community
through issue resolution and collaboration is mutually beneficial for everyone
involved.