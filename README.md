# tn-rpi-camera-driver

[![Technexion](https://github.com/TechNexion-Vision/TEV-Jetson_Camera_driver/assets/28101204/08cd2fa9-7333-4a16-819f-c69a3dbf290c)](https://www.technexion.com/products/embedded-vision/)

[![Producer: Technexion](https://img.shields.io/badge/Producer-Technexion-blue.svg)](https://www.technexion.com)
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

## Introduction

[TechNexion Embedded Vision Solutions](https://www.technexion.com/products/embedded-vision/) provide embedded system developers access to high-performance, industrial-grade camera solutions to accelerate their time to market for embedded vision projects.

### Version 0.0.1 (Beta)
---

## Support System Version

- [Raspberry Pi OS (64-bit) 2024-03-15](https://downloads.raspberrypi.com/raspios_full_arm64/images/raspios_full_arm64-2024-03-15/) [[kernel 6.6.y]](https://github.com/raspberrypi/linux/tree/rpi-6.6.y)

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

- [Raspberry Pi 5](https://www.raspberrypi.com/products/raspberry-pi-5/)
- [Raspberry Pi 4 Model B](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/)

---
## Install TN Camera on Raspberry Pi

#### Adaptor for **Raspberry Pi Camera**

TEVS-RPI22 Adaptor for TEVS

> Connect TEVS camera and TEVS-RPI22 adaptor to **Raspberry Pi 5 - "CAM/DISP 1"** directly. 

<a href="https://www.technexion.com/products/embedded-vision/mipi-csi2/evk/tevs-ar0144-c-s33-ir-rpi22/" target="_blank">
 <img src="https://www.technexion.com/wp-content/uploads/2023/11/tevs-ar0144-c-s33-ir-rpi22.png" width="400" height="400" />
</a>

TEVS-RPI15 Adaptor for TEVS

> Connect TEVS camera and TEVS-RPI15 adaptor to **Raspberry Pi 4 Model B** directly. 

<a href="https://www.technexion.com/products/embedded-vision/mipi-csi2/evk/tevi-ar0144-c-s33-ir-rpi15/" target="_blank">
 <img src="https://www.technexion.com/wp-content/uploads/2023/11/tevs-ar0144-c-s33-ir-rpi15.png" width="400" height="400" />
</a>

---

#### Method 1 - Using Technexion Pre-built modules, only for **kernel 6.6.20+rpt-rpi-2712 (RPI5)** or **kernel 6.6.20+rpt-rpi-v8 (RPI4)**

1. Make a SD card with *"Raspberry Pi OS (64-bit) 2024-03-15"* on Raspberry Pi Imager.

2. Boot RPI5/RPI4 with SD card.

3. Download pre-built modules.

For RPI5:
```shell
$ wget https://download.technexion.com/demo_software/EVK/RPI/RPI5/pre-built-modules/latest/tn_camera_module_rpi5_6.6.y.tar.gz
```

For RPI4:
```shell
$ wget https://download.technexion.com/demo_software/EVK/RPI/RPI4/pre-built-modules/latest/tn_camera_module_rpi4_6.6.y.tar.gz
```

4. Uncompress the modules.

For RPI5:
```shell
$ tar -xf tn_camera_module_rpi5_6.6.y.tar.gz
```

For RPI4:
```shell
$ tar -xf tn_camera_module_rpi4_6.6.y.tar.gz
```

5. Run installation script.

For RPI5:
```shell
$ cd tn_camera_module_rpi5_6.6.y/
$ sh tn_install.sh
****** TechNexion Camera Driver Installation ******
This installation is easy to install TechnNexion Camera Drivers for Raspberry Pi 5. 
Before start to install camera driver, You should BACKUP your image and config 
to avoid any file you lost while installing process.
Do you want to continue?[Y/n]y
Continuing with the installation...
Install TN-CAM modules: tevs.ko.xz
Installed TN-CAM module file Done.
Install TN-CAM DTBO file: tevs-rpi22.dtbo
Installed TN-CAM DTBO file Done.
Add TN-CAM Configuration for modules: tevs
Install TN-CAM service...
Launch TN-CAM Service...
Created symlink /etc/systemd/system/multi-user.target.wants/tn_cam.service → /etc/systemd/system/tn_cam.service.
Finish Camera Driver Installation. Return Code:[1]
You should Reboot Device to enable TEVS Cameras.
Do you want to reboot now?[Y/n]y
Rebooting....
```

For RPI4:
```shell
$ cd tn_camera_module_rpi4_6.6.y/
$ sh tn_install.sh
****** TechNexion Camera Driver Installation ******
This installation is easy to install TechnNexion Camera Drivers for Raspberry Pi 4. 
Before start to install camera driver, You should BACKUP your image and config 
to avoid any file you lost while installing process.
Do you want to continue?[Y/n]y
Continuing with the installation...
Install TN-CAM modules: tevs.ko.xz
Installed TN-CAM module file Done.
Install TN-CAM DTBO file: tevs-rpi15.dtbo
Installed TN-CAM DTBO file Done.
Add TN-CAM Configuration for modules: tevs
Install TN-CAM service...
Launch TN-CAM Service...
Created symlink /etc/systemd/system/multi-user.target.wants/tn_cam.service → /etc/systemd/system/tn_cam.service.
Finish Camera Driver Installation. Return Code:[1]
You should Reboot Device to enable TEVS Cameras.
Do you want to reboot now?[Y/n]y
Rebooting....
```

---

#### Method 2 - Build drivers from source code (cross-compiling)

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
$ git clone --depth=1 -b rpi-6.6.y https://github.com/raspberrypi/linux

# technexion rpi camera driver
$ git clone --depth=1 -b tn_rpi_kernel-6.6 https://github.com/TechNexion-Vision/tn-rpi-camera-driver.git
```

4. Copy TN rpi camera driver to raspberrypi linux kernel

```shell
$ cp -rv tn-rpi-camera-driver/drivers/media/i2c/* linux/drivers/media/i2c/
$ cp -rv tn-rpi-camera-driver/arch/arm64/boot/dts/overlays/* linux/arch/arm64/boot/dts/overlays/
```

5. Build sources

For RPI5:
```shell
$ cd linux
$ KERNEL=kernel_2712

# default configuration
$ make distclean
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2712_defconfig
```

For RPI4:
```shell
$ cd linux
$ KERNEL=kernel8

# default configuration
$ make distclean
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2711_defconfig
```

configuring camera and building kernel image.
```shell
# config camera
$ make menuconfig
# -> Device Drivers
#   -> Multimedia support
#     -> Media ancillary drivers
#       -> Camera sensor devices
#         -> TechNexion TEVS sensor support
#            Set "VIDEO_TEVS" to module,
#            Press "m", save to original name (.config) and exit

# build kernel
$ mkdir -p modules
$ MODULE_PATH=./modules
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- LOCALVERSION="-tn-raspi" -j$(nproc) Image modules dtbs
$ sudo make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- INSTALL_MOD_PATH=$MODULE_PATH modules_install
```

6. Plug in the SD card which burned *"Raspberry Pi OS (64-bit) 2024-03-15 "* to PC

7. Install onto the SD card

```shell
$ sudo cp arch/arm64/boot/Image /media/$(users)/bootfs/$KERNEL.img
$ sudo cp arch/arm64/boot/dts/broadcom/*.dtb /media/$(users)/bootfs
$ sudo cp arch/arm64/boot/dts/overlays/*.dtb* /media/$(users)/bootfs/overlays/
$ sudo cp arch/arm64/boot/dts/overlays/README /media/$(users)/bootfs/overlays/
# you can use "make kernelversion" to check kernel version 
$ sudo cp -ra modules/lib/modules/$(make kernelversion)-v8-16k-tn-raspi/ /media/$(users)/rootfs/lib/modules/.
$ sync
```

8. Boot RPI5/RPI4 with SD card.

9. Modify the config.txt file to add the camera configuraion.

For RPI5:
```shell
$ sudo nano /boot/firmware/config.txt

# Automatically load overlays for detected cameras
> camera_auto_detect=0
> dtoverlay=tevs-rpi22
```
Modify `camera_auto_detect=0` and add `dtoverlay=tevs-rpi22` after the line.\
Then `Ctrl+s` to save file and `Ctrl+x` to exit.

For RPI4:
```shell
$ sudo nano /boot/firmware/config.txt

# Automatically load overlays for detected cameras
> camera_auto_detect=0
> dtoverlay=tevs-rpi15,media-controller=0
```

Modify `camera_auto_detect=0` and add `dtoverlay=tevs-rpi15,media-controller=0` after the line. \
Then `Ctrl+s` to save file and `Ctrl+x` to exit.

10. Ensure dependent modules loading order.

```shell
$ sudo touch /etc/modprobe.d/tevs.conf
$ sudo nano /etc/modprobe.d/tevs.conf
> softdep tevs pre: gpio_pca953x
```
Add a configuration file for loading dependent modules. \
Then `Ctrl+s` to save file and `Ctrl+x` to exit.

11. Restart system.

```shell
$ sudo reboot
```

---

## Bring up Camera by GStreamer

If you succeed in initialing the camera, you can follow the steps to open the camera. \
RPi5 always requires Media Controller to configure the pipeline as the front end is more complex since you can't just drive it from /dev/video0.

1. Check the media deivce. (The **/dev/mediaN** node number may move as they are not fixed allocations).

For RPI5:
```shell
$ media-ctl -d /dev/mediaN -p
Media controller API version 6.6.22

Media device information
------------------------
driver          rp1-cfe
model           rp1-cfe
serial          
bus info        platform:1f00128000.csi
hw revision     0x114666
driver version  6.6.22

Device topology
- entity 1: csi2 (8 pads, 8 links)
            type V4L2 subdev subtype Unknown flags 0
            device node name /dev/v4l-subdev0
        pad0: Sink
                [fmt:SRGGB10_1X10/640x480 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
                <- "tevs 4-0048":0 [ENABLED,IMMUTABLE]
        pad1: Sink
                [fmt:unknown/16384x1 field:none]
        pad2: Sink
                [fmt:SRGGB10_1X10/640x480 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
        pad3: Sink
                [fmt:SRGGB10_1X10/640x480 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
        pad4: Source
                [fmt:SRGGB10_1X10/640x480 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
                -> "rp1-cfe-csi2_ch0":0 []
                -> "pisp-fe":0 []
        pad5: Source
                [fmt:unknown/16384x1 field:none]
                -> "rp1-cfe-embedded":0 []
        pad6: Source
                [fmt:SRGGB10_1X10/640x480 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
                -> "rp1-cfe-csi2_ch2":0 []
                -> "pisp-fe":0 []
        pad7: Source
                [fmt:SRGGB10_1X10/640x480 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
                -> "rp1-cfe-csi2_ch3":0 []
                -> "pisp-fe":0 []

- entity 10: pisp-fe (5 pads, 7 links)
             type V4L2 subdev subtype Unknown flags 0
             device node name /dev/v4l-subdev1
        pad0: Sink
                [fmt:SRGGB16_1X16/640x480 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
                <- "csi2":4 []
                <- "csi2":6 []
                <- "csi2":7 []
        pad1: Sink
                [fmt:FIXED/16384x1 field:none]
                <- "rp1-cfe-fe_config":0 []
        pad2: Source
                [fmt:SRGGB16_1X16/640x480 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
                -> "rp1-cfe-fe_image0":0 []
        pad3: Source
                [fmt:SRGGB16_1X16/640x480 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
                -> "rp1-cfe-fe_image1":0 []
        pad4: Source
                [fmt:FIXED/16384x1 field:none]
                -> "rp1-cfe-fe_stats":0 []

- entity 16: tevs 4-0048 (1 pad, 1 link)
             type V4L2 subdev subtype Sensor flags 0
             device node name /dev/v4l-subdev2
        pad0: Source
                [fmt:UYVY8_1X16/640x480@1/30 field:none colorspace:srgb xfer:srgb ycbcr:601 quantization:full-range
                 crop.bounds:(0,0)/640x480
                 crop:(0,0)/640x480]
                -> "csi2":0 [ENABLED,IMMUTABLE]

...


```

For RPI4:
```shell
$ media-ctl -d /dev/mediaN -p
Media controller API version 6.6.20

Media device information
------------------------
driver          unicam
model           unicam
serial          
bus info        platform:fe801000.csi
hw revision     0x0
driver version  6.6.20

Device topology
- entity 1: tevs 10-0048 (1 pad, 1 link)
            type V4L2 subdev subtype Sensor flags 0
            device node name /dev/v4l-subdev0
        pad0: Source
                [fmt:UYVY8_1X16/640x480@1/30 field:none colorspace:srgb xfer:srgb ycbcr:601 quantization:full-range
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

3. Enable csi2 link for video0 node. (**only for RPI5**)

```shell
$ media-ctl -d /dev/mediaN -l '1:4 -> '\''rp1-cfe-csi2_ch0'\'':0 [1]'
```

4. Bring up the camera (/dev/video0) with 640x480 by Gstreamer pipeline.

For RPI5:
```shell
$ media-ctl -d /dev/mediaN -V '16:0 [fmt:UYVY8_1X16/640x480 field:none colorspace:srgb xfer:srgb ycbcr:601]'
$ DISPLAY=:0 gst-launch-1.0 v4l2src device=/dev/video0 ! \
"video/x-raw, format=(string)UYVY, width=(int)640, height=(int)480" ! \
fpsdisplaysink video-sink=glimagesink sync=false
```

For RPI4:
```shell
$ DISPLAY=:0 gst-launch-1.0 v4l2src device=/dev/video0 ! \
"video/x-raw, format=(string)UYVY, width=(int)640, height=(int)480" ! \
fpsdisplaysink video-sink=glimagesink sync=false
```

5. Change resolution with 1280x720 and bring up by Gstreamer pipeline directly.

For RPI5:
```shell
$ media-ctl -d /dev/mediaN -V '16:0 [fmt:UYVY8_1X16/1280x720 field:none colorspace:srgb xfer:srgb ycbcr:601]'
$ DISPLAY=:0 gst-launch-1.0 v4l2src device=/dev/video0 ! \
"video/x-raw, format=(string)UYVY, width=(int)1280, height=(int)720" ! \
fpsdisplaysink video-sink=glimagesink sync=false
```

For RPI4:
```shell
$ DISPLAY=:0 gst-launch-1.0 v4l2src device=/dev/video0 ! \
"video/x-raw, format=(string)UYVY, width=(int)1280, height=(int)720" ! \
fpsdisplaysink video-sink=glimagesink sync=false
```

---

## Troubleshooting


---

## WIKI Pages
