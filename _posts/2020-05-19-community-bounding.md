---
layout: post
title: "Everyone makes a script"
date: 2020-05-20 15:00:00 -0300
categories: gsoc-2020
--- 

Meanwhile, in GSoC:

I took the second week of Community Bonding to make some improvements in my
development environment. As I have reported before, I use a QEMU VM to develop
kernel contributions. I was initially using an Arch VM for development;
however, at the beginning of the year, I reconfigured it to use a Debian VM,
since my host is a Debian installation - fewer context changes. In this
movement, some ends were loose, and I did some workarounds, well... better
round it off.

I also use kworkflow (KW) to ease most of the no-coding tasks included in the
day-to-day coding for Linux kernel. The KW automates repetitive steps of a
developer's life, such as compiling and installing my kernel modifications;
finding information to format and send patches correctly; mounting or remotely
accessing a VM, etc. During the time that preceded the GSoC project submission,
I noticed that the feature of installing a kernel inside the VM was
incompleted. At that time, I started to use the "remote" option as palliative.
Therefore, I spent the last days learning more features and how to hack the
kworkflow to improve my development environment (and send it back to the kw
project).

I have started by sending a minor fix on alert message:

[kw: small issue on u/mount alert
message](https://github.com/kworkflow/kworkflow/pull/176)

Then I expanded the feature "explore" - looking for a string in directory
contents - by adding GNU grep utility in addition to the already used git grep.
I gathered many helpful suggestions for this patch, and I applied them together
with the reviews received in a new version:

[src: add grep utility to explore
feature](https://github.com/kworkflow/kworkflow/pull/178/commits)

Finally, after many hours of searching, reading and learning a little about
[guestfish](http://libguestfs.org/guestfish.1.html),
[grub](https://help.ubuntu.com/community/Grub2/Setup),
[initramfs-tools](https://manpages.debian.org/testing/initramfs-tools-core/initramfs-tools.7.en.html)
and [bash](https://www.gnu.org/software/bash/manual/bash.pdf), I could create
the first proposal of code changes that enable kw to automate the build and
install of a kernel in VM:

[add support for deployment in a
debian-VM](https://github.com/kworkflow/kworkflow/pull/181)

The main barrier to this feature was figuring out how to update the grub on the
VM without running the update-grub command via ssh access. First, I thought
about adding a custom file with a new entry to boot. Thinking and researching a
little more, I realized that guestfish could solve the problem and, following
this logic, I found a [blog
post](https://blog.svedr.in/posts/imageless-vm-builds-with-libguestfs/)
describing how to run "update-grub" with guestfish. From that, I made some
adaptations that created the solution.

However, in addition to updating grub, the feature still lacks some steps to
install the kernel on the VM properly. I checked the missing code by visiting
an old [FLUSP tutorial that describes the step-by-step of compiling and install
the Linux Kernel inside a
VM](https://flusp.ime.usp.br/others/Kernel-compilation-and-installation/). I
also used the implementation of the "remote" mode of the "kw deploy" to wrap
up.

Now I use kw to automatically compile and install a custom kernel on my
development VM. So, time to sing: "Ooh, that's why I'm easy; I'm easy as Sunday
morning!"

Maybe not now. It's time to learn more about IGT tests!
