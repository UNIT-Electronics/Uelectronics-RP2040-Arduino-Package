# Uelectronics-RP2040 Arduino Package for DualMCU Boards

The Uelectronics-RP2040 Arduino Package is a powerful set of tools and libraries designed to enable developers to fully leverage the capabilities of the RP2040 microcontroller. This package simplifies the programming and control of RP2040-based devices on DualMCU boards, allowing developers to work seamlessly within the Arduino platform.

With this package, you can explore a wide range of possibilities for creating innovative IoT solutions, developing prototypes, and tapping into the potential of connected devices. The package also includes a rich collection of example projects and sample code to help you get started quickly and efficiently.

What is Uelectronics-RP2040 Arduino Package?
The Uelectronics Arduino Core is an advanced and optimized version of the Raspberry Pi Pico Arduino Core, based on the exceptional work of Earle F. Philhower, III. This adaptation uses the Raspberry Pi Pico SDK and a custom GCC 10.3/Newlib 4.0 toolchain, aligning with the versions 2.6.4 and 4.0.1 of the original core.

---

## Key Features
Full support for the DualMCU ONE RP2040, the latest innovation in the DualMCU family by UNIT Electronics.
Compatibility with popular RP2040-based boards, including the Raspberry Pi Pico, Pico W, and other configurable designs.
Utilizes the powerful Raspberry Pi Pico SDK and a custom GCC 10.3/Newlib 4.0 toolchain for optimal performance.
Includes comprehensive examples and libraries to accelerate your development process.

This package has been specifically designed to make the most out of the DualMCU line's capabilities, including the DualMCU RP2040 boards and the Dual ONE RP2040.

##  Supported Boards
* DualMCU RP2040
* Dual ONE RP2040 (the latest addition to the DualMCU line)
* Raspberry Pi Pico
* Raspberry Pi Pico 2
* Raspberry Pi Pico W
* Generic (customizable flash memory and I/O pin configurations)

---

# Installing via Arduino Boards Manager

1. Open the Arduino IDE and go to File → Preferences.

2. In the dialog that pops up, enter the following URL in the "Additional Boards Manager URLs" field:
    ```plaintext
    https://raw.githubusercontent.com/UNIT-Electronics/Uelectronics-RP2040-Arduino-Package/main/package_Uelectronics_rp2040_index.json
    ```  

![image](https://github.com/Rabadan-uelectronics/DualMCU-RP2040/blob/main/releases/download/0.0.0/Preferences-AditionalBoardsManagerURL.png)

3. Hit OK to close the dialog.

4. Go to Tools->Boards->Board Manager in the IDE

Type "UNIT Uelectronics RP2040" in the search box and select "Add":

![image](https://github.com/Rabadan-uelectronics/DualMCU-RP2040/blob/main/releases/download/0.0.0/BoardsManager.png)

---

# Uploading Sketches

## Uploading Your First Sketch on the DualMCU

To upload your first sketch, plug the USB-C cable into the DualMCU, move the mechanical USB selector to the “A” position (see section 3.11, "Mechanical Selector for USB Communication," in the [Product Reference Manual](https://github.com/UNIT-Electronics/DualMCU/blob/main/DualMCU(Product%20Reference%20Manual).pdf) and press and hold the RP2040 reset button (PB1), labeled “RST” on the board.

![image](https://github.com/UNIT-Electronics/DualMCU/blob/main/Docs/RP2040-Reset_BUTTON.jpg)

Without releasing the RESET button, press and hold the RP2040 boot button (PB2), labeled “BOOT.”

![image](https://github.com/UNIT-Electronics/DualMCU/blob/main/Docs/RP2040-Enter_Bootloader_mode.jpg)

Afterward, release both the RST and BOOT buttons. The sketch should now transfer and begin to run.

![image](https://github.com/UNIT-Electronics/DualMCU/blob/main/Docs/RP2040-Boot_button.jpg)

## Uploading Your First Sketch on the DualMCU ONE

1. Connect the DualMCU ONE RP2040 board via a USB-C cable.
2. Press and hold the RESET (RP-RST) and BOOT buttons, as described in the previous steps for the DualMCU board.

Upload your sketch and experience the innovation!

---

## **Documentation and Examples**  
For detailed usage information, examples, and further resources, explore the following repositories:  

- [UNIT DualMCU GitHub Repository](https://github.com/UNIT-Electronics/DualMCU)  
- [UNIT DualMCU-ONE GitHub Repository](https://github.com/UNIT-Electronics/DualMCU-ONE)  

The included examples provide practical insights and real-world applications to kickstart your development journey.

---

# Contributing
If you want to contribute or have bugfixes open an issue/PR here.

---

# Licensing and Credits
* The [Arduino IDE and ArduinoCore-API](https://arduino.cc) are developed and maintained by the Arduino team. The IDE is licensed under GPL.
* The [RP2040 GCC-based toolchain](https://github.com/earlephilhower/pico-quick-toolchain) is licensed under under the GPL.
* The [Pico-SDK](https://github.com/raspberrypi/pico-sdk) is by Raspberry Pi (Trading) Ltd and licensed under the BSD 3-Clause license.
* [Arduino-Pico](https://github.com/earlephilhower/arduino-pico) core files are licensed under the LGPL.
* [LittleFS](https://github.com/ARMmbed/littlefs) library written by ARM Limited and released under the [BSD 3-clause license](https://github.com/ARMmbed/littlefs/blob/master/LICENSE.md).
* [UF2CONV.PY](https://github.com/microsoft/uf2) is by Microsoft Corporation and licensed under the MIT license.
* Networking and filesystem code taken from the [ESP8266 Arduino Core](https://github.com/esp8266/Arduino) and licensed under the LGPL.
* DHCP server for AP host mode from the [Micropython Project](https://micropython.org), distributed under the MIT License.
* [FreeRTOS](https://freertos.org) is Copyright Amazon.com, Inc. or its affiliates, and distributed under the MIT license.
* [lwIP](https://savannah.nongnu.org/projects/lwip/) is (c) the Swedish Institute of Computer Science and licenced under the BSD license.
* [BearSSL](https://bearssl.org) library written by Thomas Pornin, is distributed under the [MIT License](https://bearssl.org/#legal-details).
* [UZLib](https://github.com/pfalcon/uzlib) is copyright (c) 2003 Joergen Ibsen and distributed under the zlib license.
* [LEAmDNS](https://github.com/LaborEtArs/ESP8266mDNS) is copyright multiple authors and distributed under the MIT license.
* [http-parser](https://github.com/nodejs/http-parser) is copyright Joyent, Inc. and other Node contributors.
* WebServer code modified from the [ESP32 WebServer](https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer) and is copyright (c) 2015 Ivan Grokhotkov and others
* The [Raspberry Pi Pico Arduino Core](https://github.com/earlephilhower/arduino-pico) of Earle F. Philhower, III (earlephilhower).


