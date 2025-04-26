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
#include <stdbool.h>

#define SYSCLK 50000000               // 80 MHz system clock
#define ONE_MS_RELOAD (SYSCLK / 1000) // Reload value for 1 millisecond

extern volatile uint32_t systick_count;

// Initialize SysTick with busy wait running at bus clock
void SysTick_Init(void);

// Wait for delay milliseconds
void SysTick_Wait(uint32_t delay);

uint32_t Get_Random_Seed(void);

// Delay for delay milliseconds
void Delay_1ms(uint32_t time_ms);

void SysTick_Handler(void);

#endif
