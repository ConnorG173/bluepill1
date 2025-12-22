#include "clocks_stm32f103.h"


int main(void)
{
    volatile unsigned int *rcc_apb2enr = (unsigned int *)(0x40021000 + 0x18);
    volatile unsigned int *gpioc_crh = (unsigned int *)(0x40011000 + 0x04);
    volatile unsigned int *gpioc_odr = (unsigned int *)(0x40011000 + 0x0C);

    // Enable GPIOC clock
    *rcc_apb2enr = *rcc_apb2enr | (0b00010000);
    // Configure PC13 as general purpose i/o: open/drain, max speed 2MHz
    *gpioc_crh = (*gpioc_crh & ~ (0xf << ((13-8)*4))) | (6 << ((13-8)*4));

    for (int i = 0; i < 20; i++)
    {
        for (volatile unsigned int i = 0; i <= 100000; i++)
        {
            __asm__("NOP");
        }
        *gpioc_odr = *gpioc_odr ^ (1 << 13);
    }

    clk_config(PLL, 1, 0, 9, 1, 2, 1, 8);

    while (1)
    {
        for (volatile unsigned int i = 0; i <= 100000; i++)
        {
            __asm__("NOP");
        }
        *gpioc_odr = *gpioc_odr ^ (1 << 13);
    }

}
