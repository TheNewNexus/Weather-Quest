// CECS 447: Project 4 - Weather Forecast
// File Name: ST7735Walk_man.c
// Purpose: Initializes a display, draws a scene, and animates a walking stick figure.
// Group: Robby Rimal 
// 				Jonathan Cerniaz 
// 				Joseph Guzman
// 				Afzal Hakim
// Last Modified: 10/14/2024

// Runs on TM4C123
// Uses ST7735.c LCD.
// Jonathan Valvano
// August 5, 2015

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
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "walking.h"
#include "tm4c123gh6pm.h"

void DelayWait10ms(uint32_t n);

int main(void){
	uint32_t x = 40, ht = 0;	
	int32_t l = 15, h = 15;
	
//  PLL_Init(12);
  PLL_Init(Bus80MHz);
	
  ST7735_InitR(INITR_REDTAB);
	
	// DRAW THE GROUND
	ST7735_FillScreen(ST7735_CYAN);
	ST7735_FillRect(0, 150,128, 30, ST7735_GREEN);
	  
	// DRAW THE SUN
	ST7735_FillCircle(100, 30, 15,ST7735_YELLOW);
	
	// test some driver functions
	ST7735_DrawChar(0,0,'A',ST7735_RED,ST7735_GREEN, 3);
	
  while(1){
		// DRAW THE MAN
		ST7735_XYPlotMan( x, l, h, ST7735_RED);		
		DelayWait10ms(10);
			
		// REMOVE THE MAN
		ST7735_XYPlotMan( x, l, h, ST7735_CYAN);
	 
		// MOVE THE COORDINATES OF THE MAN
		x++;
	 
		// CODE for MOVING the LEG
		l--;
		if (l == -15)
				l = 15;
	 
		// CODE for MOVING the HAND    
		if (ht == 1)
				h++;
		else
				h--;
	 
		if (h == 15)
			 ht = 0;
		else if (h == -15)   
			ht = 1;
	 
		//Reset parameters
		if(x>170){
			x = 40, ht = 0,l = 15, h = 15;
		}
	}
} 


// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
//    time = 727240*2/91;  // 10msec for launchpad
    time = 727240/91;  // for simulation
    while(time){
	  	time--;
    }
    n--;
  }
}
