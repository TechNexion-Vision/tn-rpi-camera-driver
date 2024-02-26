# tn-rpi-camera-driver

[![Technexion](https://github.com/TechNexion-Vision/TEV-Jetson_Camera_driver/assets/28101204/08cd2fa9-7333-4a16-819f-c69a3dbf290c)](https://www.technexion.com/products/embedded-vision/)

[![Producer: Technexion](https://img.shields.io/badge/Producer-Technexion-blue.svg)](https://www.technexion.com)
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

## Introduction

[TechNexion Embedded Vision Solutions](https://www.technexion.com/products/embedded-vision/) provide embedded system developers access to high-performance, industrial-grade camera solutions to accelerate their time to market for embedded vision projects.

### Version 0.0.1 (Beta)
---

## Support System Version

- [Raspberry Pi OS (64-bit) 2023-12-06](https://downloads.raspberrypi.com/raspios_arm64/images/raspios_arm64-2023-12-06/) [[kernel 6.1.y]](https://github.com/raspberrypi/linux/tree/rpi-6.1.y)

## Support Camera Modules

#### MIPI Cameras
- TEVS-AR0144-C
- TEVS-AR0234-C
- TEVS-AR0521-C
- TEVS-AR0522-C
- TEVS-AR0522-M
- TEVS-AR0821-C
- TEVS-AR1335-C

[More Camera Products Details...](https://www.technexion.com/products/embedded-vision)

## Supported Raspberry Pi

- [Raspberry Pi 4B](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/)

---
## Install TN Camera on Raspberry Pi

#### Adaptor for **Raspberry Pi Camera**

TEVS-RPI15 Adaptor for TEVS

> Connect TEVS camera and TEVS-RPI15 adaptor to **Raspberry Pi** directly. 

<a href="https://www.technexion.com/products/embedded-vision/mipi-csi2/evk/tevi-ar0144-c-s33-ir-rpi15/" target="_blank">
 <img src="https://www.technexion.com/wp-content/uploads/2023/11/tevs-ar0144-c-s33-ir-rpi15.png" width="400" height="400" />
</a>

---


#### Build drivers from source code (cross-compiling)

1. You can reference [Raspberrypi Documentation - Building the kernel](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel).

2. Make sure the dependencies

```shell
$ sudo apt install -y git bc bison flex libssl-dev make libc6-dev libncurses5-dev

# Install the 64-bit toolchain for a 64-bit kernel
$ sudo apt install -y crossbuild-essential-arm64
```

3. Get the kernel sources

```shell
# raspberrypi linux kerbel
$ git clone -b rpi-6.1.y git@github.com:raspberrypi/linux.git

# technexion rpi camera driver
$ git clone -b tn_rpi_kernel-6.1 git@github.com:TechNexion-Vision/tn-rpi-camera-driver.git
```

4. Copy TN rpi camera driver to raspberrypi linux kernel

```shell
$ cp -r tn-rpi-camera-driver/drivers/media/i2c linux/drivers/media/i2c
$ cp -r tn-rpi-camera-driver/arch/arm64/boot/dts/overlays linux/arch/arm64/boot/dts/overlays
```

5. Build sources

```shell
$ cd linux
$ KERNEL=kernel8

# default configuration
$ make distclean
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2711_defconfig

# config camera
$ make menuconfig
# -> Device Drivers
#   -> Multimedia support
#     -> Media ancillary drivers
#       -> Camera sensor devices
#         -> TechNexion TEVS sensor support
#            Press "m", save and exit
> change "VIDEO_TEVS" to module

# build kernel
$ mkdir -p modules
$ MODULE_PATH=./modules
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- LOCALVERSION="-tn-raspi" -j$(nproc) Image modules dtbs
$ sudo make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- INSTALL_MOD_PATH=$MODULE_PATH modules_install
```

6. Plug in the SD card which burned Raspberry Pi OS (64-bit) 2023-12-06 to PC

7. Install onto the SD card

```shell
$ sudo cp arch/arm64/boot/Image /media/$(users)/bootfs/$KERNEL.img
$ sudo cp arch/arm64/boot/dts/broadcom/*.dtb /media/$(users)/bootfs
$ sudo cp arch/arm64/boot/dts/overlays/*.dtb* /media/$(users)/bootfs/overlays/
$ sudo cp arch/arm64/boot/dts/overlays/README /media/$(users)/bootfs/overlays/
# you can use "make kernelversion" to check kernel version 
$ sudo cp -ra modules/lib/modules/$(make kernelversion)-v8-tn-raspi/ /media/$(users)/rootfs/lib/modules/.
$ sync
```

#### Set up Camera

Modify the config.txt file to add camera configuraion.

```shell
$ sudo nano /boot/firmware/config.txt
```

Modify `camera_auto_detect=0` and add `dtoverlay=tevs,media-controller=0` after the line.
And then `Ctrl+x` > `y` > `Enter` to save file.

```shell
# Automatically load overlays for detected cameras
camera_auto_detect=0
dtoverlay=tevs,media-controller=0
```

Restart system.

```shell
$ sudo reboot
```

---

## Bring up Camera by GStreamer

If you succeed in initialing the camera, you can follow the steps to open the camera.

1. Check the media deivce. (The /dev/mediaN node number may move as they are not fixed allocations).

```shell
$ media-ctl -d /dev/mediaN -p
Device topology
- entity 1: tevs 10-0048 (1 pad, 1 link)
            type V4L2 subdev subtype Sensor flags 0
            device node name /dev/v4l-subdev0
        pad0: Source
                [fmt:UYVY8_2X8/640x480@1/30 colorspace:srgb xfer:srgb ycbcr:601 quantization:full-range
                 crop.bounds:(0,0)/640x480
                 crop:(0,0)/640x480]
                -> "unicam-image":0 [ENABLED,IMMUTABLE]

- entity 3: unicam-image (1 pad, 1 link)
            type Node subtype V4L flags 1
            device node name /dev/video0
         pad0: Sink
                <- "tevs 10-0048":0 [ENABLED,IMMUTABLE]
```

2. Install GStreamer.

```shell
$ sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-base-apps gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-pulseaudio
```

3. List device support formats and resolutions.

```shell
$ gst-device-monitor-1.0
Device found:

        name  : unicam
        class : Video/Source
        caps  : video/x-raw, format=UYVY, width=1280, height=800, pixel-aspect-ratio=1/1, framerate=60/1
                video/x-raw, format=UYVY, width=1280, height=720, pixel-aspect-ratio=1/1, framerate=60/1
                video/x-raw, format=UYVY, width=640, height=480, pixel-aspect-ratio=1/1, framerate=60/1
                video/x-raw(format:Interlaced), format=UYVY, width=640, height=480, pixel-aspect-ratio=1/1, framerate=60/1, interlace-mode=alternate
        properties:
                udev-probed = true
                device.bus_path = platform-fe801000.csi
                sysfs.path = /sys/devices/platform/soc/fe801000.csi/video4linux/video0
                device.subsystem = video4linux
                device.product.name = unicam
                device.capabilities = :capture:
                device.api = v4l2
                device.path = /dev/video0
                v4l2.device.driver = unicam
                v4l2.device.card = unicam
                v4l2.device.bus_info = platform:fe801000.csi
                v4l2.device.version = 393549 (0x0006014d)
                v4l2.device.capabilities = 2241855489 (0x85a00001)
                v4l2.device.device_caps = 85983233 (0x05200001)
        gst-launch-1.0 v4l2src ! ...
```

4. Bring up the camera (/dev/video0) with 640x480 by Gstreamer pipeline.

```shell
$ DISPLAY=:0 gst-launch-1.0 v4l2src device=/dev/video0 ! \
"video/x-raw, format=(string)UYVY, width=(int)640, height=(int)480" ! \
fpsdisplaysink video-sink=glimagesink sync=false
```

5. Change resolution with 1280x720 and bring up by Gstreamer pipeline directly.

```shell
$ DISPLAY=:0 gst-launch-1.0 v4l2src device=/dev/video0 ! \
"video/x-raw, format=(string)UYVY, width=(int)1280, height=(int)720" ! \
fpsdisplaysink video-sink=glimagesink sync=false
```

---

## Troubleshooting


---

## WIKI Pages
