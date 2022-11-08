---
layout: page
title: "Draft - V3D enablement in mainline kernel"
date: 2022-09-22 08:00:00 -0100
draft: draft
permalink: /draft-v3d-enablement
---

Hey,

If you enjoy using upstream Linux kernel in your Raspberry Pi system or just
want to give a try in the freshest kernel graphics drivers there, the good news
is that now you can compile and boot the V3D driver from the mainline in your
Raspberry Pi 4. Thanks to the work of Stefan, Peter and Nicolas[1][2], the V3D
enablement reached the Linux kernel mainline. That means hacking and using new
features available in the upstream V3D driver directly from the source.

However, even for those used to compiling and installing a custom kernel in the
Raspberry Pi, there are some quirks to getting the mainline v3d module
available in 32-bit and 64-bit systems. I've quickly summarized how to compile
and install upstream kernel versions (>=6.0) in this short blog post.

> Note: V3D driver is not present in Raspberry Pi models 0-3.

First, I'm taking into account that you already know how to cross-compile a
custom kernel to your system. If it is not your case, a good tutorial is
already available in the [Raspberry Pi
documentation](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#cross-compiling-the-kernel),
but it targets the kernel in the rpi-linux repository (downstream kernel).

From this documentation, the main differences in the upstream kernel are
presented below:

## Raspberry Pi 4 64-bit (arm64)

### Diff short summary:

1. instead of getting the .config file from `bcm2711_defconfig`, get it by running `make ARCH=arm64 defconfig`
2. compile and install the kernel image and modules as usual, but just copy the dtb file `arch/arm64/boot/dts/broadcom/bcm2711-rpi-4-b.dtb` to the `/boot` of your target machine (no `/overlays` directory to copy)
3. change `/boot/config.txt`:
   * comment/remove the `dt_overlay=vc4-kms-v3d` entry
   * add a `device_tree=bcm2711-rpi-4-b.dtb` entry

## Raspberry Pi 4 32-bits (arm)

### Diff short summary:

1. get the .config file by running `make ARCH=arm multi_v7_defconfig`
2. using `make ARCH=arm menuconfig` or a similar tool, enable `CONFIG_ARM_LPAE=y`
3. compile and install the kernel image and modules as usual, but just copy the dtb file `arch/arm/boot/dts/bcm2711-rpi-4-b.dtb` to the `/boot` of your target machine (no `/overlays` directory to copy)
4. change `/boot/config.txt`:
   * comment/remove the `dt_overlay=vc4-kms-v3d` entry
   * add a `device_tree=bcm2711-rpi-4-b.dtb` entry

## Step-by-step for remote deployment:

### Set variables

**Raspberry Pi 4 64-bit (arm64)**

```
cd <path-to-upstream-linux-directory>
KERNEL=`make kernelrelease`
ARCH="arm64"
CROSS_COMPILE="aarch64-linux-gnu-"
DEFCONFIG=defconfig
IMAGE=Image
DTB_PATH="broadcom/bcm2711-rpi-4-b.dtb"
RPI4=<ip>
TMP=`mktemp -d`
```

**Raspberry Pi 4 32-bits (arm)**

```
cd <path-to-upstream-linux-directory>
KERNEL=`make kernelrelease`
ARCH="arm"
CROSS_COMPILE="arm-linux-gnueabihf-"
DEFCONFIG=multi_v7_defconfig
IMAGE=zImage
DTB_PATH="bcm2711-rpi-4-b.dtb"
RPI4=<ip>
TMP=`mktemp -d`
```

### Get default .config file

```
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE $DEFCONFIG
```

**Raspberry Pi 4 32-bit (arm)**

Additional step for 32-bit system.
Enable `CONFIG_ARM_LPAE=y` using `make ARCH=arm menuconfig`

### Cross-compile the mainline kernel
```
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE $IMAGE modules dtbs
```

### Install modules to send
```
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE INSTALL_MOD_PATH=$TMP modules_install
```

### Copy kernel image, modules and the dtb to your remote system
```
ssh $RPI4 mkdir -p /tmp/new_modules /tmp/new_kernel
rsync -av $TMP/ $RPI4:/tmp/new_modules/
scp arch/$ARCH/boot/$IMAGE $RPI4:/tmp/new_kernel/$KERNEL
scp arch/$ARCH/boot/dts/$DTB_PATH $RPI4:/tmp/new_kernel
ssh $RPI4 sudo rsync -av /tmp/new_modules/lib/modules/ /lib/modules/
ssh $RPI4 sudo rsync -av /tmp/new_kernel/ /boot/
rm -rf $TMP
```

### Set config.txt of you RPi 4
In your Raspberry Pi 4, open the config file `/boot/config.txt`
* comment/remove the `dt_overlay=vc4-kms-v3d` entry
* add a `device_tree=bcm2711-rpi-4-b.dtb` entry
* add a `kernel=<image-name>` entry

## Why not Kworkflow?

You can safely use the steps above, but if you are hacking the kernel and need
to repeat this compiling and installing steps repeatedly, why don't try the
Kworkflow?

[Kworkflow](https://github.com/kworkflow/kworkflow) is a set of scripts to
synthesize all steps to have a custom kernel compiled and installed in local
and remote machines and it supports kernel building and deployment to Raspberry
Pi machines for Raspbian 32 bit and 64 bit.

After learning the kernel compilation and installation step by step, you can
simply use `kw bd` command and have a custom kernel installed in your Raspberry
Pi 4.

> Note: the userspace 3D acceleration (glx/mesa) is working as expected on arm64,
> but the driver is not loaded yet for arm. Besides that, a bunch of pte invalid
> errors may appear when using 3D acceleration, it's a known issue that are still
> under investigation.

[1] https://lore.kernel.org/all/20220601125344.60602-1-stefan.wahren@i2se.com/
[2] https://lore.kernel.org/all/20220603092610.1909675-1-pbrobinson@gmail.com/
