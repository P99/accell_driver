# Linux kernel driver for accelerometer chip LIS3DH

We are running on BeagleBoardBlack hardware (ARM architecture)

The accelerometer chip is connected through the I2C bus

# Design

The driver is a misc device so we don't have to care about finding a suitable "MINOR_VERSION" for the module

We are using a simple character device to read acceleration values on three axis (X, Y, Z)

# Usage

Makefile contains a absolute path to the kernel headers, it should be modufied accordingly

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-