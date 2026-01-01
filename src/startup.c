
#include "platform.h"

extern ui32 _estack;

extern ui32 _sdata;
extern ui32 _edata;
extern ui32 _sidata;

extern ui32 _sbss;
extern ui32 _ebss;

void Reset_Handler(void); // Never Returns
extern int main(void);

void Default_Handler(void);

void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler"))); // Weak, points to default if not defined
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVCall_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMonitor_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

void USART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));


// __attribute__((section(".vectors"), used)) // "Linker, put this in the ".vectors" section, used = no remove on -O2
// void (* const vectors[])(void) = { // Constant (live in flash) Array of ptrs to functions that return void
//     (void (*)(void))(&_estack),  // Initial stack pointer, [0]
//     Reset_Handler,               // Reset, [1]
//     NMI_Handler,                 // 2
//     HardFault_Handler,           // 3
//     MemManage_Handler,           // 4
//     BusFault_Handler,            // 5
//     UsageFault_Handler,          // 6
//     0, 0, 0, 0,                  // 7–10 reserved
//     SVCall_Handler,             // 11 SVCall
//     DebugMonitor_Handler,             // 12 DebugMonitor
//     0,                           // 13 reserved
//     PendSV_Handler,             // 14 PendSV
//     SysTick_Handler,              // 15 SysTick

// };

__attribute__((section(".vectors"), used)) // "Linker, put this in the ".vectors" section, used = no remove on -O2
void (* const vectors[])(void) = { // Constant (live in flash) Array of ptrs to functions that return void
    /* Core exceptions */
    [0]  = (void (*)(void))(&_estack), // Initial stack pointer
    [1]  = Reset_Handler,
    [2]  = NMI_Handler,
    [3]  = HardFault_Handler,
    [4]  = MemManage_Handler,
    [5]  = BusFault_Handler,
    [6]  = UsageFault_Handler,
    [11] = SVCall_Handler,
    [12] = DebugMonitor_Handler,
    [14] = PendSV_Handler,
    [15] = SysTick_Handler,

    /* USART interrupts */
    [53] = USART1_IRQHandler,
    [54] = USART2_IRQHandler,
    [55] = USART3_IRQHandler,
};

void Reset_Handler(void)
{
    __asm__ volatile ("cpsid i"); //Disable All Interrupts
    ui32 *src;
    ui32 *dst;

    src = &_sidata;
    for (dst = &_sdata; dst < &_edata; dst++) // Write initialized globals from flash into ram
    {
        *dst = *src;
        src++;
    }

    dst = &_sbss;
    while (dst < &_ebss) // Zero the rest of ram (uninitialized globals)
    {
        *dst++ = 0;
    }

    __asm__ volatile ("cpsie i"); //ReEnable Interrupts before main()
    main(); // Call main()

    while(1); // Infinite loop to handle main return
}

void Default_Handler(void)
{
    while(1);
}

