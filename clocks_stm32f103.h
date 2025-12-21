#ifndef CLOCKS_STM32F103_H_
#define CLOCKS_STM32F103_H_

#include <stdint.h>
#include <stdbool.h>

volatile unsigned int* RCC_CFGR = (unsigned int *)(0x40021000 + 0x04);

//Maybe Unused
volatile unsigned int* ITM_LR = (unsigned int *)(0xE0000FB0);
volatile unsigned int* DEMCR = (unsigned int *)(0xE000EDFC);
volatile unsigned int* DWTCR = (unsigned int *)(0xE0001000);
volatile unsigned int* DWT_CYCCNT_REG = (unsigned int *)(0xE0001004);

enum CLKSRC
{
    HSI,
    HSE,
    PLL
};

void clk_config(enum CLKSRC source, bool pll_src_hse, bool pll_hse_div2, int pll_mul, int ahb_pre, int apb1_pre, int apb2_pre, int adc_pre);

void systick_count(int value);

uint64_t get_hclock_frequency(void);

void crude_delay_cyc(uint64_t n); //Delay for n cycles using 1-cycle loop
void crude_delay_ms(int n); //Delay for n milliseconds using crude_delay_cyc
void crude_delay_s(int n); //delay for n seconds using crude_delay_cyc

void delay_cyc(uint64_t n); //Delay for n systick cycles
void delay_ms(int n); //Delay for n milliseconds using systick
void delay_s(int n); //delay for n seconds using systick

#endif