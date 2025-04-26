// CECS 447: Project 4 - Weather Forecast
// File Name: Switch.h
// Purpose: This program initializes PORTF switches 1 and 2.
// Group: Robby Rimal 
// 				Jonathan Cerniaz 
// 				Joseph Guzman
// 				Afzal Hakim
// Last Modified: 10/14/2024


#ifndef SWITCH_H
#define SWITCH_H

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define SW1 0x10
#define SW2 0x01
#define NVIC_EN0_PORTF 0x40000000

// Initialize Switch 1 and 2
void Switch_Init(void);

#endif