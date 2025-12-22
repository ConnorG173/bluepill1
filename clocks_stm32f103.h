#ifndef CLOCKS_STM32F103_H_
#define CLOCKS_STM32F103_H_

#include <stdint.h>
#include <stdbool.h>

enum CLKSRC
{
    HSI,
    HSE,
    PLL
};

void clk_config(enum CLKSRC source, bool pll_src_hse, bool pll_hse_div2, int pll_mul, int ahb_pre, int apb1_pre, int apb2_pre, int adc_pre);

void systick_count(int value);

uint32_t get_hclock_frequency(void);

void crude_delay_cyc(uint64_t n); //Delay for n cycles using 1-cycle loop
void crude_delay_ms(int n); //Delay for n milliseconds using crude_delay_cyc
void crude_delay_s(int n); //delay for n seconds using crude_delay_cyc

void delay_cyc(uint64_t n); //Delay for n systick cycles
void delay_ms(int n); //Delay for n milliseconds using systick
void delay_s(int n); //delay for n seconds using systick

#endif