---
layout: post
title: "How was my first steps on IIO Subsystem"
date: 2019-04-10 14:03:00 -0300
categories: randomness
---

I've already started my contribution journey. However, I forgot to write about it earlier.
In this post, I present my memories of this previous phase: (1) Before being in person with the FLUSP group,  my attempts to understand the Linux project flow and how to start to contribute; (2) Developing initial skills with support from FLUSP (people and materials); (3) Sending my first path;

## Activities before FLUSP

Because I was not at USP, I have remotely started to set my development environment. To this, I follow instructions of Rodrigo Siqueira (the most experient Linux developer of my study laboratory) via e-mails, tutorials, and sporadical video conference. Besides some technical issues related to distro portability and personal environment, I was always in doubt if I do everything right. This doubt reduced my motivation, and slow down my progress in this preliminary step. I think I just needed a few words like: "ok, no problem, go ahead". 

I have never sent a patch in this remote period. I have suspended twice my activity of setting the environment. Always when I restarted, I forgot where I have stopped and reinitialized everything.

Now, when I reflect on this previous phase, I think I didn't trust my self because I have an idea of Linux development as being too complex to be so light to do. I believed that only with a lot of acknowledging about the whole kernel I would be able to contribute. Developing currently to IIO subsystem I can see that it is not fundamental. It is possible to anyone contribute to the kernel following a kind of divide-and-conquest strategy.

## Developing with FLUSP

When I was at FLUSP, I believe I move faster from this setup step to the next step of sending my first patch. This acceleration has happened because: (1) Remotely, I advanced well my environment configuration (even without knowing it); (2) FLUSP has improved its Linux tutorials to mitigate many newcomer issues; (3) I was able to immediately check with colleagues on my progress and also to solve doubts.

Basically, I used three tutorials to set up my work environment and start coding and making things happen:
1. To setup a VM for safe working: https://flusp.ime.usp.br/others/2019/02/15/use-qemu-to-play-with-linux
2. To compile and install kernel - seeing my future code changes: https://flusp.ime.usp.br/others/2019/02/16/Kernel-compilation-and-installation
3. To develop contributions for a module: https://flusp.ime.usp.br/others/2019/04/07/play_with_modules

## Sending my first patch

FLUSP's tutorials made everything easier. However, they also allow a superficial comprehension of the process by impatient newcomers. I also had the privilege to do this step with the mentoring of Siqueira. So I learnt the workflow and where to find/what to do in the process of sending my first patch.

My first patch was a code style improvement. I decided to contribute to the IIO subsystem because everyone said its maintainer is kind and patient. I believe one of the top fear of newcomer is to receive a rude review of your initial contributions and possible mistakes.

I felt fear in the exact moment of press enter to send my patch via email (even with the approval of my patch by FLUSP more experienced members).

Another concern around this experience was the time to maintainer review my patch. Fortunately, I know the window of his analysis and took care to remain patient. But I can comprehend the anxiety of others.
The maintainer has accepted my patch and also pointed out the limitation of this type of contribution. 


