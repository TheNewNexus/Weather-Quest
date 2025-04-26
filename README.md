# Project : Weather Quest

This project implements a weather animation display using a TM4C123 microcontroller and an ST7735 LCD screen. It cycles through different weather conditions and displays animated weather icons.

## Features

- Displays three weather conditions: Clear, Cloudy, and Rainy
- Animates weather icons using two-frame animations
- Cycles through weather conditions using a button press
- Uses ST7735 LCD for display output

## Hardware Requirements

- TM4C123 microcontroller
- ST7735 LCD screen
- Push button (connected to SW1)

## Pin Connections

- Backlight (pin 10) connected to +3.3 V
- MISO (pin 9) unconnected
- SCK (pin 8) connected to PA2 (SSI0Clk)
- MOSI (pin 7) connected to PA5 (SSI0Tx)
- TFT_CS (pin 6) connected to PA3 (SSI0Fss)
- CARD_CS (pin 5) unconnected
- Data/Command (pin 4) connected to PA6 (GPIO)
- RESET (pin 3) connected to PA7 (GPIO)
- VCC (pin 2) connected to +3.3 V
- Gnd (pin 1) connected to ground

## Software Dependencies

- PLL library
- SysTick library
- ST7735 library
- WeatherIcons library

## Usage

1. Initialize the system by calling the necessary setup functions in `main()`.
2. The display will show the first weather condition (Clear).
3. Press SW1 to cycle through the weather conditions.
4. The weather icon will animate between two frames for each condition.

## Files

- `WeatherAnimations.c`: Main program file
- `WeatherIcons.h`: Header file for weather icon definitions
- `WeatherIcons.c`: Implementation of weather icon functions

## Building and Flashing

Use Keil uVision or your preferred ARM development environment to build and flash the project to your TM4C123 microcontroller.
