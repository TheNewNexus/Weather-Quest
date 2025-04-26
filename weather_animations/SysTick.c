// CECS 447: Project 4 - Weather Forecast
// File Name: SysTick.c
// Purpose: This program initializes SysTick with busy wait running at bus clock
// Group: Robby Rimal 
// 				Jonathan Cerniaz 
// 				Joseph Guzman
// 				Afzal Hakim
// Last Modified: 10/14/2024

//  This program runs a simple SysTick init
//  and delay function
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"

// #define SYSCLK 16000000               // 16 MHz system clock
#define SYSCLK 80000000               // 80 MHz system clock
#define ONE_MS_RELOAD (SYSCLK / 1000) // Reload value for 1 millisecond

void SysTick_Init(void)
{
    // Disable SysTick during setup
    NVIC_ST_CTRL_R = 0;
    // Set the reload register to its maximum value
    NVIC_ST_RELOAD_R = ONE_MS_RELOAD - 1; // For 1ms delay
    // Reset the current value register
    NVIC_ST_CURRENT_R = 0;
    // Enable SysTick with core clock
    NVIC_ST_CTRL_R = 0x05; // Enable SysTick with system clock, no interrupts
}

void SysTick_Wait(uint32_t delay)
{
    // Wait for the delay in milliseconds
    while (delay > 0)
    {
        // Load the reload value for 1ms
        NVIC_ST_RELOAD_R = ONE_MS_RELOAD - 1;
        // Clear the current register
        NVIC_ST_CURRENT_R = 0;
        // Wait until the COUNT flag is set
        while ((NVIC_ST_CTRL_R & 0x00010000) == 0)
        {
        }
        // Decrement the delay
        delay--;
    }
}

void Delay(uint32_t time_ms)
{
    // Call SysTick_Wait for the given number of milliseconds
    SysTick_Wait(time_ms);
}