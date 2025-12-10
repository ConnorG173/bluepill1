#include <memory.h>

void init_cyccnt(void)
{
    volatile unsigned int* ITM_LR = (unsigned int *)(0xE0000FB0);
    volatile unsigned int* DEMCR = (unsigned int *)(0xE000EDFC);
    volatile unsigned int* DWTCR = (unsigned int *)(0xE0001000);
    volatile unsigned int* DWT_CYCCNT_REG = (unsigned int *)(0xE0001004);

    *ITM_LR = 0xC5ACCE55; //Enable Write to ITM Registers
    *DEMCR |= (1 << 24); //TRCENA set on DEMCR
    *DWTCR |= (1 << 0); //Enable DWT Cycle Count
    *DWT_CYCCNT_REG = 0;
}

int get_hclock_frequency(void)
{
    volatile unsigned int* RCC_CFGR = (unsigned int *)(0x40021000 + 0x04);

    const int HSE_FRQ = 8000000; //Change if different
    const int HSI_FRQ = 8000000;

    enum CLKSRC //Bits 3:2 
    {
        HSI,
        HSE,
        PLL,
    };
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

    int AHBPRE = 1;
    if (((*RCC_CFGR >> 7) & 0b1) == 0b0) {AHBPRE = 1;}
    else {AHBPRE = 1 << (((*RCC_CFGR >> 4) & 0b111) + 1);} //2 ^ value of ((bits 6:4) + 1)

    /*
    ask chatgpt:
    i dont think it's incorrect. The if statement checks bit 7, and if it's 0, the value is in the range 0-7, 
    so AHBPRE must be 1. Then, i use the other 3 bits to check the rest of the values. if you notice, the AHBPRE 
    is always the value 2 to the power of ((bits 6:4) + 1), so i set AHBPRE equal to that value when bit 7 is not 0. 
    i calculate 2^n by bitshifting 1 left by n.
    */

    return PLLSRCFREQ * PLLMUL / AHBPRE;
}

unsigned int get_cycle_count(void)
{
    volatile unsigned int* DWT_CYCCNT_REG = (unsigned int *)(0xE0001004);
    return *DWT_CYCCNT_REG;
}

void delay_cyc(int n) // Delays the program for n cycles
{
    volatile unsigned int* DWT_CYCCNT_REG = (unsigned int *)(0xE0001004);
    *DWT_CYCCNT_REG = 0;
    int count = *DWT_CYCCNT_REG;
    int* countptr = &count;
    while (count < n)
    {
        memcpy(countptr, DWT_CYCCNT_REG, 32);
    }
}

void delay_s(int n)
{
    delay_cyc(get_hclock_frequency() * n);
}

void delay_ms(int n)
{
    delay_cyc(get_hclock_frequency()/1000 * n);
}