#ifndef ACTIM_STM32F103_H
#define ACTIM_STM32F103_H

#include "platform.h"

// Forward decl
typedef struct ACTIM actim_t;
typedef struct channel ac_channel_t;

typedef enum channelModes {
    inputCapture,
    outputCompare,
    PWM1,
    PWM2
} ac_channel_modes;

typedef enum countModes {
    upcnt,
    downcnt,
    centeralign1,
    centeralign2,
    centeralign3
} count_modes;

typedef enum outcompModes {
    frozen,
    sethigh,
    setlow,
    toggle,
    forcehigh,
    forcelow
} outcomp_modes;

typedef enum inputCompSampFreq {
    CK_INT,
    DTS_DIV2,
    DTS_DIV4,
    DTS_DIV8,
    DTS_DIV16,
    DTS_DIV32,
} input_comp_samp_freq;

typedef struct ACTIM {
    TIM_TypeDef* regs; 
    ac_channel_t* ch0;
    ac_channel_t* ch1;
    ac_channel_t* ch2;
    ac_channel_t* ch3;
    ui8 ID;
    count_modes countmode;
} actim_t;

typedef struct channel {
    volatile ui32* CCER;
    volatile ui32* CCR;
    ui8 ID; //1, 2, 3, or 4
    ac_channel_modes mode;
    actim_t* parent;
} ac_channel_t;

extern actim_t* tim1ptr;
#ifdef TIM8
extern actim_t* tim8ptr;
#endif
// No Tim8 on STM32F103C8

void actim_config(actim_t* a, ui32 cnt_clk_hz, ui16 cnt_period_counts, count_modes cntmode);

ui32 actim_get_count(actim_t* a);
ui32 actim_get_period_counts(actim_t* a);
ui32 actim_get_freq_hz(actim_t* a);
ui32 actim_get_ckcnt(actim_t* a);
ui32 actim_get_ckint(actim_t* a);
ui32 actim_get_DTSCLK(actim_t* a);

void actim_enable_MOE(actim_t* a);
void actim_disable_MOE(actim_t* a);

void enable_actim(actim_t* a);
void disable_actim(actim_t* a);
void actim_set_DTS_mul(actim_t* a, ui32 mul); // Mul = 1, 2, or 4

void enable_channel(ac_channel_t* c);
void enable_channel_compout(ac_channel_t* c);
void disable_channel(ac_channel_t* c);
void diable_channel_compout(ac_channel_t* c);

void config_channel_PWM(ac_channel_t* c, bool is_pwm_mode_2);
bool set_pwm_duty(ac_channel_t* c, ui32 dutycycle);

void config_channel_outcomp(ac_channel_t* c, outcomp_modes mode, bool is_active_low);
void set_outcomp_compnum(ac_channel_t* c, ui32 num);

void config_channel_incap(actim_t* a, ac_channel_t* c, bool use_alt_timer_input, input_comp_samp_freq samp_freq, ui32 threshold_n, bool capture_falling_edge);
ui32 get_incap_result_counts(ac_channel_t* c);
ui32 get_incap_result_dutycycle(ac_channel_t* c);
ui32 get_incap_result_ms(ac_channel_t* c);

#endif