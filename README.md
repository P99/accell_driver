# Linux kernel driver for accelerometer LIS3DH

## Hardware

We are running on [BeagleBoneBlack](https://beagleboard.org/black) hardware (ARM architecture)
The accelerometer chip is ST Microelectronics [LIS3DH](http://www.st.com/content/ccc/resource/technical/document/datasheet/3c/ae/50/85/d6/b1/46/fe/CD00274221.pdf/files/CD00274221.pdf/jcr:content/translations/en.CD00274221.pdf).
For convenience, we are using [Adafruit LIS3DH breakout](https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout/pinouts)
The accelerometer chip is connected through the I2C bus.

The adafruit LIS3DH is connected on the BeagleBoneBlack as follow.

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

Handy we don't need any pull up / pull down resistors or level adpatation for 3.3v to 5v

Once the wiring is done, don't take it for granted, on the board run:
```
root@beaglebone:~# i2cdetect -r 1
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-1 using read byte commands.
I will probe address range 0x03-0x77.
Continue? [Y/n] y
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- UU -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- UU UU UU UU -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
root@beaglebone:~#
```

## Design

The driver is a misc device so we don't have to care about finding a suitable "MINOR_VERSION" for the module

We are using a simple character device "lxaccell" to read acceleration values on three axis (X, Y, Z)

The driver registers an i2c driver interface. Upon successfull recongnition of the accelerometer chip, the misc device is created.

## Environment setup

The I2C bus is already up and running on the original debian image, so you don't actually need to recompile the kernel

Here is a quick hack to get you up and running:

1. On the board, install the kernel headers:
  root@beaglebone:~# sudo apt-get install linux-headers-`uname -r`

2. Then copy over all the headers: (modifiy the kernel version accordingly)
  scp -r root@bbb:/usr/src/linux-headers-3.8.13-bone70 .

3. Replace linux-headers-3.8.13-bone70/scripts by a cross-compiled version of your own
  Grab the cross-toochain on [linaro](http://releases.linaro.org/components/toolchain/binaries/4.9-2016.02/arm-linux-gnueabihf/) website

4. Then, the classic `make` (cross-compiling for ARM)
  make

## Details

### I2C autodetect
