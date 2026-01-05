#include "platform.h"
#include "usart_stm32f103.h"
#include "clocks_stm32f103.h"


// Declare usart objects
static usart_t usart1_t = { .id = USART_1 };
static usart_t usart2_t = { .id = USART_2 };
static usart_t usart3_t = { .id = USART_3 };

// Pointers for API access
usart_t* usart1 = &usart1_t;
usart_t* usart2 = &usart2_t;
usart_t* usart3 = &usart3_t;


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


static void set_USART_BRR(USART_NUMBER usart_number, ui32 baud_rate)
{
    /* 
    baud = pclk / 16 * USARTDIV 
    USARTDIV is an unsigned fixed point number that is coded on the USART_BRR register.
    USART_BRR: bits [31:16] -> reserved, [15:4] -> USART_DIV Mantissa, [3:0] -> USART_DIV Fraction (ex. [3:0] = 0011 -> 3/16)
    */
    ui32 pclk = 1;
    if (usart_number == USART_1) {
        pclk = get_pclk2();
    }
    else {
        pclk = get_pclk1();
    }
    ui32 USART_DIV = (pclk + baud_rate/2) / baud_rate; //The fixed point shift will do the /16 for us
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
                USART1->CR1 |= ((1 << 10) | ((ui8)parity_selection_odd << 9));
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
                USART2->CR1 |= ((1 << 10) | ((ui8)parity_selection_odd << 9));
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
                USART3->CR1 |= ((1 << 10) | ((ui8)parity_selection_odd << 9));
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
        default:
        {
            __asm__("NOP");
        }
    }
}


bool usart_config(usart_t* u, usart_wordlength_t mode, ui32 baud_rate_bps, bool clock_used, bool parity_used, bool parity_selection_odd)
// You can call this multiple times to re-configure a usart channel to new settings
{
    USART_NUMBER usart_number = u->id;

    config_usart_clocks(usart_number);
    config_usart_pins(usart_number, clock_used);
    set_USART_BRR(usart_number, baud_rate_bps);
    set_basic_configs(usart_number, parity_used, parity_selection_odd); //Also clears rest of CR1
    set_word_length(usart_number, mode);

    if (usart_number != USART_1 && usart_number != USART_2 && usart_number != USART_3) {
        return false;
    }

    switch (usart_number)
    {
        case USART_1:
        {   
            NVIC_SetPriority(USART1_IRQn, USART1_IRQ_PRIO);
            NVIC_EnableIRQ(USART1_IRQn);
            if (mode == LENGTH8B)
            {
            static ringbuf8_t ringbuf1tx;
            static ringbuf8_t ringbuf1rx;
            usart1_t.registers = USART1;
            usart1_t.tx = &ringbuf1tx;
            usart1_t.rx = &ringbuf1rx;
            usart1_t.mode = mode;
            }
            else
            {
            static ringbuf9_t ringbuf1tx;
            static ringbuf9_t ringbuf1rx;
            usart1_t.registers = USART1;
            usart1_t.tx = &ringbuf1tx;
            usart1_t.rx = &ringbuf1rx;
            usart1_t.mode = mode;
            }
            return true;
        }
        case USART_2:
        {   
            NVIC_SetPriority(USART2_IRQn, USART2_IRQ_PRIO);
            NVIC_EnableIRQ(USART2_IRQn);
            if (mode == LENGTH8B)
            {
            static ringbuf8_t ringbuf2tx;
            static ringbuf8_t ringbuf2rx;
            usart2_t.registers = USART2;
            usart2_t.tx = &ringbuf2tx;
            usart2_t.rx = &ringbuf2rx;
            usart2_t.mode = mode;
            }
            else
            {
            static ringbuf9_t ringbuf2tx;
            static ringbuf9_t ringbuf2rx;
            usart2_t.registers = USART2;
            usart2_t.tx = &ringbuf2tx;
            usart2_t.rx = &ringbuf2rx;
            usart2_t.mode = mode;
            }
            return true;
        }
        case USART_3:
        {   
            NVIC_SetPriority(USART3_IRQn, USART3_IRQ_PRIO);
            NVIC_EnableIRQ(USART3_IRQn);
            if (mode == LENGTH8B)
            {
            static ringbuf8_t ringbuf3tx;
            static ringbuf8_t ringbuf3rx;
            usart3_t.registers = USART3;
            usart3_t.tx = &ringbuf3tx;
            usart3_t.rx = &ringbuf3rx;
            usart3_t.mode = mode;
            }
            else
            {
            static ringbuf9_t ringbuf3tx;
            static ringbuf9_t ringbuf3rx;
            usart3_t.registers = USART3;
            usart3_t.tx = &ringbuf3tx;
            usart3_t.rx = &ringbuf3rx;
            usart3_t.mode = mode;
            }
            return true;
        }
    }
}


bool set_stop_bits(usart_t* u, ui32 stopBits_timesTen)
// Reconfigures # of stop bits for already configured USART
{
    const ui8 stopbitsmasks[4] = {0b01, 0b00, 0b11, 0b10}; //{0.5, 1, 1.5, 2} stop bits
    ui8 stopbitsmask = 0b00;
    if (stopBits_timesTen != 5 || stopBits_timesTen != 10 || stopBits_timesTen != 15 || stopBits_timesTen != 20)
    { return false; } //Error
    stopbitsmask = stopbitsmasks[((stopBits_timesTen / 5) - 1)];
    u->registers->CR2 &= ~(0b11 << 12); //reset existing stop bits
    u->registers->CR2 |= (stopbitsmask << 12);
    return true;
}


static inline bool rb8_push(ringbuf8_t* rb, ui8 data) //Returns false if buffer full
{
    ui16 next = rb->writeidx + 1;
    if (next >= USART_BUFFER8_SIZE) { next = 0; }
    if (next == rb->readidx) 
    { 
        rb->overflowflag = 1;
        return false; 
    }
    rb->buffer[rb->writeidx] = data;
    rb->writeidx = next;
    return true;
}


static inline bool rb8_pop(ringbuf8_t* rb, ui8* output) //Returns false if buffer empty
{
    if (rb->readidx == rb->writeidx) {return false;} // Empty
    *output = rb->buffer[rb->readidx];

    ui16 next = rb->readidx + 1;
    if (next >= USART_BUFFER8_SIZE) {next = 0;}
    rb->readidx = next;
    return true;
}


static inline bool rb9_push(ringbuf9_t* rb, ui16 data)
{
    data &= 0b000111111111; //Enforce 9-bit width
    ui16 next = rb->writeidx + 1;
    if (next >= USART_BUFFER9_SIZE) { next = 0; }
    if (next == rb->readidx) 
    { 
        rb->overflowflag = 1;
        return false; 
    }
    rb->buffer[rb->writeidx] = data;
    rb->writeidx = next;
    return true;
}


static inline bool rb9_pop(ringbuf9_t* rb, ui16* output)
{
    if (rb->readidx == rb->writeidx) {return false;} // Empty
    *output = rb->buffer[rb->readidx] & 0b000111111111; // Enforce 9-bit width

    ui16 next = rb->readidx + 1;
    if (next >= USART_BUFFER8_SIZE) {next = 0;}
    rb->readidx = next;
    return true;
}


bool tx_buffer_full(usart_t* u)
{
    usart_wordlength_t mode = u->mode;
    ui16 next;
    ui16 readidx;
    if (mode == LENGTH8B) 
    { 
        next = ((ringbuf8_t*)u->tx)->writeidx + 1;
        if (next >= USART_BUFFER8_SIZE) { next = 0; }
        readidx = ((ringbuf8_t*)u->tx)->readidx;
    }
    else 
    { 
        next = ((ringbuf9_t*)u->tx)->writeidx + 1;
        if (next >= USART_BUFFER9_SIZE) { next = 0; }
        readidx = ((ringbuf9_t*)u->tx)->readidx;
    }
    if (next == readidx) { return true; }
    return false;
}


bool rx_buffer_full(usart_t* u)
{
    usart_wordlength_t mode = u->mode;
    ui16 next;
    ui16 readidx;
    if (mode == LENGTH8B) 
    { 
        next = ((ringbuf8_t*)u->rx)->writeidx + 1;
        if (next >= USART_BUFFER8_SIZE) { next = 0; }
        readidx = ((ringbuf8_t*)u->rx)->readidx;
    }
    else 
    { 
        next = ((ringbuf9_t*)u->rx)->writeidx + 1;
        if (next >= USART_BUFFER9_SIZE) { next = 0; }
        readidx = ((ringbuf9_t*)u->rx)->readidx;
    }
    if (next == readidx) { return true; }
    return false;
}


bool usart_tx_push(usart_t* u, ui16 data)
{
    __disable_irq();
    bool succeeded;
    if (u->mode == LENGTH8B) { succeeded = rb8_push((ringbuf8_t*)u->tx, (ui8)data); }
    else { succeeded = rb9_push((ringbuf9_t*)u->tx, data); }
    if (succeeded) { u->registers->CR1 |= (1 << 7); } // Enable TXEIE so we can handle the new data we just pushed
    __enable_irq();
    return succeeded;
}


bool usart_rx_pop(usart_t* u, ui16* output)
{
    if (u->mode == LENGTH8B)
    {
        ui8 temp;
        if (!rb8_pop((ringbuf8_t*)u->rx, &temp)) { return false; }
        *output = temp;
    }
    else 
    {
        if (!rb9_pop((ringbuf9_t*)u->rx, output)) { return false; }
    }
    return true;
}

void usart_irq_handler(usart_t* u)
{
    bool done_rx = false;
    if (u->registers->SR & 0b1111) // Error bits held in SR[3:0] - One check to see if there's errors at all
    {
        if (u->registers->SR & 0b1) { u->errors.parity_err_cnt++; }
        if (u->registers->SR & 0b10) { u->errors.framing_err_cnt++; }
        if (u->registers->SR & 0b100) { u->errors.noise_err_cnt++; }
        if (u->registers->SR & 0b1000) { u->errors.overrun_err_cnt++; }
        (void)(u->registers->DR); // read DR to clear errors
        done_rx = true;
    }

    // RX Side
    if (((u->registers->SR >> 5) & 0b1) && !done_rx) //Read RXNE if there werent errors
    {
        ui16 data = (ui16)(u->registers->DR & 0b000111111111U); // read DR, cap to 9 bits
        if (u->mode == LENGTH8B)
        {
            ui8 data8 = (ui8)(data & 0xFFU); // Cast to 8 bits
            rb8_push(u->rx, data8);
        }
        else
        {
            rb9_push(u->rx, data);
        }
    }

    // TX Side
    if (((u->registers->SR >> 7) & 0b1) && ((u->registers->CR1 >> 7) & 0b1)) // If TXE and TXEIE
    {
        if (u->mode == LENGTH8B)
        {
            ui8 output8;
            if (rb8_pop((ringbuf8_t*)u->tx, &output8)) { u->registers->DR = output8; } //pop helper returns true and runs if it has stuff
            else { u->registers->CR1 &= ~(1 << 7); } // If false -> tx buffer empty. Disable TXEIE 
        }
        else
        {
            ui16 output9;
            if (rb9_pop((ringbuf9_t*)u->tx, &output9)) { u->registers->DR = (output9 & 0x1FF); } // 9 bits
            else { u->registers->CR1 &= ~(1 << 7); }
        }
    }
}

void USART1_IRQHandler(void)
{
    usart_irq_handler(&usart1_t);
}

void USART2_IRQHandler(void)
{
    usart_irq_handler(&usart2_t);
}

void USART3_IRQHandler(void)
{
    usart_irq_handler(&usart3_t);
}