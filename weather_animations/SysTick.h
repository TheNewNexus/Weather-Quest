// CECS 447: Project 4 - Weather Forecast
// File Name: SysTick.h
// Purpose: This program initializes SysTick with busy wait running at bus clock
// Group: Robby Rimal 
// 				Jonathan Cerniaz 
// 				Joseph Guzman
// 				Afzal Hakim
// Last Modified: 10/14/2024

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include "tm4c123gh6pm.h"

// Initialize SysTick with busy wait running at bus clock
void SysTick_Init(void);

// Wait for delay milliseconds
void SysTick_Wait(uint32_t delay);

// Delay for delay milliseconds
void Delay(uint32_t time_ms);

#endif
