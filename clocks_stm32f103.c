#include <memory.h>
#include "clocks_stm32f103.h"


void clk_config(enum CLKSRC source, bool pll_src_hse, bool pll_hse_div2, int pll_mul, int ahb_pre, int apb1_pre, int apb2_pre, int adc_pre)
{
    volatile unsigned int* RCC_CR = (unsigned int *)(0x40021000);
    volatile unsigned int* RCC_CFGR = (unsigned int *)(0x40021000 + 0x04);
    volatile unsigned int* FLASH_ACR = (unsigned int *)(0x40022000 + 0x00);

    const int HSI_FREQ = 8000000;
    const int HSE_FREQ = 8000000; //Change if different

    int pll_input = 0;
    int pll_output = 0;
    int sysclk;
    int hclk = 0;
    int pclk1 = 0;
    int pclk2 = 0;
    int adcclk = 0;

    switch (source)
    {
        case HSI:
            sysclk = HSI_FREQ;
            hclk = sysclk / ahb_pre;
            break;
        case HSE:
            sysclk = HSE_FREQ;
            hclk = sysclk / ahb_pre;
            break;
        case PLL:
            if (pll_src_hse)
            {
                pll_input = HSE_FREQ;
                if (pll_hse_div2) {pll_input /= 2;}
            }
            else
            {
                pll_input = HSI_FREQ / 2;
            }
            sysclk = pll_input * pll_mul;
            hclk = sysclk / ahb_pre;
            break;
    }
    pclk1 = hclk / apb1_pre;
    pclk2 = hclk / apb2_pre;
    adcclk = pclk2 / adc_pre;
    if (sysclk > 72000000 || pclk1 > 36000000 || adcclk > 14000000)
    {
        return; //Frequency too high for STM32F103
    }

    *RCC_CFGR &= ~(0b11 << 0); //Stay or Switch to HSI before final configs
    int wait_timeout = 0x20000;
    while (((*RCC_CFGR >> 2) & 0b11) != 0b00) {if (!--wait_timeout) return;} //Wait for HSI SWS before doing the rest

    *RCC_CR &= ~(1 << 24); //Diable PLL during config
    wait_timeout = 0x20000;
    while (((*RCC_CR >> 25) & 0b1) != 0b0) {if (!--wait_timeout) return;} //Wait for HSI SWS before doing the rest

    if ((pll_src_hse && source == PLL) || source == HSE)
    {
        *RCC_CR |= (1 << 16); //Enable HSE
        wait_timeout = 0x20000;
        while(((*RCC_CR >> 17) & 0b1) == 0b0) {if (!--wait_timeout) return;} //Wait for HSE Ready
    }
    if (( !pll_src_hse && source == PLL) || source == HSI)
    {
        *RCC_CR |= (1 << 0);
        wait_timeout = 0x20000;
        while(((*RCC_CR >> 1) & 0b1) == 0b0) {if (!--wait_timeout) return;} //Wait for HSI Ready
    }

    //Flash Memory Latency Config for new SYSCLK
    *FLASH_ACR &= ~(0b011); //Reset flash latency bits
    if (sysclk <= 24000000)      { *FLASH_ACR |= 0b000; } //0 wait states
    else if (sysclk <= 48000000) { *FLASH_ACR |= 0b001; } //1 wait state
    else                         { *FLASH_ACR |= 0b010; } //2 wait states
    
    *FLASH_ACR |= (1 << 4); //Enable Prefetch Buffer

    int ahb_pre_mask = 0;
    switch (ahb_pre)
    {
        case 1: ahb_pre_mask = 0b0000; break;
        case 2: ahb_pre_mask = 0b1000; break;
        case 4: ahb_pre_mask = 0b1001; break;
        case 8: ahb_pre_mask = 0b1010; break;
        case 16: ahb_pre_mask = 0b1011; break;
        case 64: ahb_pre_mask = 0b1100; break;
        case 128: ahb_pre_mask = 0b1101; break;
        case 256: ahb_pre_mask = 0b1110; break;
        case 512: ahb_pre_mask = 0b1111; break;
        default: ahb_pre_mask = 0b0000; break;
    }
    *RCC_CFGR &= ~(0b1111 << 4); //Clear AHB Prescaler bits
    *RCC_CFGR |= (ahb_pre_mask << 4); //Set AHB Prescaler
    int apb1_pre_mask = 0;
    switch (apb1_pre)
    {
        case 1: apb1_pre_mask = 0b000; break;
        case 2: apb1_pre_mask = 0b100; break;
        case 4: apb1_pre_mask = 0b101; break;
        case 8: apb1_pre_mask = 0b110; break;
        case 16: apb1_pre_mask = 0b111; break;
        default: apb1_pre_mask = 0b000; break;
    }
    *RCC_CFGR &= ~(0b111 << 8); //Clear APB1 Prescaler bits
    *RCC_CFGR |= (apb1_pre_mask << 8); //Set APB1 Prescaler
    int apb2_pre_mask = 0;
    switch (apb2_pre)
    {
        case 1: apb2_pre_mask = 0b000; break;
        case 2: apb2_pre_mask = 0b100; break;
        case 4: apb2_pre_mask = 0b101; break;
        case 8: apb2_pre_mask = 0b110; break;
        case 16: apb2_pre_mask = 0b111; break;
        default: apb2_pre_mask = 0b000; break; 
    }
    *RCC_CFGR &= ~(0b111 << 11); //Clear APB2 Prescaler bits
    *RCC_CFGR |= (apb2_pre_mask << 11); //Set APB2 Prescaler
    int adc_pre_mask = 0;
    switch (adc_pre)
    {
        case 2: adc_pre_mask = 0b00; break;
        case 4: adc_pre_mask = 0b01; break;
        case 8: adc_pre_mask = 0b10; break;
        case 16: adc_pre_mask = 0b11; break;
        default: adc_pre_mask = 0b00; break;
    }
    *RCC_CFGR &= ~(0b11 << 14); //Clear ADC Prescaler bits
    *RCC_CFGR |= (adc_pre_mask << 14); //Set ADC Prescaler

    if (source == HSI) 
    {
        wait_timeout = 0x20000;
        while (((*RCC_CFGR >> 2) & 0b11) != 0b00) {if (!--wait_timeout) return;}
        return;
    }
    if (source == HSE)
    {
        *RCC_CFGR |= 0b01;
        wait_timeout = 0x20000;
        while (((*RCC_CFGR >> 2) & 0b11) != 0b01) {if (!--wait_timeout) return;} //Wait for HSE SWS
        return;
    }
    *RCC_CFGR &= ~(1 << 16);
    *RCC_CFGR &= ~(1 << 17);
    if (pll_src_hse) {*RCC_CFGR |= (1 << 16);}
    if (pll_hse_div2) {*RCC_CFGR |= (1 << 17);}

    int pll_mul_mask = 0;
    switch (pll_mul)
    {
        case 2: pll_mul_mask = 0b0000; break;
        case 3: pll_mul_mask = 0b0001; break;
        case 4: pll_mul_mask = 0b0010; break;
        case 5: pll_mul_mask = 0b0011; break;
        case 6: pll_mul_mask = 0b0100; break;
        case 7: pll_mul_mask = 0b0101; break;
        case 8: pll_mul_mask = 0b0110; break;
        case 9: pll_mul_mask = 0b0111; break;
        case 10: pll_mul_mask = 0b1000; break;
        case 11: pll_mul_mask = 0b1001; break;
        case 12: pll_mul_mask = 0b1010; break;
        case 13: pll_mul_mask = 0b1011; break;
        case 14: pll_mul_mask = 0b1100; break;
        case 15: pll_mul_mask = 0b1101; break;
        case 16: pll_mul_mask = 0b1110; break;
        default: pll_mul_mask = 0b0000; break;
    }
    *RCC_CFGR &= ~(0b1111 << 18); //Clear PLL Multiplier bits
    *RCC_CFGR |= (pll_mul_mask << 18); //Config PLL Multiplier

    *RCC_CR |= (1 << 24); //Enable PLL
    wait_timeout = 0x20000;
    while (((*RCC_CR >> 25) & 0b1) == 0b0) {if (!--wait_timeout) return;} //Wait for PLL Ready

    *RCC_CFGR |= 0b10; //Switch to PLL
    wait_timeout = 0x20000;
    while (((*RCC_CFGR >> 2) & 0b11) != 0b10) {if (!--wait_timeout) return;} //Wait for PLL SWS
    return;
}

uint32_t get_hclock_frequency(void)
{
    volatile unsigned int* RCC_CFGR = (unsigned int *)(0x40021000 + 0x04);

    const int HSE_FRQ = 8000000; //Change if different
    const int HSI_FRQ = 8000000;

    enum CLKSRC clocksource = HSI;
    clocksource = (*RCC_CFGR >> 2) & 0b11;

    if (clocksource == HSI) {return HSI_FRQ;}
    if (clocksource == HSE) {return HSE_FRQ;}

    int PLLXTPRE = 1;
    if (((*RCC_CFGR >> 17) & 0b1) == 0b1) {PLLXTPRE = 2;}

    int PLLSRCFREQ = HSI_FRQ / 2; //(HSI / 2)
    if (((*RCC_CFGR >> 16) & 0b1) == 0b1) {PLLSRCFREQ = (HSE_FRQ / PLLXTPRE);} //(HSE / PLLXTPRE)

    int PLLMUL = 2;
    if (((*RCC_CFGR >> 18) & 0b1111) == 0b1111) {PLLMUL = 16;}
    else {PLLMUL = ((*RCC_CFGR >> 18) & 0b1111) + 2;}
    
    int index = ((*RCC_CFGR >> 4) & 0b1111);
    static const int AHBPrescTable[16] = {1,1,1,1,1,1,1,1,2,4,8,16,64,128,256,512};
    int AHBPRE = AHBPrescTable[index];

    return PLLSRCFREQ * PLLMUL / AHBPRE;
}

void crude_delay_cyc(uint32_t cycles)
{
    while(cycles)
    {
        __asm__("NOP"); //1 Cycle Loop
        cycles--;
    }
}