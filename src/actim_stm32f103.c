
#include "platform.h"
#include "actim_stm32f103.h"
#include "clocks_stm32f103.h"

static actim_t tim1_t;
actim_t* tim1ptr = &tim1_t;
#ifdef TIM8
static actim_t tim8_t;
actim_t* tim8ptr = &tim8_t;
#endif


void tim1_reset(void)
{
    RCC->APB2RSTR |= (1 << 11);
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    RCC->APB2RSTR &= ~(1 << 11);
}

void actim_config(actim_t* a, ui32 cnt_clk_hz, ui16 cnt_period_counts, count_modes cntmode)
{
    RCC->APB2ENR |= (1 << 11); //Enable Tim1 clock
    TIM_TypeDef* tim = TIM1;

    #ifdef TIM8
    if (a->ID == 8) { tim = TIM8; }
    #endif
    
    tim1_reset();
    tim->CR1 &= ~(1 << 0); //Disable counter

    ui16 presc = ((actim_get_ckint(a)/cnt_clk_hz) & 0xFFFFUL) - 1; //16 bits | prescaler = PSC register value + 1
    tim->PSC = presc;
    tim->ARR = (cnt_period_counts - 1); // 1 cycle extra
    switch (cntmode)
    {
        case (upcnt):
        {
            tim->CR1 &= ~(0b111 << 4); //CMS = 00, clear up/down direction
            break;
        }
        case (downcnt):
        {
            tim->CR1 &= ~(0b111 << 4); //CMS = 00, clear up/down direction
            tim->CR1 |= (1 << 4); //enable down count
            break;
        }
        case (centeralign1):
        {
            tim->CR1 &= ~(0b11 << 5); //CMS = 00, clear up/down direction
            tim->CR1 |= (0b01 << 5);
            break;
        }
        case (centeralign2):
        {
            tim->CR1 &= ~(0b11 << 5); //CMS = 00, clear up/down direction
            tim->CR1 |= (0b10 << 5);
            break;
        }
        case (centeralign3):
        {
            tim->CR1 &= ~(0b11 << 5); //CMS = 00, clear up/down direction
            tim->CR1 |= (0b11 << 5);
            break;
        }
        default:
        {
            tim->CR1 &= ~(0b111 << 4); //CMS = 00, clear up/down direction
            break;
        }
    }
    a->countmode = cntmode;
    tim->CR1 |= (1 << 7); //Enable ARR Preload
    tim->EGR |= (1 << 0);
}   

inline ui32 actim_get_count(actim_t* a)
{
    return a->regs->CNT;
}
ui32 actim_get_period_counts(actim_t* a)
{
    return a->regs->ARR + 1;
}
ui32 actim_get_freq_hz(actim_t* a)
{
    ui32 counts = actim_get_period_counts(a);
    return actim_get_ckcnt(a) / counts;
}
ui32 actim_get_ckcnt(actim_t* a) //how many times do we increment TIMx_CNT per second
{
    ui32 presc = a->regs->PSC;
    return actim_get_ckint(a) / (presc + 1);
}
ui32 actim_get_ckint(actim_t* a)
{
    ui32 ckint = get_pclk2();
    ui32 apb2presc = get_hclock_frequency()/get_pclk2();
    if (apb2presc != 1) { ckint *= 2; }
    return ckint;
}

ui32 actim_get_DTSCLK(actim_t* a)
{
    ui32 CKD_bits = ((a->regs->CR1 >> 8) & 0b11);
    ui32 mul = 1;
    if (CKD_bits == 0b01) { mul = 2; }
    if (CKD_bits == 0b10) { mul = 4; }
    return actim_get_ckint(a) * mul;
}

void actim_enable_MOE(actim_t* a)
{
    a->regs->BDTR |= (1 << 15);
}
void actim_disable_MOE(actim_t* a)
{
    a->regs->BDTR &= ~(1 << 15);
}
void enable_actim(actim_t* a)
{
    a->regs->CR1 |= (1 << 0);
}
void disable_actim(actim_t* a)
{
    a->regs->CR1 &= ~(1 << 0);
}
void actim_set_DTS_mul(actim_t* a, ui32 mul) // Mul = 1, 2, or 4
{
    disable_actim(a);
    a->regs->CR1 &= ~(0b11 << 8);
    if (mul != 2 && mul != 4) {return;} //do nothing if 1
    switch (mul)
    {
        case (2): { a->regs->CR1 |= (0b01 << 8); break; }
        case (4): { a->regs->CR1 |= (0b10 << 8); break; }
        default: { a->regs->CR1 |= (0b00 << 8); break; }
    }
    enable_actim(a);
}

void enable_channel(ac_channel_t* c)
{
    ui8 id = c->ID;
    switch (id)
    {
        case (1): { *c->CCER |= (1 << 0); break; }
        case (2): { *c->CCER |= (1 << 4); break; }
        case (3): { *c->CCER |= (1 << 8); break; }
        case (4): { *c->CCER |= (1 << 12); break; }
        default:                        { break; }
    }
}
void enable_channel_compout(ac_channel_t* c)
{
    ui8 id = c->ID;
    switch (id)
    {
        case (1): { *c->CCER |= (1 << 2); break; }
        case (2): { *c->CCER |= (1 << 6); break; }
        case (3): { *c->CCER |= (1 << 10); break; }
        case (4): { *c->CCER |= (1 << 14); break; }
        default:                        { break; }
    }
}
void disable_channel(ac_channel_t* c)
{
    ui8 id = c->ID;
    switch (id)
    {
        case (1): { *c->CCER &= ~(1 << 0); break; }
        case (2): { *c->CCER &= ~(1 << 4); break; }
        case (3): { *c->CCER &= ~(1 << 8); break; }
        case (4): { *c->CCER &= ~(1 << 12); break; }
        default:                        { break; }
    }
}
void diable_channel_compout(ac_channel_t* c)
{
    ui8 id = c->ID;
    switch (id)
    {
        case (1): { *c->CCER &= ~(1 << 2); break; }
        case (2): { *c->CCER &= ~(1 << 6); break; }
        case (3): { *c->CCER &= ~(1 << 10); break; }
        case (4): { *c->CCER &= ~(1 << 14); break; }
        default:                        { break; }
    }
}

void config_channel_PWM(ac_channel_t* c, bool is_pwm_mode_2); //Must use up/down counting, not middle
bool set_pwm_duty(ac_channel_t* c, ui32 dutycycle)
{
    count_modes countmode = c->parent->countmode;
    if (countmode != upcnt && countmode != downcnt) { return; }
    if (dutycycle > 100) { dutycycle = 100; } //clamp 100
    ui32 overall = actim_get_period_counts(c->parent);
    ui32 crrload = 0;
    if (countmode == upcnt) { crrload = (overall * dutycycle / 100) - 1; }
    else { crrload = (overall * (100 - dutycycle) / 100) - 1; } // ex. dutycycle 75 -> interrupt happens at 25% of reload val and turns pin off
    c->CCR = (crrload & 0xFFFFUL); //16 bits
}

void config_channel_outcomp(ac_channel_t* c, outcomp_modes mode, bool is_active_low);
void set_outcomp_compnum(ac_channel_t* c, ui32 num);

void config_channel_incap(actim_t* a, ac_channel_t* c, bool use_alt_timer_input, input_comp_samp_freq samp_freq, ui32 threshold_n, bool capture_falling_edge);
ui32 get_incap_result_counts(ac_channel_t* c);
ui32 get_incap_result_dutycycle(ac_channel_t* c);
ui32 get_incap_result_ms(ac_channel_t* c);