
#include <stdint.h>
#include "systick_stm32f103.h"
#include "clocks_stm32f103.h"

#define SYSTICK_BASE 0xE000E010

volatile uint32_t totalticks = 0;

volatile unsigned int* STK_CTRL = (unsigned int *)(SYSTICK_BASE + 0x00);
volatile unsigned int* STK_LOAD = (unsigned int *)(SYSTICK_BASE + 0x04);
volatile unsigned int* STK_VAL = (unsigned int *)(SYSTICK_BASE + 0x08);

uint32_t calculate_onems_cycles(uint32_t ahbfreq)
{
    uint32_t cycles = ahbfreq / 1000;
    return (cycles); // Period of N processor cycles = N-1 reload value
}

void SysTick_Handler(void) //Should override weak def in startup.c
{
    totalticks++;
}

void systick_init(const uint32_t periodms) // Creates a constant, immutable ticker
{
    static bool wasexecuted = 0; // Initialized only once
    if (!wasexecuted)
    {
    systick_stop();
    uint32_t reloadval = periodms * calculate_onems_cycles(get_hclock_frequency()) - 1; //Period of N cycles = N - 1 reload
    if (reloadval < 0x00FFFFFF && reloadval > 0x00000001) // Min/Max reload values
    {
    *STK_LOAD = reloadval; // Set Reload Register
    }
    
    *STK_VAL = 0;

    *STK_CTRL |= (1 << 2); // Select AHBCLK, not AHB / 8
    *STK_CTRL |= (1 << 1); // Enable SysTick Interrupt (TICKINT)

    systick_start();
    wasexecuted = 1;
    }
}

uint32_t get_tick_periodms(void)
{
    uint32_t cycles = *STK_LOAD;
    return ((cycles + 1) / calculate_onems_cycles(get_hclock_frequency()));
}

void systick_start(void)
{
    *STK_CTRL |= (0b1 << 0);
}
void systick_stop(void)
{
    *STK_CTRL &= ~(0b1 << 0);
}

void delay_ticks(uint32_t ticks) //Not Subtick Accurate!
{
    uint32_t ticks1 = totalticks;
    while (totalticks < ticks1 + ticks);
}

void delay_ms(uint32_t miliseconds)
{
    //Not sure
}

void delay_s(uint32_t seconds)
{
    delay_ms(seconds * 1000);
}

uint32_t get_system_time_ticks(void)
{
    return totalticks;
}

uint32_t get_system_time_ms(void)
{
    return totalticks * get_tick_periodms();
}