// CECS 447: Project 4 - Weather Forecast
// File Name: WeatherAnimations.c
// Purpose: This program displays weather animations on the ST7735 LCD.
// Group: Robby Rimal 
// 				Jonathan Cerniaz 
// 				Joseph Guzman
// 				Afzal Hakim
// Last Modified: 10/14/2024


// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "Switch.h"
#include "ST7735.h"
#include "PLL.h"
#include "WeatherIcons.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"

char weatherCondition[20];
int weatherIndex = 0;
int weatherWheelSize = 2; // 0, 1, 2 for three weather conditions
int frame = 0;

WeatherCondition currentWeather; 

bool isSwitchPressed = false;



void UpdateWeather(void);
void DisableInterrupts(void);
void EnableInterrupts(void);
void WaitForInterrupt(void);


int main(void) {
    DisableInterrupts();
    PLL_Init(Bus80MHz);
    SysTick_Init();
    Switch_Init();
    //ST7735_InitR(INITR_GREENTAB);
		ST7735_InitR(INITR_REDTAB);
		//ST7735_InitR(INITR_BLACKTAB);
		//ST7735_InitB();
    EnableInterrupts();

    UpdateWeather(); // Initialize with the first weather condition

    while(1) {
        for (int i = 0; i < 100; i++) {
            if (frame % 2 == 0)
                WeatherIcon_Draw1(currentWeather, 36, 48, ST7735_WHITE);
            else
                WeatherIcon_Draw2(currentWeather, 36, 48, ST7735_WHITE);
            frame++;
            Delay(125);  //125 ms delay

            // Check for switch press and update immediately if pressed
            if (isSwitchPressed) {
                weatherIndex = (weatherIndex + 1) % (weatherWheelSize + 1);
                UpdateWeather();
								Delay(50);
                isSwitchPressed = false;
                break; // Exit the animation loop to start new weather immediately
            }
        }
    }
}

void UpdateWeather(void) {
    
    switch(weatherIndex) {
        case 0:
						ST7735_FillScreen(ST7735_SKYBLUE);
            strcpy(weatherCondition, "Clear");
						ST7735_SetFont(Font_Bold);
						ST7735_DrawString(0, 0, "New York", ST7735_YELLOW);
						ST7735_SetFont(Font_Italic);
            ST7735_DrawString(0,1,"Clear, 77F, 60%", ST7735_YELLOW);
            currentWeather = WEATHER_CLEAR;
            break;
        case 1:
						ST7735_FillScreen(ST7735_LIGHTGREY);
            strcpy(weatherCondition, "Cloudy");
						ST7735_SetFont(Font_Thin);
						ST7735_DrawString(0, 0, "London", ST7735_WHITE);
						ST7735_SetFont(Font);
						ST7735_DrawString(0,1,"Cloudy, 64F, 75%", ST7735_WHITE);
            currentWeather = WEATHER_CLOUDY;
            break;
        case 2:
						ST7735_FillScreen(ST7735_DARKGREY);
            strcpy(weatherCondition, "Rainy");
						ST7735_SetFont(Font_Italic);
						ST7735_DrawString(0,0,"Seattle", ST7735_CYAN);
						ST7735_SetFont(Font_Thin);
						ST7735_DrawString(0,1,"Rainy, 59F, 90%", ST7735_CYAN);
						currentWeather = WEATHER_RAINY;
            break;
    }
    frame = 0; // Reset frame counter for smooth animation start
}


void GPIOPortF_Handler(void) {
    Delay(50); // Simple debounce
    if (GPIO_PORTF_RIS_R & SW1) {
        isSwitchPressed = true;
        GPIO_PORTF_ICR_R |= SW1;
    }
}
