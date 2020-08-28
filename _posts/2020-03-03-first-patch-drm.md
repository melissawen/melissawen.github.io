---
layout: post
title: "First patches to the DRM community"
date: 2020-03-04 10:00:00 -0300
categories: linux-kernel-newbie
--- 

In this post, I talk a little about my first steps to introduce myself to the
DRM community, a Linux subsystem from which I am looking to learn more and
develop skills.

In my opinion, sending contributions is the best way to start to fit in. We can
also send questions or interact about an idea, but as nobody knows you there,
showing your potential and your interest in practice leads to a more fluid
relationship.

Well, I can say that a safe way to start contributing code to a subsystem of
which we are little acquainted is by sending code style improvement patches.
They are simple patches, and many believe that they have no relevance or
translate into technical capacity. For me, they don't reflect in technical
ability; however, they break the first barriers of sending and communication and
make it possible to gradually learn about how to modify a large volume of code.

With that in mind, I tried to contribute in three ways:
1. some code style improvements;
2. removing unused code; and
3. fixing bugs

### Improving code style

I can see two direct contributions generate for this kind of patch: getting rid
of warnings (giving space to look to relevant problems), and improving the code
readability lead to lot of good consequences for coworking remotely.

A good way to discover codestyle problems is using checkpatch.pl. Even better is
using [kworkflow](https://github.com/kworkflow/kworkflow) to guide you, since
the tool has a option to check codestyle and to discover the maintainers
responsible for the file (in case of sending contributions).

So, I sent a patcheset to clean up two functions in a AMD's file that is full of
code style problems:

[PATCH v2 0/2] drm/amd/display: dc_link: cleaning up some code style issues
[PATCH v2 1/2] drm/amd/display: dc_link: code clean up on enable_link_dp
function [PATCH v2 2/2] drm/amd/display: dc_link: code clean up on detect_dp
function

Examples of code style issues include lines with more than 80 characteres,
avoiding comparisons to NULL in conditional clauses, and alignment of
parenthesis and indentations.

These simple patches call my attention for the branch that I was using for
development. Since the file is maintained by AMD team, instead of using the
drm-misc repository on the branch drm-misc-next, I should base on repository of
Alex Deucher(agd5f/amdgpu). Asking him on IRC channel (#dri-devel) for the right
branch to send my contribution, he said: drm-next. So, I needed to rebase my
patch from drm-misc to the right branch.

### Removing unused code

The [2017 Linux Kernel Development
Report](https://go.pardot.com/l/6342/2017-10-24/3xr3f2/6342/188781/Publication_LinuxKernelReport_2017.pdf)
declared:

``` The kernel has grown steadily since its first release in 1991, when there
were only about 10,000 lines of code. At almost 25 million lines (up from nearly
22 million), the kernel is almost three million lines larger than it was at the
time of the previous version of this report. 
```

Someone can think "Wow, it is great!", but is important to noticed that large
size is not direct related to power or potential. Lines of code need of
maintainance, which in turn needs skilled developers and
organization/management. So, unused code seems inoffencive, but it is not. It
may affect maintainability and readability. Thinking in a house and in how
terrible is living with a lot of trash or things that you no longer use in your
life.

Even in the house analogy, when there is a messy room in your house, it is hard
to clean up and decide what stay and what live. So, for getting rid of a unused
code you need to check is someone still need it. In a large project like Linux,
there are a lot of files and people involved. So, first, check the files that
the stuff appears, and then send the removal as patch, and wait for someone
claims the necessity of it.

Then, I sent a patch to remove an entire function from another AMD's file.

[PATCH] drm/amd/display: dcn20: remove an unused function

This was a suggestion from Siqueira. However I also looking for apparison of
this function in the whole project and also in the same file, as I didn't find
nothing, I removed and sent. Done!

### Attempt to fix bugs

For me, fixing bugs is a small and punctual task, but very challenging. Often,
it also requires a breadth of knowledge. Besides, it is a task that, when
completed, brings satisfaction and some confidence.

I have tried to solve two mapped problems on vkms: a bug reported by the
syzkaller and a bug found by an IGT test.

#### Bug reported by syzkaller

I have sent a e-mail for dri-vel mailing list asked for help. In this e-mail, I
describe my attempts:

I tried to reproduce a syzkaller bug found in the vkms [WARNING in
vkms\_gem\_free\_object](https://groups.google.com/forum/#!msg/syzkaller-bugs/_oARhriB1SA/PelnW1BqAwAJ)
However, I was not very successful in this task.

Looking at the bug history at
https://syzkaller.appspot.com/bug?extid=e7ad70d406e74d8fc9d0, it seems like the
bug still exists.

For testing, I used a VM (QEMU) with Debian 10 with a compiled kernel from
https://cgit.freedesktop.org/drm/drm-misc (branch drm-misc-next)

1. Using the usual .config for my VM, I compiled and installed the kernel and,
   as root, ran the C program provided by syzkaller:
[1](https://syzkaller.appspot.com/x/repro.c?x=15e27c53600000 Nothing happened.)

2. Then, I checked the debug/panic/hacking/drm/i915 debugging/vkms settings on
   the .config reported by syzkaller:
[2](https://syzkaller.appspot.com/x/.config?x=7cf4eed5fe42c31a) and enabled the same
things in my .config.  I compiled and installed the kernel and ran the C
program.  Nothing happened.

3. So, I reverted my current branch to the commit that generated the bug (as
   reported: 94e2ec3f7fef86506293a448273b2b4ee21e6195) and used the kernel on
that state.  Nothing happened.

4. I decided to use the syzkaller .config without modifications and adaptations
   for my VM (although I didn't think it felt right). I compiled, installed...
some boot problems happened, but the kernel worked. I ran the C program and
nothing.

Daniel Vetter replied by saying that he also had no experience with syzkaller
bug reports and that perhaps it would be better for me to seek guidance from the
syzkaller community. I also asked Rodrigo Siqueira and he also had no previous
experience in this.

An then I gave up... for a while :)

#### IGT test failure

So I pass to the next bug, some failure to compute the CRC with the transparent
cursor. The kms\_cursor\_crc subtest (pipe-A-cursor-alpha-transparent) that
checks the CRC when the cursor is transparent was failing, and everything
indicated that it was a problem around the XRGB operation that ignored the
alpha channel using the C memset function.

This bug took me a lot of time to comprehend the VKMS operations of drawing
cursor, blending planes, and computing CRC, and also understand the steps of
this IGT subtest until asserting values. See more about this situation in the
next post.
