// CECS 447: Project 4 - Weather Forecast
// File Name: Switch.c
// Purpose: This program initializes PORTF switches 1 and 2.
// Group: Robby Rimal 
// 				Jonathan Cerniaz 
// 				Joseph Guzman
// 				Afzal Hakim
// Last Modified: 10/14/2024

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Switch.h"


//Initialize Switch 1 and 2 with interrupts
//Initialization for SW1 (PF4)
void Switch_Init(void) {
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;        // (a) Activate clock for Port F
    while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOF) == 0) {} // Wait for clock to be ready

    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;           // Unlock PortF PF0 (if needed, but not used here)
    GPIO_PORTF_CR_R |= 0x10;                     // Allow changes to PF4
    GPIO_PORTF_DIR_R &= ~0x10;                   // Configure PF4 as input (SW1)
    GPIO_PORTF_AFSEL_R &= ~0x10;                 // Disable alternate function on PF4
    GPIO_PORTF_DEN_R |= 0x10;                    // Enable digital I/O on PF4
    GPIO_PORTF_PCTL_R &= ~0x000F0000;            // Configure PF4 as GPIO
    GPIO_PORTF_AMSEL_R &= ~0x10;                 // Disable analog functionality on PF4
    GPIO_PORTF_PUR_R |= 0x10;                    // Enable weak pull-up on PF4 (SW1)

    GPIO_PORTF_IS_R &= ~0x10;                    // Configure PF4 for edge-sensitive interrupts
    GPIO_PORTF_IBE_R &= ~0x10;                   // PF4 is not both edges triggered
    GPIO_PORTF_IEV_R &= ~0x10;                   // PF4 interrupt on falling edge event
    GPIO_PORTF_ICR_R = 0x10;                     // Clear interrupt flags for PF4
    GPIO_PORTF_IM_R |= 0x10;                     // Arm interrupt on PF4

    NVIC_PRI7_R = (NVIC_PRI7_R & ~0x00E00000) | 0x00A00000; // Set interrupt priority 5 for Port F
    NVIC_EN0_R |= NVIC_EN0_PORTF;                // Enable interrupt 30 in NVIC (Port F)
}
