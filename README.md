# Linux kernel driver for LIS3DH accelerometer

*The driver should showcase I2C communications and blocking vs Async IO*

## Hardware

We are running on [BeagleBoneBlack](https://beagleboard.org/black) hardware (ARM architecture)
The accelerometer chip is ST Microelectronics [LIS3DH](http://www.st.com/content/ccc/resource/technical/document/datasheet/3c/ae/50/85/d6/b1/46/fe/CD00274221.pdf/files/CD00274221.pdf/jcr:content/translations/en.CD00274221.pdf).
For convenience, we are using [Adafruit LIS3DH breakout](https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout/pinouts)
The accelerometer chip is connected through the I2C-2 bus.

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

Handy we don't need any pull up / down resistors or level adpatation for 3.3v to 5v

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

## Environment setup

The I2C bus is already up and running on the original debian image, so you don't actually need to recompile the kernel

Here is a quick hack to get you up and running:

1. On the board, install the kernel headers:
  ```root@beaglebone:~# sudo apt-get install linux-headers-\`uname -r\````

2. Then copy over all the headers: (modifiy the kernel version accordingly)
  ```scp -r root@bbb:/usr/src/linux-headers-3.8.13-bone70 .```

3. Replace linux-headers-3.8.13-bone70/scripts by a x86 version of your own
  Grab the cross-toochain on [linaro](http://releases.linaro.org/components/toolchain/binaries/4.9-2016.02/arm-linux-gnueabihf/) website

4. Then, the classic `make` (cross-compiling for ARM)
  make

5. Copy the kernel module + sample application to the target
```
scp lxdriver.ko root@bbb:/root
scp lxapp root@bbb:/root
ssh root@bbb
root@beaglebone:~# insmod lxdriver.ko
root@beaglebone:~# cat /dev/lxaccell
root@beaglebone:~# <CTRL-C>
root@beaglebone:~# ./lxapp
root@beaglebone:~# ./lxapp
Usage: ./lxapp [THRESHOLD]
 -> Threshold defaults to 100
Waiting for acceleration data
-1052,-152,8
-844,-468,228
-1032,-224,-72
-1052,-116,0
-816,-224,220
```
  

# Design

## Declaring a misc device

The driver is a misc device so we don't have to care about finding a suitable "MINOR_VERSION" for the module

We are using a simple character device "lxaccell" to read acceleration values on three axis (X, Y, Z)

The device is created automatically while inserting the kernel module. No need to mess up with `mknod`

The driver registers an i2c driver interface. Upon successfull recognition of the accelerometer chip, the misc device is created.

A sample application "lxapp" is provided for test purposes

## I2C autodetect

It doesn't matter where you wired the accelerometer device, you can use the kernel's auto-detect feature.

While registering the `i2c_driver` structure:
```c
    /* For auto-detect*/
    .class		= I2C_CLASS_HWMON,
    .detect		= driver_i2c_detect,
    .address_list	= driver_i2c_address_list /* for lis3dh 0x18, 0x19 */
```

The kernel will iterate each i2c bus, trying to find your device. Then the `.detect` callback is called.
In this callback you have to identify the device (using WHO_I_AM for instance).
Upon succefull identification, you have to fill the i2c_board_info structure
```c
strcpy(info->type, YOUR_DEVICE_NAME);
```

## Blocking vs async IO operations

You can try to issue `cat /dev/lxaccell` and see what happend.
The `cat` command keeps reading a large amount of bytes and would issues new calls till reaching EndOfFile.
Problem is: the accelerometer device is issuing new data at a specific rate (confiurable).
You can then request the same data endlessly and this is a problem because it would keep the I2C bus very busy.
Don't forget this bus is shared with other devices! Not good.
In such case, the trick is to block the calling process.

 * Entering the .read callback
 
 ** Check the STATUS register if new data is available:
 
 *** Yes ? Grab the new data immediately and return
 
 *** No ? Wait for 50ms, then grab the data

This is good but is could be a problem if a program always want to grab the data imediately, this is where Async operations becomes handy

Check out the demo app:

 * Open the device
 
 ** Set the ASYNC flag
 
 *** Call `sigwait()`
 
 **** Read the device, go back to wait next message

When a message is sent by the kernel, sigwait return and you can grab the data immediately!

