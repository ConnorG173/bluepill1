
#include <stdint.h>
#include "systick_stm32f103.h"
#include "clocks_stm32f103.h"

//Helpers

const uint32_t tickperiodms = 1;
volatile uint32_t totalticks = 0;

#define SYSTICK_BASE 0xE000E010
#define STK_CTRL (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define STK_LOAD (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define STK_VAL  (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

uint32_t calculate_onems_cycles(uint32_t ahbfreq)
{
    uint32_t cycles = ahbfreq / 1000;
    return (cycles);
}
void SysTick_Handler(void) //Should override weak def in startup.c
{
    totalticks++;
}

//API

bool systick_init(void) //more than 1 call is undefined
{
    systick_stop();
    uint32_t reloadval = tickperiodms * calculate_onems_cycles(get_hclock_frequency()) - 1; //Period of N cycles = N - 1 reload

    if (reloadval > 0x00FFFFFFUL || reloadval == 0x0UL) { return 0; } // reloadval outside range 1->2^24
    STK_LOAD = reloadval; // Set Reload Register

    STK_VAL = 0;

    STK_CTRL |= (1 << 2); // Select AHBCLK, not AHB / 8
    STK_CTRL |= (1 << 1); // Enable SysTick Interrupt (TICKINT)

    systick_start();
    return 1;
}

void systick_start(void)
{
    STK_CTRL |= (0b1 << 0);
}
void systick_stop(void)
{
    STK_CTRL &= ~(0b1 << 0);
}

uint32_t systime_ticks_now(void)
{
    return totalticks;
}
uint32_t systime_ticks_elapsed(uint32_t since)
{
    return totalticks - since;
}
bool systime_ticks_expired(uint32_t start, uint32_t duration)
{
    return (systime_ticks_elapsed(start) >= duration);
}
void delay_ticks(uint32_t duration_ticks) //Not Subtick Accurate!
{
    uint32_t start = systime_ticks_now();
    while (!systime_ticks_expired(start, duration_ticks)) {
        __asm__("NOP");
    }
}

uint64_t ticks_to_ms(uint32_t ticks)
{
    return ticks * tickperiodms; //rounded to nearest tick - quantization
}

uint32_t ms_to_ticks(uint64_t ms) // wraps at 2^32 ticks
{
    uint64_t ticks = ms / tickperiodms;
    return (uint32_t)ticks;
}
