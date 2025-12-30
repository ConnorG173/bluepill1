
#ifndef USART_STM32F103_H
#define USART_STM32F103_H

#include "platform.h"

#define USART_BUFFER8_SIZE 128U // 128 uint8_t
#define USART_BUFFER9_SIZE 128U // 128 uint16_t

typedef enum
{
    USART_1,
    USART_2,
    USART_3
} USART_NUMBER;

typedef enum
{
    LENGTH8B,
    LENGTH9B,
} usart_wordlength_t;

typedef struct ringbuf8 {
    uint8_t buffer[USART_BUFFER8_SIZE];
    volatile uint16_t writeidx; // Index to write at
    volatile uint16_t readidx; // Index to read from
} ringbuf8_t;

typedef struct ringbuf9 {
    uint16_t buffer[USART_BUFFER9_SIZE];
    volatile uint16_t writeidx; // Index to write at
    volatile uint16_t readidx; // Index to read from
} ringbuf9_t;

typedef struct usart {
    USART_TypeDef *registers; // Should point to base of USARTx | ex. USART1_BASE
    void *tx; // Transmit ring buffer
    void *rx; // Recieve ring buffer
    usart_wordlength_t mode;
} usart_t;

usart_t* usart_config(USART_NUMBER usart_number, usart_wordlength_t mode, uint32_t baud_rate_bps, bool clock_used, bool parity_used, bool parity_selection_odd); 

bool usart_tx_push(usart_t* u, uint16_t data);
bool usart_rx_pop(usart_t* u, uint16_t* output);

#endif