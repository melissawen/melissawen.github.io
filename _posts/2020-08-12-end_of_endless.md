---
layout: post
title: "The end of an endless debugging of an endless wait"
date: 2020-08-12 20:00:00 -0300
categories: randomness
--- 

After a long debugging process, we finally reached an acceptable solution to
the problem of running sequential subtests of IGT tests involving CRC capture
using the VKMS.

Despite all the time, uncertainty, and failures, this was, by far, one of the
greatest lessons learned in my journey of FLOSS developer. With
this experience, I learned a lot about how VKMS works, evolved technically,
experienced different feelings, and interacted with people I may never meet in
person. These people were sharing some of their knowledge with me in open and
accessible communication. The interactions allowed us to build together a more
suitable and effective solution, and at that moment, everything I thought was
important in the development of FLOSS came to fruition.

I know it sounds very romantic, but after more than a month of it, breathing
is not a simple and involuntary movement.

With this patch, I was able to update my progress table on GSoC:

| Result/Status    | GSoC start| After accepted patches | 
| ----------|:-------:|-------:|
|       pass|        7|24 (+17)|
|       warn|        1|1|
|       fail|        2|0 (-2)|
|       skip|      236|221 (-15)|
|      crash|        0|0|
|    timeout|        0|0|
| incomplete|        0|0|
| dmesg-warn|        0|0|
| dmesg-fail|        0|0|
|    changes|        0|0|
|      fixes|        0|0|
|regressions|        0|0|
|      total|      246|246|

## The endless wait

I believe I have already talked about the problem in some other posts as it
has been followed me since the elaboration of my GSoC project.

I was trying to solve the failure of one of the IGT kms\_cursor\_crc subtests,
the cursor-alpha-transparent. As this subtest has an implementation similar to
the cursor-alpha-opaque (that was passing), I used both to observe the
manipulation of ARGB -> XRGB happening in CRC capture. However, strangely, the
cursor-alpha-opaque kept alternating between success and failure using the same
kernel. Gradually I noticed that this was a widespread problem with the
sequential execution of subtests in VKMS.

In the beginning, I only noticed that the test crashed while waiting for a
vblank event that was never going to happen. So I looked for solutions that
enabled vblanks in time to prevent the subtest get stuck. However, as Daniel
mentioned, these solutions just looked like duct-tape.

## The endless debugging

Initially, I found two problems and thought that both were related to the test
implementation. So I sent patches to IGT to correct the cleanup process and
to force a vblank. In waiting for feedback, I realized that a previously
successful subtest was currently failing, even with the solution I
intended — bad smell. I searched the patch that modified this
behavior in the history and, in fact, reversing the change let the subtests
flow.

It was then that I saw a patch sent to the DRM mailing-list in which someone
faced a similar problem and decided to participate in the discussion. From this
point, doubts and suspicions increased even more. However, on the bright side,
I wasn't the only person looking for it. With our time zone differences, we
were three sharing the findings for someone else's next shift.

I sent a first patch about this issue on June 25th (in the wrong direction).
After so much feedback, adjustments, and some other wrong attempts, the
solution was finally accepted on August 8th.

The whole story is documented in the following discussions:

1. https://patchwork.freedesktop.org/series/78813/
2. https://patchwork.freedesktop.org/patch/373563/?series=79003
3. https://patchwork.freedesktop.org/patch/378382/?series=79758
4. https://patchwork.freedesktop.org/patch/381344/?series=80043
5. https://patchwork.freedesktop.org/series/80162/

And when it all seemed to come to an end, a fourth person appeared, raising
doubts about other VKMS parts and that, in a way, was related to the problem
at hand.

And that was amazing! There's still a lot of work out there!

Now I'm still a little tired of it, but I'll try to make a more technical post
about the solution. I changed the context, going back to that will give me a
little mental work.

## For the next developments

Don't keep the problem to yourself; share your doubts.

When making a patch, don't forget the credits! It doesn't make much difference
for you, but it can do for those who were with you on that journey. [See
more](https://www.kernel.org/doc/html/v4.10/process/submitting-patches.html)

Each thread/patch is full of information that can help you with other problems.

If you can't dive into the code, browse through it.

The mental effort is less if you write a post before changing the context.

P.S.: I still need to find a way to feel comfortable speaking on IRC.
