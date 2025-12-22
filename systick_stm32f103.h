#ifndef SYSTICK_STM32F103_H_
#define SYSTICK_STM32F103_H_

#include <stdint.h>

void systick_init(uint32_t periodms);

void systick_start(void);
void systick_stop(void);


void delay_ticks(uint32_t ticks);
void delay_ms(uint32_t miliseconds);
void delay_s(uint32_t seconds);

uint32_t systime_ticks_now(void);
uint32_t systime_ticks_elapsed(uint32_t since);
bool systime_expired(uint32_t start, uint32_t duration);

#endif