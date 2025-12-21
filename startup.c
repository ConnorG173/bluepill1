#include <stdint.h>

extern uint32_t _estack;

extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;

extern uint32_t _sbss;
extern uint32_t _ebss;

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

__attribute__((section(".vectors"), used)) // "Linker, put this in the ".vectors" section, used = no remove on -O2
void (* const vectors[])(void) = { // Constant (live in flash) Array of ptrs to functions that return void
    (void (*)(void))(&_estack),  // Initial stack pointer, [0]
    Reset_Handler,               // Reset, [1]
    NMI_Handler,                 // 2
    HardFault_Handler,           // 3
    MemManage_Handler,           // 4
    BusFault_Handler,            // 5
    UsageFault_Handler,          // 6
    0, 0, 0, 0,                  // 7–10 reserved
    Default_Handler,             // 11 SVCall
    Default_Handler,             // 12 DebugMonitor
    0,                           // 13 reserved
    Default_Handler,             // 14 PendSV
    Default_Handler              // 15 SysTick      
};

void Reset_Handler(void)
{
    __asm volatile ("cpsid i"); //Disable All Interrupts
    uint32_t *src;
    uint32_t *dst;

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

    __asm volatile ("cpsie i"); //ReEnable Interrupts before main()
    main(); // Call main()

    while(1); // Infinite loop to handle main return
}

void Default_Handler(void)
{
    while(1);
}

