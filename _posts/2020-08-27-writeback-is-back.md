---
layout: post
title: "Another day, another mistery"
date: 2020-08-27 09:00:00 -0300
categories: randomness
--- 

As a newbie, I consider debugging as a study guided.  During the process, I
have a goal that leads me to browse the code, raise and down various
suspicions, look at the changes history and perhaps relate changes from other
parts of the kernel to the current problem. Co-debugging is even more
interesting. Approaches are discussed, we open our mind to more possibilities,
refine solutions and share knowledges... in addition to preventing any
uncomplete solution.  Debugging the operation of vkms when composing plans was
a very interesting journey.  All the shared ideas was so dynamic and open that,
at the end, it was linked to another demand, the implementation of writeback
support.

And that is how I fell into another debugging journey.

Writeback support on vkms is something Rodrigo Siqueira has been working on for
some time.  However, with so many comings and goings of versions, other DRM
changes were introducing new issues. With each new version, it was necessary to
reassess the current state of the DRM structures in addition to incorporating
revisions from the previous version.

When [Leandro Ribeiro reported that calling the drm\_crtc\_vblank\_put frees
the writeback work](https://lkml.org/lkml/2020/7/31/809), this indicate that
there was also a issue with the simulated vblank execution there. I asked
Siqueira a little more about the writeback, to try to collaborate with his
work. He explained to me what writeback is, how it is implemented in his
patches and the IGT test that is used for validation. 

In that state, writeback was crashing on the last subtest,
writeback-check-output. Again, due to a timeout waiting for resource access. We
discussed the possible causes and concluded that the solution would be in
defining: when the composer work needed to be enabled and how to ensure that
vblank is happening to allow also the writeback work. [When Daniel suggested to
create a small function](https://lkml.org/lkml/2020/8/4/726) to set
`output->composer_enabled`, he was already thinking on writeback.

However, something that appeared to be straight was somewhat entangled by our
initial assumption that the problem was concentrated in the
writeback-check-output. We later adjusted our investigation to also look at the
writeback-fb-id subtest.
We began this part of the debugging process using ftrace to check the execution
steps of this two test cases. Looking at different execution cases and more parts
of the code, we examined the writeback job to find the symmetries, the actions
of each element, the beginning and the end of each cycle.
We also includes some prints on VKMS and the test code, and check dmesg.

Siqueira concluded that VKMS need to guarantee the composer is enabled when
starting the writeback job and release it (as well as vblank) in the
job's cleanup ([which is currently called when a job was queued or when the
state is destroyed](https://patchwork.kernel.org/patch/10857785/)).
Another concern was to avoid dispersed modifications and keep the changes well
encapsulated with writeback functions.

With that, Siqueira wrapped up the v5, and sent. Just fly, little bird:

* [drm/vkms: Introduces writeback support](https://patchwork.freedesktop.org/series/80961/)

As soon as it was sent, I checked if the patchset passed not only the
kms\_writeback subtests, but also if the other IGT tests I use have remained
stable: kms\_flip, kms\_cursor\_crc and kms\_pipe\_crc\_basic. With that, I
realized that a refactoring in the composer was out of date and reported the
problem for correction (since it was a fix that I recently made). I also
indicated that, apart from this small problem (not directly related to
writeback), writeback support was working well in the tests performed.
However, the series has not yet received any other feedback.

The end? Hmm... never ends.
