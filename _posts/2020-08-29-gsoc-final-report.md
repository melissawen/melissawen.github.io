---
layout: post
title: "GSoC Final Report"
date: 2020-08-31 07:00:00 -0300
categories: gsoc-2020
---

Hi!

I haven't said *Hi* for a while when starting a post. I think the rush and
the whirlwind of things happening during the GSoC made me a little
agitated. This year,
[my project](https://summerofcode.withgoogle.com/projects/#4669149428580352)
was the only one accepted for
[X.Org Foundation](https://www.x.org/wiki/XorgFoundation/), and I felt a
great responsibility. Well, this is the last week of the project, and I'm
slowing down and breathing :)

This report is a summary of my journey at Google Summer of Code 2020.
Experience and technical reports can be read in more detail in the 11 blog
posts I published during this period:

| Date | Post    |
|:-----|:--------|
|2020/05/13| [I'm in - GSoC 2020 - X.Org Foundation](https://melissawen.github.io/blog/2020/05/13/im-in-gsoc)|
|2020/05/20| [Everyone makes a script](https://melissawen.github.io/blog/2020/05/20/community-bounding)|
|2020/06/02| [Status update - Tie up loose ends before starting](https://melissawen.github.io/blog/2020/06/02/status-update)|
|2020/06/03| [Walking in the KMS CURSOR CRC test](https://melissawen.github.io/blog/2020/06/03/overview_kms_cursor_crc)|
|2020/06/15| [Status update - connected errors](https://melissawen.github.io/blog/2020/06/15/status-update)|
|2020/07/06| [GSoC First Phase - Achievements](https://melissawen.github.io/blog/2020/07/06/first-round)|
|2020/07/17| [Increasing test coverage in VKMS - max square cursor size](https://melissawen.github.io/blog/2020/07/17/add-max-cursor-size.html)|
|2020/08/12| [The end of an endless debugging of an endless wait](https://melissawen.github.io/blog/2020/08/12/end_of_endless.html)|
|2020/08/19| [If a warning remains, the job is not finished.](https://melissawen.github.io/blog/2020/08/19/let-vkms-blend-it.html)|
|2020/08/27| [Another day, another mistery](https://melissawen.github.io/blog/2020/08/27/writeback-is-back)|
|2020/08/28| [Better validation of alpha-blending](https://melissawen.github.io/blog/2020/08/28/translucent-cursor-testcase)|

So, the Google Summer of Code was an amazing experience! I have evolved not
only technically, but also as a developer in a software community. In this
period, I could work on different projects and even interact with their
community. I contributed to three projects with various purposes, sizes,
and maturities, and I describe below a little of my relationship with each
of them:

#### DRM/Linux Kernel

The [Linux kernel](https://www.kernel.org/) is one of the largest, most famous, and most mature free and
open-source project. It is also the kernel that I have been
using for over ten years. The development of Linux is so interesting to me that
I chose it as a case study of my Master's in Computer Science research.

Among the various subsystems in the project, I have contributed to
[DRM](https://01.org/linuxgraphics/gfx-docs/drm/gpu/introduction.html),
the part of Linux responsible for the interface with GPUs. It provides the
user-space with API to send commands and data in a format suitable for modern
GPUs. It is the kernel-space component of graphic stacks like the [X.Org Server](https://www.x.org/wiki/).

And was X.Org Foundation, the organization that supported my project in GSoC.
Thanks to the support from the DRI community and the X.Org Foundation, I have
contributed over the past few months to improve the
[VKMS](https://dri.freedesktop.org/docs/drm/gpu/vkms.html). The
Virtual Kernel Mode Setting is a software-only model of a KMS driver that allows
you to test DRM and run X on systems without a hardware display capability.

#### IGT GPU Tools

[IGT](https://gitlab.freedesktop.org/drm/igt-gpu-tools)
is a set of tools used to develop and validate DRM drivers. These tools can
be used by drivers other than Intel drivers and I widely used it to
evolve the VKMS. Using IGT to improve VKMS can be very useful for validating
patches sent to the core of DRM, that is, performing automated tests against
new code changes with no need of real hardware.

With this concern, all my work on GSoC aimed to bring greater stability in
IGT tests' execution using VKMS. IGT test cases guided most of my
contributions to the VKMS. Before sending any code, I took care of
validating if, with my change, the tests that I have any familiarity remained
stable and properly working.

#### KWorkFlow

[Kworflow](https://github.com/kworkflow/kworkflow) is a set of scripts that I
use in my development environment for the Linux kernel. It greatly facilitates
the execution of routine tasks of coding, examining the code, and sending
patches.

My mentor developed it, [Rodrigo Siqueira](https://siqueira.tech/) and other
students and former students of computer science at the university contributed
to add functionality and evolve the project. It supports your local kernel
version's compilation and deployment, helps you browse the code and the change
history, and provides essential information for patch formatting.

With these three projects, I had an exciting development journey and many
lessons learned to share. Here is a summary of that experience:

## From start to finish

The general purpose of my GSoC project was to evolve VKMS using IGT tests. In
this way, I used the kms\_cursor\_crc test case as a starting point to fix and
validate features, adjust behaviors, and increase test coverage in VKMS.

```

[KMS cursor crc uses]
the display CRC support to validate cursor plane functionality.
The test will position the cursor plane either fully onscreen,
partially onscreen, or fully offscreen, using either a fully opaque
or fully transparent surface. In each case, it enables the cursor plane
and then reads the PF CRC (hardware test) and compares it with the CRC
value obtained when the cursor plane was disabled and its drawing is
directly inserted on the PF by software.

```

In my project proposal, I presented the piglit statistics for kms\_cursor\_crc
using VKMS. It was seven test cases successful, two fails, one warning (under
development), and 236 skips. Now, I can present an overview of this state and
the improvements mapped and applied during this GSoC period:

**Failing tests**

Initially, three test cases failed using VKMS. Before GSoC start, I had already
sent a proposal that moved it from failure to success with a warning and was
related to the composition of planes considering the alpha channel.
The second was a case that had already worked two years ago. The
latter had never worked before. This last two were related to the behavior
of the cursor plane in power management tasks.

**From failure to warning**

Since VKMS did not consider the alpha channel for blending, it zeroed that
channel before computing the crc. However, the operation that would do this was
zeroing the wrong channel, due to an endianness trap. It led the test case to
failure. Even after fixing this operation, the test still emitted a warning.
This happened because, when zeroing the alpha channel, VKMS was delivering a fully
transparent black primary plane for capturing CRC, while the background should
be a solid black.

**A cross-cutting problem that affected the performance of VKMS for sequential subtests execution**

During the sequential execution of the kms\_cursor\_crc test cases, the results
were unstable. Successful tests failed, two runs of the same test alternated
between failure and success ... in short, a mess.

In debugging and examining the change history, I found a commit
that changes the VKMS performance of the kms\_cursor\_crc test cases. This
change replaced the drm\_wait\_for\_vblanks function with drm\_wait\_flip\_done
and, therefore, the VKMS stopped "forcing" vblank interrupts during the process
of state commit. Without vblank interruptions, the execution of testcases in
sequence got stuck. Forcing vblanks was just a stopgap and not a real solution.
Besides, the IGT test itself was also leaving a kind of trash when failure
happened, since it did not complete the cleanup and affected the next subtest.

**Skips due to unmet of test requirements**

Skips were caused by the lack of the following features in VKMS:
* Cursor sizes larger than 64x64 and non-square cursor (few drivers take this
   approach)
* Support for more than one CRTC (still not developed):

```

  Test requirement not met in function igt_require_pipe, file ../lib/igt_kms.c:1900:
  Test requirement: !(!display->pipes[pipe].enabled)
  Pipe F does not exist or not enabled

```

So, for each of these issues, I needed to identify the problem, find out what
project the problem was coming from, map what was required to resolve, and then
code the contribution.
With that, I had to combine work from two fronts: DRM and IGT. Also, with the
more intensive use of my development environment, I needed
to develop some improvements in the tool that supports me in compiling,
installing, and exploring the kernel using a virtual machine. As a consequence,
I also sent some contributions to the Kworkflow project.

### Patches sent during GSoC 2020 to solve the above issues

| ## | Project    | Patch   | Status | Blog post |
|----|:-----------|:--------|:-------|:---------:|
| 01 | DRM/Linux kernel | [drm/vkms: change the max cursor width/height](https://patchwork.freedesktop.org/patch/376183/) | Accepted | [Link](https://melissawen.github.io/blog/2020/07/17/add-max-cursor-size) |
| 02 | DRM/Linux kernel | [drm/vkms: add missing drm\_crtc\_vblank\_put to the get/put pair on flush](https://patchwork.freedesktop.org/patch/378382/) | Discarded | - |
| 03 | DRM/Linux kernel | [drm/vkms: fix xrgb on compute crc](https://patchwork.freedesktop.org/patch/382341/) | Accepted | - |
| 04 | DRM/Linux kernel | [drm/vkms: guarantee vblank when capturing crc (v3)](https://patchwork.freedesktop.org/patch/384615/) | Accepted | [Link](https://melissawen.github.io/blog/2020/08/13/end_of_endless) |
| 05 | DRM/Linux kernel | [drm/vkms: add alpha-premultiplied color blending (v2)](https://patchwork.freedesktop.org/patch/387665/) | Accepted | [Link](https://melissawen.github.io/blog/2020/08/19/let-vkms-blend-it) |
| 06 | IGT GPU Tools | [lib/igt\_fb: remove extra parameters from igt\_put\_cairo\_ctx](https://patchwork.freedesktop.org/series/79162/) | Accepted | [Link](https://melissawen.github.io/blog/2020/07/06/first-round) |
| 07 | IGT GPU Tools | [[i-g-t,v2,1/2] lib/igt\_fb: change comments with fd description](https://patchwork.freedesktop.org/patch/377581/) | Accepted | [Link](https://melissawen.github.io/blog/2020/07/06/first-round) |
| 08 | IGT GPU Tools | [[i-g-t,v2,2/2] test/kms\_cursor\_crc: update subtests descriptions and some comments](https://patchwork.freedesktop.org/patch/377582) | Accepted | [Link](https://melissawen.github.io/blog/2020/07/06/first-round) |
| 09 | IGT GPU Tools | [[i-g-t,v2,1/1] test/kms\_cursor\_crc: release old pipe\_crc before create a new one](https://patchwork.freedesktop.org/patch/377631) | Accepted | [Link](https://melissawen.github.io/blog/2020/06/15/status-update) |
| 10 | IGT GPU Tools | [[i-g-t,2/2] test/kms\_cursor\_crc: align the start of the CRC capture to a vblank](https://patchwork.freedesktop.org/patch/372942) | Discarded | [Link](https://melissawen.github.io/blog/2020/06/15/status-update) |
| 11 | IGT GPU Tools | [[i-g-t] tests/kms\_cursor\_crc: refactoring cursor-alpha subtests](https://patchwork.freedesktop.org/patch/387991) | Under review | [Link](https://melissawen.github.io/blog/2020/08/28/translucent-cursor-testcase) |
| 12 | Kworkflow | [src: add grep utility to explore feature](https://github.com/kworkflow/kworkflow/pull/178) | Merged | [Link](https://melissawen.github.io/blog/2020/05/20/community-bounding) |
| 13 | Kworkflow | [kw: small issue on u/mount alert message](https://github.com/kworkflow/kworkflow/pull/176) | Merged | [Link](https://melissawen.github.io/blog/2020/05/20/community-bounding) |
| 14 | Kworkflow | [Add support for deployment in a debian-VM](https://github.com/kworkflow/kworkflow/pull/181) | Under review | [Link](https://melissawen.github.io/blog/2020/05/20/community-bounding) |

With the patches sent, I collaborated to:

* Increasing test coverage in VKMS
* Bug fixes in VKMS
* Adjusting part of the VKMS design to its peculiarities as a fake driver allowing to stabilize its work performance
* Fixing a leak in kms\_cursor\_crc cleanup when a testcase fails
* Improving different parts of the IGT tool documentation
* Increasing the effectiveness of testing cursor plane alpha-composition
* Improve my development environment and increase my productivity.

### Not only coding, getting involved in the community

In addition to sending code improvements, my initial proposal included adding
support for real overlay planes. However, I participated in other community
activities that led me to adapt a portion of my project to meet emerging and
more urgent demands.

I took a long time debugging the VKMS's unstable behavior together with other
DRM community members. Initially, I was working in isolation on the solution.
However, I realized that that problem had a history and that it would be more
productive to talk to other developers to find a suitable solution. When I saw
on the mailing list, another developer had encountered a similar problem, and I
joined the conversation. This experience was very enriching, where I had the
support and guidance of DRM's maintainer, [Daniel Vetter](https://blog.ffwll.ch/).
Debugging together with Daniel and Sidong Yang, we got a better solution to the
problem. Finally, it seems that somehow, this debate contributed to
another developer, [Leandro Ribeiro](https://gitlab.freedesktop.org/leandrohrb),
in his work on another VKMS's issue.

In this debugging process, I also gained more experience and confidence
concerning the project. So, I reviewed and tested some patches sent to
VKMS [[1](https://patchwork.freedesktop.org/patch/374926/),[2](https://patchwork.freedesktop.org/patch/377557/),[3](https://patchwork.freedesktop.org/patch/381344/),[4](https://patchwork.freedesktop.org/patch/387714/)]. Finally, with the knowledge acquired, I was
also able to contribute to the debugging of a feature under development that
adds support for writeback [[5]](https://patchwork.freedesktop.org/series/80961/).

## Discussion and future works

The table below summarizes my progress in stabilizing the execution of
kms\_cursor\_crc using VKMS.

| Status    | Start   | End     | 
| ----------|:-------:|:--------|
|       pass|        7|25 (+18) |
|       warn|        1|0 (-1)   |
|       fail|        2|0 (-2)   |
|       skip|      236|221 (-15)|
|      total|      246|246      |


To solve the warning triggered by the test case
pipe-A-cursor-alpha-transparent, I needed to develop an already mapped feature:
`TODO: Use the alpha value to blend vaddr\_src with vaddr\_dst instead of overwriting it in blend()`.
This feature showed that another test case, the pipe-A-cursor-alpha-opaque, had
a false-pass. Moreover, both test cases related to the composition of the
cursor plane with the primary plane were not sufficient to verify the
composition's correctness. As a result, I sent IGT a refactoring of the test
cases to improve coverage.

The handling of the two failures initially reported were built by the same
debugging process. However, they had different stories. The dpms test case had
already been successful in the past, according to a [commit in the git
log](https://patchwork.freedesktop.org/patch/247563/). It started to fail after
a [change](https://patchwork.freedesktop.org/patch/318968/), which was correct
but evidenced the driver's deficiency. There was no report for the suspend test
case that it worked correctly at some point, but it lacked the same: ensuring
that vblank interruptions are occurring for the composition work and CRC
captures.

The remaining skips are related to:
1. non-square cursor support. Which, as far as I know, is a very specific feature
for Intel drivers. If this restricted application is confirmed, the right thing
to do is moving these test cases to the specific i915 driver test folder.
2. the test requirement for more than one pipe: "Pipe (B-F) does not exist or
not enabled". This need a more complex work to add support for more than one
CRTC.

I also started examining how to support the real overlay:
* create a parameter to enable the overlay;
* enable the third type of plane overlay in addition to the existing primary
and cursor;
* and allow the blending of the three planes.

#### Lessons learned

1. In one of my first contributions to the VKMS I received feedback, let's say,
scary. To this day, I have no idea who that person was, and maybe he didn't
want to scare me, but his lack of politeness to say 'That looks horrid' was
pretty disheartening for a beginner. Lucky for me that I didn't take it so
seriously, I continued my journey, started GSoC, and realized that the most
active developers in the community are very friendly and inclusive. I learned a
lot, and I would like to thank them for sharing their knowledge and
information.
2. Still related to the previous topic, I feel that the less serious developers
also don't care much about contribution etiquette and code of conduct. By
unknowing or overlooking the rules of the community, they end up creating
discomfort for other developers. In the DRM community, I noticed care in
maintaining the community and bringing in new contributors. I did not feel
unworthy or discredited in any discussion with the others. On the contrary, I
felt very encouraged to continue, to learn, and to contribute.
3. In addition to the diversity of skills, maturity, and culture of the
   developers, the Linux community deals with timezone differences. It's crazy
to see the energy of the DRI community on IRC channel, even with people
sleeping and waking up at random times.
4. I still don't feel very comfortable talking on the IRC channel, for two
reasons: I always take time to understand the subject being discussed
technically, and my English is not very good. The dialogues I had by e-mail
were very useful. It gave me time to think and check the validity of my ideas
before speaking. It was still a record/documentation for future contributions.
5. The Linux kernel is very well documented. For most questions, there is an
   answer somewhere in the documentation. However, it is not always immediate
   to find the answer because it is an extensible project. Besides, a lot has
   already been encapsulated and encoded for reuse, so creating something from
   scratch can often be unnecessary.

#### And after GSoC

Well, my first, short-term plan is to make a good presentation at XDC 2020.
In it, I will highlight interesting cases on my project of working on IGT and
VKMS together. My presentation will be on the first day, September 16; more
details:

[VKMS improvements using IGT GPU Tools](https://xdc2020.x.org/event/9/contributions/625/)

My second plan is to continue contributing to the VKMS, initially adding the
mapped features of adding a real overlay and structuring the VKMS for more than
one CRTC. Probably, with the entry of writeback support, doing these things
requires even more reasoning.

Not least, I want to finish my master's degree and complete my research on
Linux development, adding my practical experiences with VKMS development.
I’m almost there, hopefully later this year.
And then, I hope to find a job that will allow me to work on Linux development.
And live. :P

## Acknowledgment

Finally, I thank X.Org Foundation for accepting my project and believing in my
performance (since I was the organization's sole project in this year's GSoC).
Also, [Trevor Woerner](http://twoerner.blogspot.com/) for motivation,
communication and confidence! He was always very attentive, guiding me and
giving tips and advice.

Thank my mentor, Rodrigo Siqueira, who believed in my potential, openly shared
knowledge that he acquired with great effort, and gave relevance to each
question that I presented, and encouraged me to talk with the community. Many
thanks also to the DRI community and Daniel Vetter for sharing their time and
so much information and knowledge with me and being friendly and giving me
constructive feedback.

#### Additional Links

1. https://patchwork.freedesktop.org/patch/374926/
2. https://patchwork.freedesktop.org/patch/377557/
3. https://patchwork.freedesktop.org/patch/381344/
4. https://patchwork.freedesktop.org/patch/387714/
5. https://patchwork.freedesktop.org/series/80961/
