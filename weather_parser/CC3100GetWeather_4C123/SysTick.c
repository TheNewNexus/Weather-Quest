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
#include "../inc/tm4c123gh6pm.h"
#include "SysTick.h"
#include <stdbool.h>

volatile uint32_t systick_count = 0;


//volatile bool updateIcon = false;
void SysTick_Init(void) {
    NVIC_ST_CTRL_R = 0; // Disable SysTick during setup
    NVIC_ST_RELOAD_R = ONE_MS_RELOAD - 1; // Set reload for 1ms
    NVIC_ST_CURRENT_R = 0; // Clear current value
    
		//NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_INTEN + NVIC_ST_CTRL_ENABLE; // Enable SysTick with core clock
		NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_ENABLE; // Enable SysTick with core clock
		NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x1FFFFFFF) | 0x40000000; // Set priority
		//NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // Set priority to 2
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
				 // Increment the systick_count
        systick_count++;
    }
}

uint32_t Get_Random_Seed(void) {
    return (systick_count ^ (NVIC_ST_CURRENT_R & 0xFFFF));
}

void Delay_1ms(uint32_t time_ms)
{
    // Call SysTick_Wait for the given number of milliseconds
    SysTick_Wait(time_ms);
}