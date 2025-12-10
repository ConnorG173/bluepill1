#include "stm32f103c8t6gpio.h"

void
reset(void)
{
    volatile unsigned int *rcc_apb2enr = (unsigned int *)(0x40021000 + 0x18);
    volatile unsigned int *gpioc_crh = (unsigned int *)(0x40011000 + 0x04);
    volatile unsigned int *gpioc_odr = (unsigned int *)(0x40011000 + 0x0C);

    // Enable GPIOC clock
    *rcc_apb2enr = *rcc_apb2enr | (0b00010000);
    // Configure PC13 as general purpose i/o: open/drain, max speed 2MHz
    *gpioc_crh = (*gpioc_crh & ~ (0xf << ((13-8)*4))) | (6 << ((13-8)*4));
    
    volatile unsigned int* ITM_LR = (unsigned int *)(0xE0000FB0);
    volatile unsigned int* DEMCR = (unsigned int *)(0xE000EDFC);
    volatile unsigned int* DWTCR = (unsigned int *)(0xE0001000);
    volatile unsigned int* DWT_CYCCNT_REG = (unsigned int *)(0xE0001004);

    *ITM_LR = 0xC5ACCE55; //Enable Write to ITM Registers
    *DEMCR |= (1 << 24); //TRCENA set on DEMCR
    *DWTCR |= (1 << 0); //Enable DWT Cycle Count
    *DWT_CYCCNT_REG = 0;

    
configure_gpio_pin('C', 13, GP_OPEN_DRAIN, OUTPUT_2MHZ);

    while (1)
    {
        for (volatile unsigned int i = 0; i <= 1000000; i++)
        {
            __asm__("NOP");
        }

        *gpioc_odr = *gpioc_odr ^ (1 << 13);
        for (volatile unsigned int i = 0; i <= 100000; i++)
        {
            __asm__("NOP");
        }
        *gpioc_odr = *gpioc_odr ^ (1 << 13);
    }
}

int STACK[256];

const void *vectors[] __attribute__ ((section (".vectors"))) = {
    STACK + sizeof(STACK)/sizeof(*STACK),
    reset
};

