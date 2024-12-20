---
layout: post
title: "Status update - Tie up loose ends before starting"
date: 2020-06-02 15:00:00 -0300
categories: gsoc-2020
--- 

As the GSoC coding time officially started this week (01/06/2020), this post is
a line between my activities in the period of community bonding and the
official development of my project. I used the last month to solve issues in my
development environment, improve the tool that supports my development activity
(kworflow), and study concepts and implementations related to my GSoC project.

### Communication

Besides e-mail, IRC chat, and Telegram, my mentor (Siqueira) and I are meeting
every Wednesday on [Jitsi](https://meet.jit.si/), where we also use tmate for
terminal sharing. We also use, together with Trevor, a spreadsheet to schedule
tasks, report my daily activity, and write any suggestions.

### Issues in my development environment

Probably you have already had this kind of pending task: a problem that trouble
your work but you can go ahead with it, therefore you postpone until the total
tiredness. So, I have some small issues on my vm: from auth to update kernel
process that I was postpone forever. The first I solved with some direct setup
and the last led me for the second task: improve kworkflow.

### Improvements for a set of scripts that facilitate my development day-to-day

The lack of support on kworkflow for deployment on vm drove me to start hacking
the code. As updating vm needs understanding things more complex, I started
developing soft changes (or less integrated with the scripts structure). This
work is still in progress, and after discuss with others kworkflow developers
(on the issue, on IRC and on voice meetings), the proposals of changes were
refined.

* [Fix alert message](https://github.com/kworkflow/kworkflow/pull/176)
* [Expand kw explore and add GNU grep](https://github.com/kworkflow/kworkflow/pull/178)
* [Development of kw deploy --vm](https://github.com/kworkflow/kworkflow/pull/181)

### Study concepts and code related to my GSoC project

The first phase of my project proposal orbits the IGT test kms\_cursor\_crc.  I
have already a preliminar experience with the test code; however, I lack a lot
of knowledge of each steps and concepts behing the implementation. Bearing this
in mind, I used part of the community bonding time to immerse myself in this
code.

#### Checking issues in a patchset that was reported by IGT CI

My first project task is to find out why it is not possible to access debugfs
files when running kms\_cursor\_crc (and fix it). Two things could help me
solve it: learning about debugfs and dissecting kms\_cursor\_crc. To guide my
studies, my mentor suggested taking a look at a [patchset for the IGT write-back
test](https://patchwork.freedesktop.org/series/68352/) implementation that CI
reported a crash on debugfs\_test for i915. For this
investigation, I installed on another machine (an old netbook) a Debian without
a graphical environment, and, accessing via ssh, I applied the patches and ran
the test. Well, everything seemed to work (and the subtests passed). Perhaps
something has been fixed or changed in IGT since the patchset was sent. Nothing
more to do here.

```

IGT_FORCE_DRIVER=i915 build/tests/debugfs_test 
IGT-Version: 1.25-gf1884574 (x86_64) (Linux: 4.19.0-9-amd64 x86_64)
Force option used: Using driver i915
Force option used: Using driver i915
Starting subtest: sysfs
Subtest sysfs: SUCCESS (0,009s)
Starting subtest: read_all_entries
Subtest read_all_entries: SUCCESS (0,094s)
Starting subtest: read_all_entries_display_on
Subtest read_all_entries_display_on: SUCCESS (0,144s)
Starting subtest: read_all_entries_display_off
Subtest read_all_entries_display_off: SUCCESS (0,316s)

```

#### Diving into the kms\_cursor\_crc test

I'm writing a kind of anatomy from the kms\_cursor\_crc test. I chose the
alpha-transparent subtest as a target and then followed each step necessary to
achieve it, understanding each function called, parameters, and also
abstractions. I am often confused by something that once seemed clear. Well, it
comes to graphic interface stuff and is acceptable that theses abstraction will
disorientate me LOL I guess... The result of this work will be my next post.
In the meantime, here are links that helped me on this journey

* [X.Org/HowVideoCardsWork](https://www.x.org/wiki/Development/Documentation/HowVideoCardsWork/)
* [The DRM/KMS subsystem from a newbie’s point of view](https://events.static.linuxfound.org/sites/events/files/slides/brezillon-drm-kms.pdf)
* [Linux Graphics Drivers: an Introduction/Stéphane Marchesin](https://people.freedesktop.org/~marcheu/linuxgraphicsdrivers.pdf)
* [Cairo Manual](https://www.cairographics.org/manual/cairo-cairo-surface-t.html)
* [igt-gpu-tools Reference Manual](https://drm.pages.freedesktop.org/igt-gpu-tools/igt-gpu-tools-Framebuffer.html)

