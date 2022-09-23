---
layout: page
title: "Draft - V3D enablement in the mainline"
date: 2022-09-22 08:00:00 -0100
draft: draft
permalink: /draft-v3d-enablement
---

Recently, the work of Stefan, Peter and Nicolas[1][2] for V3D enablement reached the kernel mainline (6.0.0) and we are now able to compile and boot the V3D driver on the Linux kernel mainline in a Raspberry Pi 4. For kernel developers it means hacking and using new features of the upstream V3D driver directly from the source.

However, even for those familiar with compiling and installing a custom kernel in the Raspberry Pi, there are some quirks to getting the mainline v3d module available in 32-bit and 64-bit systems that I've quickly summarized in this short blog post.

> Note: V3D driver is not present in Raspberry Pi models 0-3.

First, I'm taking into account you already know how to cross-compile a custom kernel to your system.
A good tutorial is already available in the [Raspberry Pi documentation](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#cross-compiling-the-kernel), but it targets the kernel in the rpi-linux repository. From this documentation, the main differences in the upstream kernel are presented below: 

## In a cross-compiling env for arm64

1. instead of getting the .config file from `bcm2711_defconfig`, get it by running `make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig`
2. compile and install the kernel image and modules normally, but only copy the dtb file `arch/arm64/boot/dts/broadcom/bcm2711-rpi-4-b.dtb` to the `/boot` of your target machine (no overlays)
3. change `/boot/config.txt`:
   * comment the `dt_overlay=` entry
   * add a `device_tree=bcm2711-rpi-4-b.dtb` entry

## In a cross-compiling env for arm (32-bits)

1. get the .config file by running `make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- multi_v7_defconfig`
2. using `make ARCH=arm menuconfig` or a similar tool, enable `CONFIG_ARM_LPAE=y`
3. compile and install the kernel image and modules normally, but only copy the dtb file `arch/arm/boot/dts/bcm2711-rpi-4-b.dtb` to the `/boot` of your target machine (no overlays)
4. change `/boot/config.txt`:
   * comment the `dt_overlay=` entry
   * add a `device_tree=bcm2711-rpi-4-b.dtb` entry
  
Note: the userspace 3D acceleration (glx/mesa) is working as expected on arm64, but the driver is not loaded yet for
arm. Besides that, a bunch of pte invalid errors may appear when using 3D acceleration, it's a known issue that are still under investigation.

[1] https://lore.kernel.org/all/20220601125344.60602-1-stefan.wahren@i2se.com/
[2] https://lore.kernel.org/all/20220603092610.1909675-1-pbrobinson@gmail.com/
