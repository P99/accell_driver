# Linux kernel driver for accelerometer LIS3DH

## Hardware

We are running on BeagleBoardBlack hardware (ARM architecture)
The accelerometer chip is ST Microelectronics LIS3DH.
For convenience, we are using [Adafruit LIS3DH breakout](https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout/pinouts)
The accelerometer chip is connected through the I2C bus.

The adafruit LIS3DH is connected as follow

Adafruit pin | BBB pin | Name
--- | --- | ---
Vin | 4 | VDD_3V3
3Vo | | NC
GND | 2 | GND
SCL | 19 | I2C2_SCL
SDA | 20 | I2C2_SDA
SDO | | NC
CS | | NC
INT | 23 | GPIO_49 

## Design

The driver is a misc device so we don't have to care about finding a suitable "MINOR_VERSION" for the module

We are using a simple character device "lxaccell" to read acceleration values on three axis (X, Y, Z)

## Environment setup

The I2C bus is already up and running on the original debian image, so you don't actually need to recompile the kernel

Here is a quick hack to get you up and running:
  On the board, install the kernel headers:
  root@beaglebone:~# sudo apt-get install linux-headers-`uname -r`

  Then copy over all the headers: (modifiy the kernel version accordingly)
  scp -r root@bbb:/usr/src/linux-headers-3.8.13-bone70 .

  Replace linux-headers-3.8.13-bone70/scripts by a cross-compiled version of your own
  Grab the cross-toochain on [linaro](http://releases.linaro.org/components/toolchain/binaries/4.9-2016.02/arm-linux-gnueabihf/) website

Then, the classic cross-compilation command:
  make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-