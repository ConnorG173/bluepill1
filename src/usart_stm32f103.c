#include "platform.h"
#include "usart_stm32f103.h"
#include "clocks_stm32f103.h"


static void config_usart_clocks(USART_NUMBER usart_number)
{
    switch (usart_number)
    {
        case USART_1:
        {
            RCC->APB2ENR |= ((1 << 0) | (1 << 2) | (1 << 14)); //Enable AFIO, GPIOA, USART1 clocks
            break;
        }
        case USART_2:
        {
            RCC->APB2ENR |= ((1 << 2) | (1 << 0)); //Enable GPIOA, AFIO clocks
            RCC->APB1ENR |= (1 << 17); //Enable USART2 clock
            break;
        }
        case USART_3:
        {
            RCC->APB2ENR |= ((1 << 3) | (1 << 0)); //Enable GPIOB, AFIO clocks
            RCC->APB1ENR |= (1 << 18); //Enable USART3 clock
            break;
        }
    }
}

static void config_usart_pins(USART_NUMBER usart_number, bool clock_used)
{
    switch (usart_number)
    {
        case USART_1: //PA9 TX, PA10 RX
        {
            GPIOA->CRH &= ~(0b11111111 << 4); //Clear PA9 and PA10 config
            GPIOA->CRH |= (0b1011 << 4); //PA9 -> Alt func output, 50MHz
            GPIOA->CRH |= (0b1000 << 8); //PA10 -> input with pull up
            break;
        }
        case USART_2: //PA2 TX, PA3 RX, PA4 CK
        {
            GPIOA->CRL &= ~(0b111111111111 << 8); //Clear PA2, PA3, PA4 config
            GPIOA->CRL |= (0b1011 << 8); //PA2 -> Alt func output, 50MHz
            GPIOA->CRL |= (0b1000 << 12); //PA3 -> input with pull up
            if (clock_used)
            {
                GPIOA->CRL |= (0b1011 << 16); //PA4 -> Alt func output, 50MHz
            }
            break;
        }
        case USART_3: //PB10 TX, PB11 RX, PB12 CK
        {
            GPIOB->CRH &= ~(0b111111111111 << 8); //Clear PB10, PB11, PB12 config
            GPIOB->CRH |= (0b1011 << 8); //PB10 -> Alt func output, 50MHz
            GPIOB->CRH |= (0b1000 << 12); //PB11 -> input with pull up
            if (clock_used)
            {
                GPIOB->CRH |= (0b1011 << 16); //PB12 -> Alt func output, 50MHz
            }
            break;
        }
    }
}

static void set_USART_BRR(USART_NUMBER usart_number, uint32_t baud_rate)
{
    /* 
    baud = pclk / 16 * USARTDIV 
    USARTDIV is an unsigned fixed point number that is coded on the USART_BRR register.
    USART_BRR: bits [31:16] -> reserved, [15:4] -> USART_DIV Mantissa, [3:0] -> USART_DIV Fraction (ex. [3:0] = 0011 -> 3/16)
    */
    uint32_t pclk = 1;
    if (usart_number == USART_1) {
        pclk = get_pclk2();
    }
    else {
        pclk = get_pclk1();
    }
    uint32_t USART_DIV = (pclk + baud_rate/2) / baud_rate; //The fixed point shift will do the /16 for us
    switch (usart_number)
    {
        case USART_1:
        {
            USART1->BRR = USART_DIV;
            break;
        }
        case USART_2:
        {
            USART2->BRR = USART_DIV;
            break;
        }
        case USART_3:
        {
            USART3->BRR = USART_DIV;
            break;
        }
    }
}

static void set_basic_configs(USART_NUMBER usart_number, bool parity_used, bool parity_selection_odd)
//Also clears rest of CR1
{
    switch (usart_number)
    {
        case USART_1:
        {
            USART1->CR1 &= 0;
            USART1->CR1 |= ((1 << 2) | (1 << 3));
            if (parity_used)
            {
                USART1->CR1 |= ((1 << 10) | ((uint8_t)parity_selection_odd << 9));
            }
            USART1->CR1 |= (1 << 13);
            break;
        }
        case USART_2:
        {
            USART2->CR1 &= 0;
            USART2->CR1 |= ((1 << 2) | (1 << 3));
            if (parity_used)
            {
                USART2->CR1 |= ((1 << 10) | ((uint8_t)parity_selection_odd << 9));
            }
            USART2->CR1 |= (1 << 13);
            break;
        }
        case USART_3:
        {
            USART3->CR1 &= 0;
            USART3->CR1 |= ((1 << 2) | (1 << 3));
            if (parity_used)
            {
                USART3->CR1 |= ((1 << 10) | ((uint8_t)parity_selection_odd << 9));
            }
            USART3->CR1 |= (1 << 13);
            break;
        }
    }
}

static void set_word_length(USART_NUMBER usart_number, usart_wordlength_t wordlength)
{
    switch (usart_number)
    {
        case USART_1:
        {
            if (wordlength == LENGTH9B) { USART1->CR1 |= (1 << 12); }
        }
        case USART_2:
        {
            if (wordlength == LENGTH9B) { USART2->CR1 |= (1 << 12); }
        }
        case USART_3:
        {
            if (wordlength == LENGTH9B) { USART3->CR1 |= (1 << 12); }
        }
    }
}

usart_t* usart_config(USART_NUMBER usart_number, usart_wordlength_t mode, uint32_t baud_rate_bps, bool clock_used, bool parity_used, bool parity_selection_odd)
// You can call this multiple times to re-configure a usart channel to new settings
{
    config_usart_clocks(usart_number);
    config_usart_pins(usart_number, clock_used);
    set_USART_BRR(usart_number, baud_rate_bps);
    set_basic_configs(usart_number, parity_used, parity_selection_odd); //Also clears rest of CR1
    set_word_length(usart_number, mode);

    switch (usart_number)
    {
        case USART_1:
        {   
            if (mode == LENGTH8B)
            {
            static ringbuf8_t ringbuf1tx;
            static ringbuf8_t ringbuf1rx;
            static usart_t usart1_obj;
            usart1_obj.registers = USART1;
            usart1_obj.tx = &ringbuf1tx;
            usart1_obj.rx = &ringbuf1rx;
            usart1_obj.mode = mode;
            usart_t *ptr = &usart1_obj;
            return ptr;
            }
            else
            {
            static ringbuf9_t ringbuf1tx;
            static ringbuf9_t ringbuf1rx;
            static usart_t usart1_obj;
            usart1_obj.registers = USART1;
            usart1_obj.tx = &ringbuf1tx;
            usart1_obj.rx = &ringbuf1rx;
            usart1_obj.mode = mode;
            usart_t *ptr = &usart1_obj;
            return ptr;
            }
            break;
        }
        case USART_2:
        {   
            if (mode == LENGTH8B)
            {
            static ringbuf8_t ringbuf2tx;
            static ringbuf8_t ringbuf2rx;
            static usart_t usart2_obj;
            usart2_obj.registers = USART2;
            usart2_obj.tx = &ringbuf2tx;
            usart2_obj.rx = &ringbuf2rx;
            usart2_obj.mode = mode;
            usart_t *ptr = &usart2_obj;
            return ptr;
            }
            else
            {
            static ringbuf9_t ringbuf2tx;
            static ringbuf9_t ringbuf2rx;
            static usart_t usart2_obj;
            usart2_obj.registers = USART2;
            usart2_obj.tx = &ringbuf2tx;
            usart2_obj.rx = &ringbuf2rx;
            usart2_obj.mode = mode;
            usart_t *ptr = &usart2_obj;
            return ptr;
            }
            break;
        }
        case USART_3:
        {   
            if (mode == LENGTH8B)
            {
            static ringbuf8_t ringbuf3tx;
            static ringbuf8_t ringbuf3rx;
            static usart_t usart3_obj;
            usart3_obj.registers = USART3;
            usart3_obj.tx = &ringbuf3tx;
            usart3_obj.rx = &ringbuf3rx;
            usart3_obj.mode = mode;
            usart_t *ptr = &usart3_obj;
            return ptr;
            }
            else
            {
            static ringbuf9_t ringbuf3tx;
            static ringbuf9_t ringbuf3rx;
            static usart_t usart3_obj;
            usart3_obj.registers = USART3;
            usart3_obj.tx = &ringbuf3tx;
            usart3_obj.rx = &ringbuf3rx;
            usart3_obj.mode = mode;
            usart_t *ptr = &usart3_obj;
            return ptr;
            }
            break;
        }
    }
}

static inline bool rb8_push(ringbuf8_t* rb, uint8_t data)
{
    uint16_t next = rb->writeidx + 1;
    if (next >= USART_BUFFER8_SIZE) { next = 0; }
    if (next == rb->readidx) { return false; }
    rb->buffer[rb->writeidx] = data;
    rb->writeidx = next;
    return true;
}

static inline bool rb8_pop(ringbuf8_t* rb, uint8_t* output)
{
    if (rb->readidx == rb->writeidx) {return false;} // Empty
    *output = rb->buffer[rb->readidx];

    uint16_t next = rb->readidx + 1;
    if (next >= USART_BUFFER8_SIZE) {next = 0;}
    rb->readidx = next;
    return true;
}

static inline bool rb9_push(ringbuf9_t* rb, uint16_t data)
{
    data &= 0b000111111111; //Enforce 9-bit width
    uint16_t next = rb->writeidx + 1;
    if (next >= USART_BUFFER9_SIZE) { next = 0; }
    if (next == rb->readidx) { return false; }
    rb->buffer[rb->writeidx] = data;
    rb->writeidx = next;
    return true;
}

static inline bool rb9_pop(ringbuf9_t* rb, uint16_t* output)
{
    if (rb->readidx == rb->writeidx) {return false;} // Empty
    *output = rb->buffer[rb->readidx] & 0b000111111111; // Enforce 9-bit width

    uint16_t next = rb->readidx + 1;
    if (next >= USART_BUFFER8_SIZE) {next = 0;}
    rb->readidx = next;
    return true;
}

bool usart_tx_push(usart_t* u, uint16_t data)
{
    __disable_irq();
    bool succeeded;
    if (u->mode == LENGTH8B) { succeeded = rb8_push((ringbuf9_t*)u->tx, (uint8_t)data); }
    else { succeeded = rb9_push((ringbuf9_t*)u->tx, data); }
    if (succeeded) { u->registers->CR1 |= (1 << 7); } // Enable TXE interrupt so we can handle the new data we just pushed
    __enable_irq();
    return succeeded;
}

bool usart_rx_pop(usart_t* u, uint16_t* output)
{
    if (u->mode == LENGTH8B)
    {
        uint8_t temp;
        if (!rb8_pop((ringbuf8_t*)u->rx, &temp)) { return false; }
        *output = temp;
    }
    else 
    {
        if (!rb9_pop((ringbuf9_t*)u->rx, output)) { return false; }
    }
    return true;
}