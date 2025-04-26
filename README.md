# Project : Weather Quest

This project implements a weather animation display using a TM4C123 microcontroller, an ST7735 LCD screen, and a WiFi Booster Pack. It fetches real-time weather data from the OpenWeather API via HTTP, parses the JSON response (temperature and humidity), and displays both animated weather icons and weather data on the LCD and UART terminal.

## Features

- Fetches live weather data (temperature, humidity, conditions) from OpenWeather API via WiFi
- Displays three weather conditions: Clear, Cloudy, and Rainy
- Animates weather icons using two-frame animations
- Cycles through weather conditions using a button press
- Displays parsed weather data on ST7735 LCD and UART terminal
- Uses ST7735 LCD for graphical output

## Hardware Requirements

- TM4C123 microcontroller (Tiva C Series LaunchPad)
- ST7735 LCD screen
- CC3100/CC3200 WiFi Booster Pack (or compatible WiFi module)
- Push button (connected to SW1)

## Pin Connections

### LCD (ST7735)
- Backlight (pin 10) → +3.3V
- MISO (pin 9) → unconnected
- SCK (pin 8) → PA2 (SSI0Clk)
- MOSI (pin 7) → PA5 (SSI0Tx)
- TFT_CS (pin 6) → PA3 (SSI0Fss)
- CARD_CS (pin 5) → unconnected
- Data/Command (pin 4) → PA6 (GPIO)
- RESET (pin 3) → PA7 (GPIO)
- VCC (pin 2) → +3.3V
- GND (pin 1) → Ground

### WiFi Booster Pack (CC3100/CC3200)
- Connect as per [TI BoosterPack Pinout](http://www.ti.com/lit/ug/swru372b/swru372b.pdf)
- Typically uses SPI interface (SSI2 on TM4C123)
- Power: +3.3V and GND
- IRQ, CS, and EN lines connected to appropriate GPIO pins (see your WiFi library documentation)

## Software Dependencies

- PLL library
- SysTick library
- ST7735 library
- WeatherIcons library
- WiFi driver/library (e.g., Texas Instruments CC3100/CC3200 SimpleLink)
- JSON parsing library (if not included in WiFi stack)
- OpenWeather API key

## Usage

1. **Configure WiFi**
   - Set your WiFi SSID and password in the code (usually in a `wifi_config.h` or similar file).
   - Enter your OpenWeather API key in the source file.

2. **Initialize the System**
   - Call the necessary setup functions in `main()` to initialize the LCD, WiFi module, and peripherals.

3. **Fetch and Display Weather**
   - On startup, the system connects to WiFi and fetches weather data from the OpenWeather API.
   - The display shows the current weather condition, temperature, and humidity.
   - Press SW1 to cycle through weather animations (Clear, Cloudy, Rainy).
   - Weather icons animate between two frames for each condition.
   - Weather data is also output to the UART terminal for debugging or logging.

## Files

### For Weather Animations
- `WeatherAnimations.c`: Main program file
- `WeatherIcons.h`: Header file for weather icon definitions
- `WeatherIcons.c`: Implementation of weather icon functions

### For Weather Quest
- `getWeather.c`: WiFi initialization and HTTP request functions and functions for parsing JSON responses
- `ST7735.c` : ST7735 Color LCD configuration
- `displayWeather.c` : Display parsed and formatted JSON responses on Color LCD

## Building and Flashing

Use Keil uVision or your preferred ARM development environment to build and flash the project to your TM4C123 microcontroller.

## OpenWeather API Setup

1. Register for a free API key at [OpenWeather](https://openweathermap.org/api).
2. Replace the placeholder API key in your source code with your actual key.
3. Ensure your WiFi network allows outbound HTTP requests.

## Troubleshooting

- **WiFi Connection Issues:** Double-check SSID, password, and power supply to the Booster Pack.
- **API Errors:** Ensure your API key is valid and not rate-limited.
- **Display Issues:** Verify LCD wiring and initialization sequence.

## References

- [TM4C123GXL LaunchPad Documentation](https://www.ti.com/tool/EK-TM4C123GXL)
- [CC3100/CC3200 WiFi BoosterPack](https://www.ti.com/tool/CC3100BOOST)
- [OpenWeather API Documentation](https://openweathermap.org/api)
- [ST7735 LCD Library](https://github.com/adafruit/Adafruit-ST7735-Library)

---

