---
layout: post
title: "Status update - connected errors"
date: 2020-06-15 10:00:00 -0300
categories: randomness
--- 

I spent the last week investigating the cause of two problems between VKMS and IGT that I have faced and reported in the development phase of my GSoC project proposal. One of the issues was a weird behavior, that I described as unstable, in the sequential execution of the kms\ _cursor\ _crc subtests or running the same subtest twice in a row: a subtest that passed in the first run failed in the second and returned to succeed in the third (and so on).

At first, it was difficult to determine where was the error because:
1. I had a very superficial comprehension (I still have it, but a little less) about the implementation of an IGT test and subtests.
2. The file used by the test to write and verify the data had the access blocked soon after the execution of a subtest
3. Currently, when vkms is the driver used, only two subtests of kms\_cursor\_crc are passing

A previous task that helped me a lot in this investigative process was stopping to read and study the test code to understand its structure and the steps taken during the execution. My mentor guided me to do this study, and I published an initial version of this anatomy in my previous post. With that, I was able to do some checks to evaluate what was leaving the file created by igt_debugfs blocked, which ended up also solving the problem of sequential execution.

### Waiting for Vblank

I describe below how I reached the idea of adding a call of waiting for vblank that solved the problem mentioned above. I verified in some other scenarios that adding this call does not cause regression, but I still don't have a good perception and confidence of why this call is only necessary for VKMS.

#### Timed out: Opening crc fd, and poll for first CRC

For checking this issue, you have to enable VKMS and run (for example) the subtest alpha-opaque twice:

```
sudo IGT_FORCE_DRIVER=vkms build/tests/kms_cursor_crc --run-subtest pipe-A-cursor-alpha-opaque
```

You will see the subtest succeed in the first execution and fail in the second. The debug will report a timeout on opening crc fd, and poll for first CRC.
From this report, I guessed that something previously allocated was not released, i.e., lacking a king of "free".

1. Checking the code, much of allocation and release of things on each round of subtest is in the function prepare\_crtc and cleanup\_crtc:

```
static void run_test(data_t *data, void (*testfunc)(data_t *), int cursor_w, int cursor_h)
{
	prepare_crtc(data, data->output, cursor_w, cursor_h);
	testfunc(data);
	cleanup_crtc(data);
}
```

From this, I change the code to skip the call testfunc(data) - a function to call a specific subtest, because I wanted to check if the problem was limited to prepare\_crtc/cleanup\_crtc or would be inside any subtest. I validated that the problem was inside prepare/cleanup operations, since the issue still happened even if no specific subtest was running.

2. I checked what was allocated in prepare\_crtc to 'mirror' it when cleaning. I also took a look at other crc tests to see how they do the setup and cleanup of things. I was partially successful when I did a pre-check before the creation of pipe\_crc, releasing pipe\_crc if it was not free at that moment.

```
	/* create the pipe_crc object for this pipe */
	if(data->pipe_crc)
		igt_pipe_crc_free(data->pipe_crc);
	data->pipe_crc = igt_pipe_crc_new(data->drm_fd, data->pipe,
					  INTEL_PIPE_CRC_SOURCE_AUTO);
```

With this, the sequential execution of subtests passed to alternate between success and failure (note that no subtest was running, only it was preparing, and cleaning). I have also tried to complement the cleanup function, releasing more things, but I didn't see any improvement.

3. I observed that something was generating an infinite busy wait for the debugfs file related to the fd. Then, I delimited this busy waiting to the line "poll(&pfd, 1, -1)" in lib/igt\_debugfs.c:igt\_pipe\_crc\_start, the line "poll(&pfd, 1, -1)". I tested that changing -1 to a finite timeout seemed to solve the problem, but it could not be a solution, since it would affect the behaviour of all IGT tests, not only the test kms\_cursor\_crc. 

4. After I have read documentations and seen other crc related tests in IGT, I looked how is the process of pipe crc creation, initialization, crc collection, stop and free of kms\_pipe\_crc\_basic, and two functions called my attention: igt\_pipe\_crc\_new\_nonblock() and igt\_wait\_for\_vblank(). At this point, I remembered a previous talk that I had with Siqueira that the [VKMS simulates vblank interrupts](https://siqueira.tech/misceleneous/add-infrastructure-for-vblank-and-page-flip-events-simulated-via-hrtimer-in-vkms/). For me, It was valid to think in the possibility that VKMS was busy waiting for this vblank interrupt... therefore, as the busy wait was during the starting of pipe_crc, I added a call for igt\_wait\_for\_vblank() before igt\_pipe\_crc\_start() and then, it seems to solve the problem... but, WHY?

```
	igt_wait_for_vblank(data->drm_fd, data->pipe);
	igt_pipe_crc_start(data->pipe_crc);
```

I hope have the answer in a next blog post.

I asked for help to my mentor and took a break to check another problem: Why the subtest alpha-transparent succeed using VKMS after the implementation of XRGB but shows a message of WARNING: Suspicious CRC: All values are 0 ?





