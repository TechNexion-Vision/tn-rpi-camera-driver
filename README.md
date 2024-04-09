# tn-rpi-camera-driver

[![Technexion](https://github.com/TechNexion-Vision/TEV-Jetson_Camera_driver/assets/28101204/08cd2fa9-7333-4a16-819f-c69a3dbf290c)](https://www.technexion.com/products/embedded-vision/)

[![Producer: Technexion](https://img.shields.io/badge/Producer-Technexion-blue.svg)](https://www.technexion.com)
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

## Introduction

[TechNexion Embedded Vision Solutions](https://www.technexion.com/products/embedded-vision/) provide embedded system developers access to high-performance, industrial-grade camera solutions to accelerate their time to market for embedded vision projects.

### Version 0.0.1 (Beta)

---

## Support System Version

- [Raspberry Pi OS 32-bit 2022-01-28](https://downloads.raspberrypi.com/raspios_full_armhf/images/raspios_full_armhf-2022-01-28/) [[kernel 5.10.y]](https://github.com/raspberrypi/linux/tree/rpi-5.10.y)

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

#### Method 1 - Using Technexion Pre-built modules, only for **kernel 5.10.92-v7l+**

1. Make a SD card with *"Raspberry Pi OS 32-bit 2022-01-28"* on Raspberry Pi Imager.

2. Boot RPI4 with SD card.

3. Modify raspberry pi config when using the camera for the first time.

```shell
$ sudo raspi-config
```

and then choose `Interface Options` > `Camera` > `Yes` > `Ok` > `Finish` > `No`.\
Note: Don't reboot system immediately, you will need to install camera driver.

4. Download pre-built modules.

```shell
$ wget https://download.technexion.com/demo_software/EVK/RPI/RPI4/pre-built-modules/latest/tn_camera_module_rpi4_5.10.y.tar.gz
```

5. Uncompress the modules.

```shell
$ tar -xf tn_camera_module_rpi4_5.10.y.tar.gz
```

6. Run installation script.

```shell
$ cd tn_camera_module_rpi4_5.10.y/
$ sh tn_install.sh
****** TechNexion Camera Driver Installation ******
This installation is easy to install TechnNexion Camera Drivers for Raspberry Pi 4. 
Before start to install camera driver, You should BACKUP your image and config 
to avoid any file you lost while installing process.
Do you want to continue?[Y/n]y
Continuing with the installation...
Install TN-CAM modules: tevs.ko
Installed TN-CAM module file Done.
Install TN-CAM DTBO file: tevs-rpi15.dtbo
Installed TN-CAM DTBO file Done.
Add TN-CAM Configuration for modules: tevs-rpi15
Install TN-CAM service...
Launch TN-CAM Service...
Created symlink /etc/systemd/system/multi-user.target.wants/tn_cam.service → /etc/systemd/system/tn_cam.service.
Job for tn_cam.service failed because the control process exited with error code.
See "systemctl status tn_cam.service" and "journalctl -xe" for details.
Finish Camera Driver Installation. Return Code:[1]
You should Reboot Device to enable TEVS Cameras.
Do you want to reboot now?[Y/n]y
Rebooting....
```

---

#### Method 2 - Build drivers from source code (cross-compiling)

1. You can reference [Raspberrypi Documentation - Building the kernel](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel).

2. Make sure the dependencies.

```shell
$ sudo apt install -y git bc bison flex libssl-dev make libc6-dev libncurses5-dev

# Install the 32-bit toolchain for a 32-bit kernel
$ sudo apt install -y crossbuild-essential-armhf
```

3. Get the kernel sources.

```shell
# raspberrypi linux kerbel
$ git clone --depth=1 -b rpi-5.10.y https://github.com/raspberrypi/linux

# technexion rpi camera driver
$ git clone --depth=1 -b tn_rpi_kernel-5.10 https://github.com/TechNexion-Vision/tn-rpi-camera-driver.git
```

4. Copy TN rpi camera driver to raspberrypi linux kernel.

```shell
$ cp -rv tn-rpi-camera-driver/drivers/media/i2c/* linux/drivers/media/i2c/
$ cp -rv tn-rpi-camera-driver/arch/arm/boot/dts/overlays/* linux/arch/arm/boot/dts/overlays/
```

5. Build sources.

```shell
$ cd linux
$ KERNEL=kernel7l
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig

# default configuration
$ make distclean
$ make bcm2711_defconfig

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
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- LOCALVERSION="-tn-raspi" -j$(nproc) zImage modules dtbs
$ sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=$MODULE_PATH modules_install
```

6. Plug in the SD card which burned *"Raspberry Pi OS 32-bit 2022-01-28"* to PC.

7. Install onto the SD card.

```shell
$ sudo cp arch/arm/boot/zImage /media/$(users)/boot/$KERNEL.img
$ sudo cp arch/arm/boot/dts/*.dtb /media/$(users)/boot
$ sudo cp arch/arm/boot/dts/overlays/*.dtb* /media/$(users)/boot/overlays/
$ sudo cp arch/arm/boot/dts/overlays/README /media/$(users)/boot/overlays/
# you can use "make kernelversion" to check kernel version
$ sudo cp -ra modules/lib/modules/$(make kernelversion)-v7l-tn-raspi/ /media/$(users)/rootfs/lib/modules/.
$ sync
```

8. Boot RPI4 with SD card.

9. Modify raspberry pi config when using the camera for the first time.

```shell
$ sudo raspi-config
```

and then choose `Interface Options` > `Camera` > `Yes` > `Ok` > `Finish` > `No`.\
Don't reboot system immediately, you will need to add camera configuraion manually.

10. Add `dtoverlay=tevs-rpi15` in the last line and `Ctrl+x` > `y` > `Enter` to save file.

```shell
$ sudo nano /boot/config.txt

> dtoverlay=tevs-rpi15
```

11. Restart system.

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

2. Install GStreamer

```shell
$ sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-base-apps gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-pulseaudio
```

3. Bring up the camera (/dev/video0) with 640x480 by Gstreamer pipeline:

```shell
$ DISPLAY=:0 gst-launch-1.0 v4l2src device=/dev/video0 ! \
"video/x-raw, format=(string)UYVY, width=(int)640, height=(int)480" ! \
fpsdisplaysink video-sink=glimagesink sync=false
```

4. Change resolution with 1280x720 and bring up by Gstreamer pipeline

```shell
$ media-ctl -d /dev/mediaN --set-v4l2 '1:0[fmt:UYVY8_2X8/1280x720]'
$ DISPLAY=:0 gst-launch-1.0 v4l2src device=/dev/video0 ! \
"video/x-raw, format=(string)UYVY, width=(int)1280, height=(int)720" ! \
fpsdisplaysink video-sink=glimagesink sync=false
```

---

## Troubleshooting


---

## WIKI Pages
