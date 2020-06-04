---
layout: post
title: "Walking in the KMS\_CURSOR\_CRC test"
date: 2020-06-03 15:00:00 -0300
categories: randomness
--- 

In this post, I describe the steps involved in the execution of a
kms\_cursor\_crc subtest. In my approach, I chose a subtest
(pipe-A-cursor-alpha-transparent) as a target and examined the code from the
beginning of the test (igt main) until reaching the target subtest and
executing it.

This is my zero version. I plan to incrementally expand this document with
evaluation/description of the other subtests. I will probably also need to
fix some misunderstandings.

As described by IGT, kms\_cursor\_crc

> Uses the display CRC support to validate cursor plane functionality.  The
> test will position the cursor plane either fully onscreen, partially
> onscreen, or fully offscreen, using either a fully opaque or fully
> transparent surface. In each case it then reads the PF CRC and compares it
> with the CRC value obtained when the cursor plane was disabled.

In the past, Haneen have shown something about the test in a [blog
post](http://haneensa.github.io/2018/07/29/drmdebug/).  Fixing any issue in
VKMS to make it passes all kms\_cursor\_crc subtest is also a case in my [GSoC
project
proposal](https://summerofcode.withgoogle.com/projects/#4669149428580352).

#### The struct data\_t

This struct is used in all subtest stores many elements such as DRM file
descriptor; framebuffers info; cursor size info; etc. Also, before the main of
the test, a _static data\_t data_ is declared global.

## The beginning - igt\_main

We can divide the main function into two parts: setup DRM stuff (igt\_fixture)
and subtest execution.

### Initial test setup

```
igt_fixture { data.drm_fd = drm_open_driver_master(DRIVER_ANY); ret =
drmGetCap(data.drm_fd, DRM_CAP_CURSOR_WIDTH, &cursor_width);
	
	igt_assert(ret == 0 || errno == EINVAL); /* Not making use of
cursor_height since it is same as width, still reading */ ret =
drmGetCap(data.drm_fd, DRM_CAP_CURSOR_HEIGHT, &cursor_height); igt_assert(ret
== 0 || errno == EINVAL);
		
```

**drmGetCap(int fd, uint64\_t capability, uint64\_t \* value)** queries
capability of the DRM driver, return 0 if the capability is supported.
DRM\_CAP\_CURSOR\_WIDTH and DRM\_CAP\_CURSOR\_HEIGHT store a valid width/height
for the hardware cursor.

```
/* We assume width and height are same so max is assigned width */
igt_assert_eq(cursor_width, cursor_height);

	kmstest_set_vt_graphics_mode();
```

**void kmstest\_set\_vt\_graphics_mode(void)**

_From lib/igt\_kms.c_: This
function sets the controlling virtual terminal (VT) into graphics/raw mode and
installs an igt exit handler to set the VT back to text mode on exit. All kms
tests must call this function to make sure that the [framebuffer
console](https://www.kernel.org/doc/Documentation/fb/fbcon.txt) doesn't
interfere by e.g. blanking the screen. 

``` igt_require_pipe_crc(data.drm_fd);
```

**void igt\_require\_pipe\_crc(int fd)** _From lib/igt\_debugfs_: checks
whether pipe CRC capturing is supported by the kernel. Uses _igt_skip_ to
automatically skip the test/subtest if this isn't the case.

```
    igt_display_require(&data.display, data.drm_fd);
```

**void igt\_display\_require(igt\_display\_t *display, int drm\_fd)**

_From
lib/igt\_kms.c_: Initializes @display (a pointer to an #igt\_display\_t
structure) and allocates the various resources required. This function
automatically skips if the kernel driver doesn't support any CRTC or outputs.

### Run test on pipe

```
for_each_pipe_static(pipe)
	igt_subtest_group
		run_tests_on_pipe(&data, pipe);
```

At this part, each subtest of kms\_cursor\_crc is runned, where the pointer for
the already setup struct data_t are passed.

**static void run_tests_on_pipe(data_t \*data, enum pipe pipe)**

This function runs each subtest grouped by pipe. In the setup, it increments
the passed data_t struct, and then starts to call each subtest.  In this
document version, I only focused on the subtest _test\_cursor\_transparent_ . 

```
igt_subtest_f("pipe-%s-cursor-alpha-transparent", kmstest_pipe_name(pipe))
	run_test(data, test_cursor_transparent, data->cursor_max_w, data->cursor_max_h);
```

### The execution of test\_cursor\_transparent

**static void run\_test(data\_t \*data, void (\*testfunc)(data\_t \*), int cursor\_w, int cursor\_h)**
The function run\_test wrap the common preparation
for running a subtest and also, after then, a cleanup. Therefore, it basically
has three steps:

1. Prepare CRTC: **static void prepare_crtc(data\_t *data, igt\_output\_t
*output, int cursor\_w, int cursor\_h)** This function is responsible for:
  * Select the pipe to be used
  * Create Front and Restore framebuffer of primary plane
  * Find a valid plane type for primary plane and cursor plane
  * Pairs primary framebuffer to its plane and sets a default size
  * Create a new pipe CRC
  * Position the cursor fully visible
  * Store test image as cairo surface
  * Start CRC capture process
2. Run subtest: testfunc(data) >> **static void test\_cursor\_transparent(data\_t \*data)** >> test\_cursor\_alpha(data, 0.0)
The subtest\_cursor\_transparent is a variation of test\_cursor\_alpha where
the alpha channel is set zero (or transparent).  So, let's take a look at
test\_cursor\_alpha execution:

```
static void test_cursor_alpha(data_t *data, double a)
{
	igt_display_t *display = &data->display;
	igt_pipe_crc_t *pipe_crc = data->pipe_crc;
	igt_crc_t crc, ref_crc;
	cairo_t *cr;
	uint32_t fb_id;
	int curw = data->curw;
	int curh = data->curh;

	/*alpha cursor fb*/
	fb_id = igt_create_fb(data->drm_fd, curw, curh,
				    DRM_FORMAT_ARGB8888,
				    LOCAL_DRM_FORMAT_MOD_NONE,
				    &data->fb);
```

When this subtest starts, it creates the cursor’s framebuffer with the format
ARGB8888 , i.e., a framebuffer with RGB plus Alpha channel (pay attention to
**endianness**) 

```
	igt_assert(fb_id);
	cr = igt_get_cairo_ctx(data->drm_fd, &data->fb);
	igt_paint_color_alpha(cr, 0, 0, curw, curh, 1.0, 1.0, 1.0, a);
	igt_put_cairo_ctx(data->drm_fd, &data->fb, cr);
```

Then, the test uses some [Cairo](https://www.cairographics.org/manual/)
resources to create a cairo surface for the cursor’s framebuffer and allocate a
drawing context for it, draw a rectangle with RGB white and the given opacity
(alpha channel) and, finally, release the cairo surface and write the changes
out to the framebuffer (_Disclaimed: looking inside the function
igt\_put\_cairo\_ctx, I am not sure if it is doing what it is saying on
comments, and also not sure if all the parameters are necessary_)

The test is divided into two parts: Hardware test and Software test. 

```
	/*Hardware Test*/
	cursor_enable(data);
	igt_display_commit(display);
	igt_wait_for_vblank(data->drm_fd, data->pipe);
	igt_pipe_crc_get_current(data->drm_fd, pipe_crc, &crc);
	cursor_disable(data);
	igt_remove_fb(data->drm_fd, &data->fb);
```

The hardware test consists in:
  * Enable cursor: Pair the cursor plane and a framebuffer, set the cursor size
    for its plane, set the cursor plane size for framebuffer. Commit
framebuffer changes to all changes of each display pipe
  * Wait for vblank. Vblank is a couple of extra scanlines region which aren't
    actually displayed on the screen designed to give the electron gun (on
CRTs) enough time to move back to the top of the screen to start scanning out
the next frame. A vblank interrupt is used to notify the driver when it can
start the updating of registers. To achieve tear-free display, users must
synchronize page flips and/or rendering to vertical blanking
[https://dri.freedesktop.org/docs/drm/gpu/drm-kms.html#vertical-blanking]
  * Calculate and check the current CRC

``` 
	cr = igt_get_cairo_ctx(data->drm_fd, &data->primary_fb[FRONTBUFFER]);
	igt_paint_color_alpha(cr, 0, 0, curw, curh, 1.0, 1.0, 1.0, a);
	igt_put_cairo_ctx(data->drm_fd, &data->primary_fb[FRONTBUFFER], cr);

	igt_display_commit(display);
	igt_wait_for_vblank(data->drm_fd, data->pipe);
	igt_pipe_crc_get_current(data->drm_fd, pipe_crc, &ref_crc);
	igt_assert_crc_equal(&crc, &ref_crc);
```
The software test consists in:
  * Create a cairo surface and drawing context, draw a rectangle on top left
    side with RGB white and the given opacity (alpha channel) and then release
de cairo context, applying changes to framebuffer. Then, commit framebuffer
changes to all changes of each display pipe.
  * Wait for vblank
  * Calculate and check the current CRC
  
Finally, the screen is clean.

And in the level of run_test, crtc(data) is clean up;
