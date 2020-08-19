---
layout: post
title: "If a warning remains, the job is not finished."
date: 2020-08-19 18:00:00 -0300
categories: randomness
--- 

In the past few weeks, I have been combing two issues on vkms: development of
writeback support and alpha blending. I try to keep activities in parallel so
that one can recover me from any tiredness from the other :P

Alpha blending is a TODO[1] of the VKMS that possibly could solve the warning[2]
triggered in the cursor-alpha-transparent testcase. And, you know, if you still
have a warning, you still have work.

1. Blend - blend value at vaddr\_src with value at vaddr\_dst
```
 * TODO: Use the alpha value to blend vaddr_src with vaddr_dst
 *	 instead of overwriting it.
```
2. WARNING: Suspicious CRC: All values are 0

To develop this feature, I needed to understand and "practice" some abstractions
like alpha composition, bitwise operation, and endianness.
The beginning was a little confusing, as I was reading many definitions and
seeing rare practical examples, which was terrible for dealing with
abstractions. I searched for information on Google and little by little, the
code was taking shape in my head.

#### Links that helped me on this topic:

* A quick way to see bitwise operations: [BitwiseCmd](http://bitwisecmd.com/) -
but you can also use bash on terminal
* A blog post with examples: [Bitwise RGBA Values - Faye Williams](https://www.fayewilliams.com/2011/09/21/bitwise-rgba-values/)
* Libraries often have consolidated operations: [Simd Library Documentation](http://ermig1979.github.io/Simd/help/group__drawing.html#ga24392fe6c365dbb60678c9540c860e83)
* Some care with divisions by 255:
  * [Question: Optimized algorithm for constant devision with 255](http://answers.google.com/answers/threadview/id/502016.html)
  * [Quick C++ Benchmark Support Quick Bench Suite](https://quick-bench.com/q/U6BGJ61Pi37PZ8wEqAqMJsArkH8)
  
## The code, the problem solved and a new problem found

I combined what I understood from these links above with what was already on
VKMS. An important note is that I consider that when we combine an alpha layer
in the background, this final composition is solid (that is, an opaque alpha),
that is, the result on the screen is not a transparent plate - it is usually
black background. That said, the resulting alpha channel is 100% opaque, so we
set it to 0xFF.

When executing my code, the two test cases related to the alpha channel
(opaque-transparent) pass clean. But for me, it still wasn't enough. I need to
see the colors read and the result of the composition (putting some pr_info in
the code). Besides, just tests of extreme values ​​(background solid black,
cursor completely white with totally opaque or transparent alpha) did not
convince me. See that I'm a little pessimist and a little wary.

So I decided to play with the cursor-alpha testcase... and I scraped myself.

Ouch!

I changed the transparent test case from 0.0 to 0.5, and things started to
fail. After a few rounds, what I saw was that the cursor color read was (ARGB):
* A: the alpha determined (ok)
* RGB: the color resulted by a white cursor with the set transparency already
applied to a standard solid black background (not ok).

That is, I expected that a white cursor with transparency 0.5 has the
following ARGB = 80FFFFFF, but the VKMS reads 80808080 (???) What is that?

I changed the RGB color of the cursor, I also tested to change the transparency
degree, and the cursor color read is always a combination of a given A + RGB
blended with black solid. Thus, the mismatch happens because, in the hardware
test, when composing a cursor color 80808080, again on a background FF000000
(primary), I obtained as the final result FF404040. However, in the software
test step, what was drawn as a final color FF808080. The developed code would
also do that if it was getting the right color... how to deal with it and why
it is not a problem on i915 drive?

### Reading the documentation

I was reading the cairo manual in addition to perform different color
combinations tests. After some days, I found the reason:

```
CAIRO_FORMAT_ARGB32

each pixel is a 32-bit quantity, with alpha in the upper 8 bits, then red, then
green, then blue. The 32-bit quantities are stored native-endian.
Pre-multiplied alpha is used. (That is, 50% transparent red is 0x80800000, not
0x80ff0000.)

```

Thanks, cairo... or not!

So, I needed to read about the difference between [straight and premultiplied
alpha](https://en.wikipedia.org/wiki/Alpha_compositing#Straight_versus_premultiplied).
I didn't even know it existed. Moreover, I needed to figure out how to do alpha
blending using this format. 

After some searching, I found a post on
[Nvidia - gameworks blog](https://developer.nvidia.com/content/alpha-blending-pre-or-not-pre)
that helped me a lot to understand this approach. Besides, the good old
[StackOverflow](https://stackoverflow.com/questions/4867685/pre-multiplied-alpha-compositing)
clarified what I needed to do.

So I adapted the code I had prepared considering a straight alpha blending so
that it could work with alpha-premultiplied colors. Of course, I put pr\_info
to see the result. I also played a little more with the cursor and background
colors on kms\_cursor\_crc test. For example, I changed the background color to
a solid red and/or the cursor color and/or the cursor alpha value. In all this
situation, the patch works fine. So, I just sent:

- [drm/vkms: add alpha-premultiplied color blending](https://patchwork.freedesktop.org/patch/386755/)

### A new interpretation for the warning

Examining this behavior, I discovered a more precise cause of the warning. As
VKMS currently only overwrites the cursor over the background, a fully
transparent black cursor has a color of 0 and results in a transparent primary
layer instead of solid black. When doing XRGB, VKMS zeroes the alpha instead of
making it opaque, which seems incorrect and triggers the warning.

I also think that this shows the weakness of the full opaque/transparent tests
since they were not able to expose the wrong composition of colors. So I will
prepare a patch for IGT to expand the coverage of this type of problem.

Well, doing the alpha blending, the warning will never bothered me anyway.

Let it go!
