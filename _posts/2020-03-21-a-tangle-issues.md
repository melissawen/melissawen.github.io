---
layout: post
title: "A tangle of issues"
date: 2020-03-23 14:00:00 -0300
categories: randomness
--- 

After I began a simple change in the cursor behavior in VKMS, many related
issues started to appear. My initial task seemed simple, sending a proposal to
enable cursor by default when loading the vkms module. However, I have now
spent a lot of time untangling issues that troubled me to validate that change.

This initial task arose after I have asked on the dri-devel IRC channel for
suggestions to contribute to VKMS because I am interested in participating in
this year's Google internship program (GSoC). As VKMS's mentor is Rodrigo
Siqueira, he suggested this warm-up task. Each day, I become more familiar with
VKMS, so I could quickly realize what I need to modify, and I proposed a simple
modification with this patch:

[PATCH] drm/vkms: enable cursor by default

However,  as a newbie, I was not confident only with the logical change on the
code. I was curious to see this behavior on bits (or something like this). For
this, I thought to look for the cursor on the framebuffer. As I have already
played with the IGT test kms\_enable\_cursor, I considered using the framebuffer
of this test to see the cursor.

First, according to the context, I checked if variables related to cursor
presented values that represent the enabling, putting a lot of pr_info to
say: "ok, it looks like working". Second, I thought I could find a white
cursor in the framebuffer of this test whenever the cursor was enabled, since
it also is a requirement for the test execution.

With this in mind, I ran the subtest pipe-A-cursor-alpha-transparent to
looking for this white cursor and see it become transparent. Then, after some
computing, the transparent cursor plane would be blend with the other planes...
but, ops... where is the cursor?

"Well, maybe there is a problem with the subtest. Let's run the
pipe-A-cursor-alpha-opaque test that I know is working and right passing! 

Ops, crash? It's weird!"

To face this weird situation encouraged me to dive into the elements of IGT
test and understand a little more of what is happening: the abstraction
involved, the test execution step by step, searching on web information about
Cairo operations called by the tests, etc.

I realized the pipe-A-cursor-alpha-transparent check the CRC from the framebuffer
that has already blended a cursor transparent in a black background, with this,
it's not possible to find a white cursor in any step of the test, that was my
initial intention.

This interpretation matches the output of the pixels printed in my investigation,
where only have two possible ARGB pixel values: transparent black and opaque
black. I am still not sure if my comprehension is correct, but following this logic
helped me to find a solution for the pipe-A-cursor-alpha-transparent test and also
put the other things on a backlog of investigation.

### Backlog of issues to treat

1. Using a VM to run both kms\_enable\_cursor subtests:
   pipe-A-cursor-alpha-transparent/opaque, I put some pr\_info to check the path
of execution inside vkms features. With this, I could understand better the
process of planes blending and crc computing, and also check if every time that I
run a subtest, all steps are executed. From the opaque-subtest episode of
crashing, I figured out an unstable behavior, I verified that the test fails
and any pr\_info inside vkms functions are not printed. I asked Siqueira about
this problem, and he suggested to dive inside the [hrtimer/vblank
operations](https://siqueira.tech/report/add-infrastructure-for-vblank-and-page-flip-events-simulated-via-hrtimer-in-vkms/)
to check if something is causing a delay. A code snippet that deserves
attention and may be involved in this problem is at vkms\_composer.c:

``` /*
 * The worker can fall behind the vblank hrtimer, make sure we catch up.  */
   while (frame_start <= frame_end)
      drm_crtc_add_crc_entry(crtc, true, frame_start++, &crc32);

```

I also checked that the file /sys/kernel/debug/dri/0/crtc-0/crc/data that
stores framebuffer and crc values (as explained by [Haneen in this
post](http://haneensa.github.io/2018/09/09/CRC/)) was blocked and I was
unallowed to see its content. These suspicions together lead me to think of two
possible problems: a lost lock/unlock operation or long busy writing operation.

2. Maybe this issue is related to the problem above. Although I found a solution
for pipe-A-cursor-alpha-transparent, this subtest still displays a warning that
needs attention: Suspicious CRC: All values are 0. The weird thing is when I
print the pixels in the test, because they are actually all zero (black: 0x000000)
after setting the alpha channel for zero, ie, ARGB to XRGB. But this warning does
not appear when running past versions of kernel on a host machine.

Could it be some debufs implementation fail or some problem on the IGT test
construction? I hope to have good news in the next post update.

### Finding a solution for pipe-A-cursor-alpha-transparent crash

I found a strategy to focus on validating the correctness of the code change to
make the test pass. The idea was that the solution would work for both the
transparent and the opaque cursor. It also needed to check if the procedure was
correct by printing the pixel values before and after the XRGB
operation. This concern was because, after several executions, I realized that
most of the proposed solutions fall into a trap related to the endianness of
orders of magnitude (bits x bytes). Then, the ideal solution would be
extracting the RGB values without depending on whether the operation to ignore
the alpha channel was little or big-endianness.

Using a previous experience of contributing to the IIO, I thought about using
bitwise / bitfield operations to ensure the interpretation in bits and the
extraction of only the RGB bits of interest.

With that, I define a GENMASK that visualized only the first 24 bits (from
right to left) and extract those bits using the FIELD\_GET function defined in
the file: linux / bitfield.h. I learned a little about this operation in a
patch I sent a year ago to improve the readability of an IIO staging driver:
[staging: iio: ad7150: use FIELD\_GET and
GENMASK](https://patchwork.kernel.org/patch/10995927/). And so my solution was
born:

[PATCH] drm/vkms: use bitfield op to get xrgb on compute crc

I am still waiting for community evaluation and feedback. And now I have new
knowledge and a lot of new challenges.
