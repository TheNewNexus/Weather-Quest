// CECS 447: Project 4 - Weather Forecast
// File Name: WeatherIcons.h
// Purpose: Header file for weather icon definitions and functions
// Group: Robby Rimal 
// 				Jonathan Cerniaz 
// 				Joseph Guzman
// 				Afzal Hakim
// Last Modified: 10/14/2024

// For use with TM4C123GXL and ST7735 LCD

#ifndef WEATHERICONS_H
#define WEATHERICONS_H

#include <stdint.h>

extern const uint16_t sun1[];
extern const uint16_t rainy[];
extern const uint16_t sun[];
extern const uint16_t sun2[];
extern const uint16_t cloud2[];
extern const uint16_t blackSquare[];
extern const uint16_t cloudy[];
extern const uint16_t rain2[];
extern const uint16_t rain1[];
extern const uint16_t cloud1[];





// Define weather icon types
typedef enum {
    WEATHER_CLEAR,
    WEATHER_CLOUDY,
    WEATHER_RAINY
} WeatherCondition;

// Function prototypes
void WeatherIcon_Init(void);
void WeatherIcon_Draw1(WeatherCondition icon, int32_t x, int32_t y, uint16_t color);
void WeatherIcon_Draw2(WeatherCondition icon, int32_t x, int32_t y, uint16_t color);


#endif // WEATHERICONS_H