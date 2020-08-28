---
layout: post
title: "The last patch sent in 2019"
date: 2019-12-26 17:40:00 -0300
categories: linux-kernel-newbie
---

2019 was a year of deepening and maturing my MSc research on FLOSS. It was also a year of introduction to the universe of contributions to the kernel Linux. I had my first achievements as a newcomer to the community. I sent eight patches to the IIO, the first one was a code-style contribution, but in the others, I made some improvements for an IIO driver that was (and still is) in the staging tree. These patches helped me to understand a little of the logic and complexity involved in Linux kernel development.

I believe one of the patches that struck me most was to fix an implementation error in the ad7150 driver (staging: iio: ad7150: fix threshold mode config bit).
I know it's not very complicated, but this patch involved a lot of positive things. First, it was a bug discovered by chance when I was making a readability improvement of the code. I consider it as a consequence of being careful to always look at a driver's datasheet when making any code modifications. Second, in a way, I became more confident in what I was doing (given the abstraction needed to code for a device you don't have it physically installed).

Finally, the flow of sending a bug fix is ​​somewhat particular in the Linux universe. This patch was an opportunity to learn more about Linux's contributions flow.
1. Ideally, you should submit a concise patch, only to fix the bug and without other code changes. I can say that I changed a little more than recommended since, besides the bug, I believe the variable name would induce future errors or even the return of the found bug - since the mistake was an inconsistency in a configuration bit indicated in the Datasheet, working oppositely.
2. Siqueira told me that bug fix usually has priority over other kinds of contributions. He told me to see other bug-fix patches sent to Linux in a way to learn about some specificities in the patch format. With his help, I understood that it is important to make clear in the commit message that this is a bug-fix patch. Also, it is helpful to place a tag "Fixes: <hash and message of the commit that introduced the error>"
3. When Jonathan accepted my patch, he sent it to the fixes-togreg branch, unlike the other contributions forwarded to branch x.
4. Interestingly enough, while my patch was merged in other top- trees, I could see different paths that my patch took. Besides being much faster, other kernel versions merged the bug-fix, unlike my other contributions to the same driver.
5. Finally, this patch was at a happy time. I sent it on my birthday and during the Kernel Dev Day event sponsored by my FLOSS contribution group at USP, the FLUSP :)

Shortly after this patch, I had to stop my contributions to write my qualifying exam manuscript. I submitted my research project for qualification in July, but I no longer returned to send patches to IIO. 

However, I didn't stop my Linux development activities out there. I started preparing to contribute to another Linux subsystem, DRM. To this end, I spent the last months of 2019 reading a little more about the subsystem and preparing my development environment. I hope 2020 is a year to dive into Linux and become a skilled kernel developer!
