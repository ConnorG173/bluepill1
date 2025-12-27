
#include "platform.h"
#include "clocks_stm32f103.h"
#include "systick_stm32f103.h"
#include "gpio_stm32f103.h"

void stdinit(void)
{
    clk_config(PLL, 1, 0, 9, 1, 2, 1, 6);
    systick_init();
    SystemCoreClockUpdate();

    //GPIO INITs

    //END GPIO
}