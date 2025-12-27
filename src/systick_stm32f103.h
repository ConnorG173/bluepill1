#ifndef SYSTICK_STM32F103_H_
#define SYSTICK_STM32F103_H_

#include <stdint.h>
#include <stdbool.h>

extern const uint32_t tickperiodms;
extern volatile uint32_t totalticks;

typedef struct subtick_time{
    uint32_t ticks;
    uint32_t cyc;
} subtick_time_t;

typedef struct subtick_duration{
    uint32_t ticks_elap;
    uint32_t cyc_elap;
} subtick_duration_t;

bool systick_init(void); //returns 1 if success, 0 if fail

void systick_start(void);
void systick_stop(void);

uint32_t systime_ticks_now(void);

uint32_t systime_ticks_elapsed(uint32_t since_time_ticks);
bool systime_ticks_expired(uint32_t start_ticks, uint32_t duration_ticks);
void delay_ticks(uint32_t duration_ticks);

subtick_time_t subtick_time_now(void);
subtick_duration_t subtick_time_elapsed(subtick_time_t *since);
bool subtick_time_expired(subtick_time_t *start, subtick_duration_t *duration);
void delay_subtick_ticks(subtick_duration_t *duration);
subtick_duration_t gen_duration_t(uint32_t ticks, uint32_t cycles); //cycles less than STK_LOAD

uint64_t ticks_to_ms(uint32_t ticks);
uint32_t ms_to_ticks(uint64_t ms);

#endif