#ifndef SYSTICK_STM32F103_H_
#define SYSTICK_STM32F103_H_

#include <stdint.h>
#include <stdbool.h>

extern const uint32_t tickperiodms;
extern volatile uint32_t totalticks;

bool systick_init(void); //returns 1 if success, 0 if fail

void systick_start(void);
void systick_stop(void);

uint32_t systime_ticks_now(void);

uint32_t systime_ticks_elapsed(uint32_t since_time_ticks);
bool systime_ticks_expired(uint32_t start_ticks, uint32_t duration_ticks);
void delay_ticks(uint32_t duration_ticks);

uint64_t ticks_to_ms(uint32_t ticks);
uint32_t ms_to_ticks(uint64_t ms);

#endif