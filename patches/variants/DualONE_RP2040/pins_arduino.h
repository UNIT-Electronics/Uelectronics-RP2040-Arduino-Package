#pragma once

// ============================================================
// UNIT Electronics - DualMCU ONE
// RP2040
// Arduino UNO-style pin mapping
// ============================================================


// ============================================================
// Arduino Digital Pin Mapping
// ============================================================

// UART / Digital
#define __PIN_D0   (1u)    // RX  - GPIO1  - UART0 RX
#define __PIN_D1   (0u)    // TX  - GPIO0  - UART0 TX
#define __PIN_D2   (5u)    // GPIO5
#define __PIN_D3   (4u)    // GPIO4
#define __PIN_D4   (9u)    // GPIO9
#define __PIN_D5   (11u)   // GPIO11
#define __PIN_D6   (8u)    // GPIO8
#define __PIN_D7   (10u)   // GPIO10

// Standard Arduino SPI area
#define __PIN_D8   (2u)    // GPIO2
#define __PIN_D9   (3u)    // GPIO3
#define __PIN_D10  (17u)   // GPIO17 - SPI0 CS
#define __PIN_D11  (19u)   // GPIO19 - SPI0 MOSI / TX
#define __PIN_D12  (16u)   // GPIO16 - SPI0 MISO / RX
#define __PIN_D13  (18u)   // GPIO18 - SPI0 SCK

// Analog header
#define __PIN_D14  (26u)   // A0 - GPIO26
#define __PIN_D15  (27u)   // A1 - GPIO27
#define __PIN_D16  (28u)   // A2 - GPIO28
#define __PIN_D17  (29u)   // A3 - GPIO29

// Additional digital / I2C1
#define __PIN_D18  (22u)   // GPIO22 - I2C1 SDA
#define __PIN_D19  (23u)   // GPIO23 - I2C1 SCL

// On-board peripherals
#define __PIN_D28  (24u)   // GPIO24 - NeoPixel
#define __PIN_D29  (25u)   // GPIO25 - Built-in LED


// ============================================================
// Analog Pins
// ============================================================

#define __PIN_A0   (26u)
#define __PIN_A1   (27u)
#define __PIN_A2   (28u)
#define __PIN_A3   (29u)


// ============================================================
// UART
// ============================================================

// Serial1 - UART0
// D1/TX = GPIO0
// D0/RX = GPIO1

#define __SERIAL1_DEVICE uart0

#define PIN_SERIAL1_TX   (0u)
#define PIN_SERIAL1_RX   (1u)


// Serial2 - UART1
// D3 = GPIO4 = UART1 TX
// D2 = GPIO5 = UART1 RX

#define __SERIAL2_DEVICE uart1

#define PIN_SERIAL2_TX   (4u)
#define PIN_SERIAL2_RX   (5u)

#define SERIAL_HOWMANY   (2u)


// ============================================================
// SPI
// ============================================================

// SPI - Primary Arduino SPI bus
//
// D10 = GPIO17 = SPI0 CS
// D11 = GPIO19 = SPI0 MOSI / TX
// D12 = GPIO16 = SPI0 MISO / RX
// D13 = GPIO18 = SPI0 SCK

#define __SPI0_DEVICE    spi0

#define PIN_SPI0_MISO    (16u)
#define PIN_SPI0_MOSI    (19u)
#define PIN_SPI0_SCK     (18u)
#define PIN_SPI0_SS      (17u)


// SPI1 - Secondary SPI bus
//
// A0 / D14 = GPIO26 = SPI1 SCK
// A1 / D15 = GPIO27 = SPI1 MOSI / TX
// A2 / D16 = GPIO28 = SPI1 MISO / RX
// A3 / D17 = GPIO29 = SPI1 CS

#define __SPI1_DEVICE    spi1

#define PIN_SPI1_MISO    (28u)
#define PIN_SPI1_MOSI    (27u)
#define PIN_SPI1_SCK     (26u)
#define PIN_SPI1_SS      (29u)

#define SPI_HOWMANY      (2u)


// ============================================================
// I2C / Wire
// ============================================================

// Wire - Primary Arduino I2C bus
//
// Dedicated SDA/SCL pins on the Arduino-style header:
//
// SDA = GPIO20 = I2C0 SDA
// SCL = GPIO21 = I2C0 SCL
//
// GPIO4/GPIO5 (D3/D2) are also valid alternate I2C0 pins,
// but they are not used as the default Wire pins.

#define __WIRE0_DEVICE   i2c0

#define PIN_WIRE0_SDA    (20u)
#define PIN_WIRE0_SCL    (21u)


// Wire1 - Secondary I2C bus
//
// D18 = GPIO22 = I2C1 SDA
// D19 = GPIO23 = I2C1 SCL

#define __WIRE1_DEVICE   i2c1

#define PIN_WIRE1_SDA    (22u)
#define PIN_WIRE1_SCL    (23u)

#define WIRE_HOWMANY     (2u)


// ============================================================
// On-board LED
// ============================================================

// GPIO25 / Arduino D29

#define PIN_LED          (25u)
#define LED_BUILTIN      PIN_LED


// ============================================================
// On-board NeoPixel
// ============================================================

// GPIO24 / Arduino D28

#define PIN_NEOPIXEL     (24u)
#define NUM_NEOPIXEL     (1u)


// ============================================================
// Arduino-Pico common definitions
// ============================================================

#include "../generic/common.h"