#ifndef CLOCKS_STM32F103_H_
#define CLOCKS_STM32F103_H_

#include "platform.h"

enum CLKSRC
{
    HSI,
    HSE,
    PLL
};

void clk_config(enum CLKSRC source, bool pll_src_hse, bool pll_hse_div2, int pll_mul, int ahb_pre, int apb1_pre, int apb2_pre, int adc_pre);
uint32_t get_hclock_frequency(void);

#endif