
#include "platform.h"
#include "systick_stm32f103.h"

//Helpers

const ui32 tickperiodms = 1;
volatile ui32 totalticks = 0;

#define SYSTICK_BASE 0xE000E010
#define STK_CTRL (*(volatile ui32 *)(SYSTICK_BASE + 0x00))
#define STK_LOAD (*(volatile ui32 *)(SYSTICK_BASE + 0x04))
#define STK_VAL  (*(volatile ui32 *)(SYSTICK_BASE + 0x08))

ui32 calculate_onems_cycles(ui32 ahbfreq)
{
    ui32 cycles = ahbfreq / 1000;
    return (cycles);
}
void SysTick_Handler(void) //Should override weak def in startup.c
{
    totalticks++;
}

//API

bool systick_init(void) //more than 1 call is undefined behavior
{
    systick_stop();
    ui32 reloadval = tickperiodms * calculate_onems_cycles(get_hclock_frequency()) - 1; //Period of N cycles = N - 1 reload

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

ui32 systime_ticks_now(void)
{
    return totalticks;
}
ui32 systime_ticks_elapsed(ui32 since)
{
    return totalticks - since;
}
bool systime_ticks_expired(ui32 start, ui32 duration)
{
    return (systime_ticks_elapsed(start) >= duration);
}
void delay_ticks(ui32 duration_ticks) //Not Subtick Accurate!
{
    ui32 start = systime_ticks_now();
    while (!systime_ticks_expired(start, duration_ticks)) {
        __WFI();
    }
}

subtick_time_t subtick_time_now(void)
{
    ui32 t1;
    ui32 val;
    while (1)
    {
        __disable_irq(); // disable interrupts
        t1 = totalticks;
        val = STK_VAL;
        ui32 ctrl = STK_CTRL;
        bool countflag = (bool)(ctrl >> 16 & 0b1);
        __enable_irq(); // enable interrupts
        if (countflag) {continue;}
        ui32 t2 = totalticks;
        if (t1 == t2) {break;}
    }
    subtick_time_t now;
    now.ticks = t1;
    now.cyc = (STK_LOAD + 1) - val;

    return now;
}
subtick_duration_t subtick_time_elapsed(subtick_time_t *since)
{
    subtick_time_t now = subtick_time_now();
    ui32 ticks = now.ticks - since->ticks;
    ui32 cycles = 0;
    if (now.cyc >= since->cyc) { cycles = now.cyc - since->cyc; }
    else {
    ticks -= 1;
    cycles = (STK_LOAD - since->cyc) + now.cyc;
    }
    subtick_duration_t elapsed;
    elapsed.ticks_elap = ticks;
    elapsed.cyc_elap = cycles;
    return elapsed;
}
bool subtick_time_expired(subtick_time_t *start, subtick_duration_t *duration)
{
    subtick_duration_t elapsed = subtick_time_elapsed(start);
    return ((elapsed.ticks_elap > duration->ticks_elap) || 
            (elapsed.ticks_elap == duration->ticks_elap && elapsed.cyc_elap >= duration->cyc_elap));
}
void delay_subtick_ticks(subtick_duration_t *duration)
{
    subtick_time_t start = subtick_time_now();
    subtick_time_t *ptr = &start;
    while (!subtick_time_expired(ptr, duration)) {
        __WFI(); // Power Friendly Do Nothing Instruction
    }
}
subtick_duration_t gen_duration_t(ui32 ticks, ui32 cycles)
{
    if (cycles > STK_LOAD) {
        ticks += cycles / STK_LOAD;
        cycles = cycles % STK_LOAD;
    }
    subtick_duration_t generated;
    generated.ticks_elap = ticks;
    generated.cyc_elap = cycles;
    return generated;
}

ui64 ticks_to_ms(ui32 ticks)
{
    return ticks * tickperiodms; //rounded to nearest tick - quantization
}

// ui32 ms_to_ticks(ui64 ms) // wraps at 2^32 ticks
// {
//     ui64 ticks = ms / tickperiodms;
//     return (ui32)ticks;
// }
