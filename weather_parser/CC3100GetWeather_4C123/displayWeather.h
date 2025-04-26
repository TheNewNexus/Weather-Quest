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

#define MAX_LEN 20

#include <stdint.h>
#include <stdlib.h>
#include "SysTick.h"

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

extern WeatherCondition currentWeather;


// Function prototypes
void UpdateWeather(char City_name[MAX_LEN], char Current_Temperature[MAX_LEN], char min_Temperature[MAX_LEN], char max_Temperature[MAX_LEN], char current_Humidity[MAX_LEN], char weather_condition[MAX_LEN]);
void WeatherIcon_Draw1(WeatherCondition icon, int32_t x, int32_t y);
void WeatherIcon_Draw2(WeatherCondition icon, int32_t x, int32_t y);



#endif // WEATHERICONS_H