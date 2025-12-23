#include "stm32f103c8t6gpio.h"

void configure_gpio_pin(char port, int pin, enum GPIO_CONFIGS config, enum GPIO_MODES mode)
{
    volatile unsigned int *rcc_apb2enr = (unsigned int *)(0x40021000);
    volatile unsigned int *gpio_cr_base = (unsigned int *)(0x0);
    switch (port)
    {
        case 'A':
            *rcc_apb2enr |= (1 << 2);
            gpio_cr_base = (unsigned int *)(0x40010800);
            break;
        case 'B':
            *rcc_apb2enr |= (1 << 3);
            gpio_cr_base = (unsigned int *)(0x40010C00);
            break;
        case 'C':
            *rcc_apb2enr |= (1 << 4);
            gpio_cr_base = (unsigned int *)(0x40011000);
            break;
        case 'D':
            *rcc_apb2enr |= (1 << 5);
            gpio_cr_base = (unsigned int *)(0x40011400);
            break;
        case 'E':
            *rcc_apb2enr |= (1 << 6);
            gpio_cr_base = (unsigned int *)(0x40011800);
            break;
        default:
            return;
    }
    volatile unsigned int *gpio_cr;
    if (pin > 7)
    {
        gpio_cr = gpio_cr_base + 0x04; // CRH
        pin -= 8;
    }
    else
    {
        gpio_cr = gpio_cr_base + 0x00; // CRL
    }

    int cnf_mode_bitmask = 0b0000;
    switch (mode)
    {
        case INPUT:
            cnf_mode_bitmask |= 0b00;
            break;
        case OUTPUT_10MHZ:
            cnf_mode_bitmask |= 0b01;
            break;
        case OUTPUT_2MHZ:
            cnf_mode_bitmask |= 0b10;
            break;
        case OUTPUT_50MHZ:
            cnf_mode_bitmask |= 0b11;
            break;
    }
    switch (config)
    {
        case ANALOG:
            cnf_mode_bitmask |= (0b00 << 2);
            break;
        case FLOATING:
            cnf_mode_bitmask |= (0b01 << 2);
            break;
        case PULLUP_PULLDOWN:
            cnf_mode_bitmask |= (0b10 << 2);
            break;
        case RESERVED_DONOTUSE:
            cnf_mode_bitmask |= (0b11 << 2);
            break;
        case GP_PUSH_PULL:
            cnf_mode_bitmask |= (0b00 << 2);
            break;
        case GP_OPEN_DRAIN:
            cnf_mode_bitmask |= (0b01 << 2);
            break;
        case AF_PUSH_PULL:
            cnf_mode_bitmask |= (0b10 << 2);
            break;
        case AF_OPEN_DRAIN:
            cnf_mode_bitmask |= (0b11 << 2);
            break;
    }
    int shift = pin * 4;
    *gpio_cr = *gpio_cr | (cnf_mode_bitmask << shift);
}

