
#include "platform.h"
#include "init.h"
#include "systick_stm32f103.h"
#include "usart_stm32f103.h"
#include "uprintf.h"

int main(void)
{
    stdinit();
    
    // volatile ui32 *rcc_apb2enr = (unsigned int *)(0x40021000 + 0x18);
    // volatile ui32 *gpioc_crh = (unsigned int *)(0x40011000 + 0x04);
    // volatile ui32 *gpioc_odr = (unsigned int *)(0x40011000 + 0x0C);

    // // Enable GPIOC clock
    // *rcc_apb2enr = *rcc_apb2enr | (0b00010000);
    // // Configure PC13 as general purpose i/o: open/drain, max speed 2MHz
    // *gpioc_crh = (*gpioc_crh & ~ (0xf << ((13-8)*4))) | (6 << ((13-8)*4));

    usart_config(usart1, LENGTH8B, 115200, false, false, false);
    usart1->registers->CR1 |= (1 << 5); //Enable RXNEIE
    int i = 1;
    float x = 2.325;
    x += 5.745;
    if (x > 1.0) { i = 2; }
    while (1) 
    {
        delay_ticks(2000);
        uprintf(usart1, "The variable i has a value of: %d\n", i);
    }
}

