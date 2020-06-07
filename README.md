
# epd-driver for [2.7"] display

[![Build_Status](https://travis-ci.com/sohal/epd-driver.svg?branch=master)](https://travis-ci.com/sohal/epd-driver)

The E-Paper display driver written in C from [Embedded Artists][1] is modified to be used with [Colibri VF50][2] module on [Viola V1.1][5] from Toradex&reg;.

_Works on Linux_

## Requirement on the Embedded Linux board

### Connection to EPD 14 Pin connector

| Viola Pins   | Pins   | Description   | Description   | Pin   | Viola Pins   |
| :----------: | -----: | :------------ | ------------: | :---- | :----------: |
| 7  | 1  | GND         |  3v3      | 2  | 4  |
| 22 | 3  | SLCK        | MOSI      | 4  | 20 |
| 21 | 5  | MISO        | SSEL      | 6  | 23 |
| 34 | 7  | BUSY        | BORDER    | 8  | 37 |
|*NC*| 9  | SCL         | SDA       | 10 |*NC*|
|*NC*| 11 | PWM         | RESET     | 12 | 39 |
| 36 | 13 | PANEL ON    | DISCHARGE | 14 | 38 |

*NC Not connected to Viola Board*

### Firmware Changes

The following changes were required to be made for the E-Paper display interface.

   5 GPIO were required for the interface. The SPI was already in the default dtb configuration. Since the resources needed the modifications, a patch is created for additional reference.   For this implementation UART1 and UART2 is scavanged. While the CAN1 is introduced for need of application not dependent on E-Paper Display.

## Project management

The project is editied using visual studio code with following plugins:

- C/C++ extension from Microsoft&trade;.
- CMake language support plugin from [twxs][6].
- CMake language support plugin from [vector-of-bool][7].

The compiler tools are
 
- ARM compiler toolchain for Toradex boards as described in [this][3] page.

## Targets

There are 3 project targets:

### epd_fuse

This generates the fuse library enabled part that will be later used as a /dev/epd device using [FUSE] as discussed in the original [Embedded Artists][1] repository.

- The board had to be updated with the necessary [FUSE] library using "opkg install fuse". 
- The cross compilation needs definition of *_FILE_OFFSET_BITS=64* during linking [FUSE] library.

### epd_test

The test program that is already part of Embedded Artists code and toggles the various images from the "image" folder after 5 seconds. The /dev/epd need not be installed for this to work.

### epd_app

The application program which is specific. The intention is to update the connected interface and it's ethernet IP on the display every minute using cron. Thus whenever the device is connected to network, the IP always is available to ssh in the device.

The fonts required are used from [openglcd][4].

### epd_netid(optional)

This is a target application that only tests if the IP search is working and buffer handling is OK.

[1]: https://github.com/embeddedartists/gratis
[2]: https://www.toradex.com/computer-on-modules/colibri-arm-family/nxp-freescale-vybrid-vf5xx
[3]: https://developer.toradex.com/knowledge-base/build-u-boot-and-linux-kernel-from-source-code
[4]: https://bitbucket.org/bperrybap/openglcd/wiki/Home
[5]: https://www.toradex.com/products/carrier-board/viola-carrier-board
[6]: https://github.com/twxs/gitflow
[7]: https://github.com/vector-of-bool/vscode-cmake-tools
[FUSE]: https://en.wikipedia.org/wiki/Filesystem_in_Userspace
[2.7"]: https://www.embeddedartists.com/products/displays/lcd_27_epaper.php

*The product names mentioned here may be trade marks of the respective brands.*
