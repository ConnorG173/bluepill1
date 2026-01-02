
#ifndef USART_STM32F103_H
#define USART_STM32F103_H

#include "platform.h"

#define USART_BUFFER8_SIZE 128U // size in bytes = USART_BUFFER8_SIZE * sizeof(ui8) bytes
#define USART_BUFFER9_SIZE 128U // size in bytes = USART_BUFFER9_SIZE * sizeof(ui16) bytes

#define USART1_IRQ_PRIO 8U // Range 0-15
#define USART2_IRQ_PRIO 8U // Range 0-15
#define USART3_IRQ_PRIO 8U // Range 0-15

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
    ui8 buffer[USART_BUFFER8_SIZE];
    volatile ui32 overflowflag;
    volatile ui16 writeidx; // Index to write at
    volatile ui16 readidx; // Index to read from
} ringbuf8_t;

typedef struct ringbuf9 {
    ui16 buffer[USART_BUFFER9_SIZE];
    volatile ui32 overflowflag;
    volatile ui16 writeidx; // Index to write at
    volatile ui16 readidx; // Index to read from
} ringbuf9_t;

typedef struct usart_err_cnt {
    volatile ui32 overrun_err_cnt;
    volatile ui32 noise_err_cnt;
    volatile ui32 framing_err_cnt;
    volatile ui32 parity_err_cnt;
} usart_err_cnt_t;

typedef struct usart {
    USART_NUMBER id;
    USART_TypeDef *registers; // Should point to USARTx | ex. USART1
    void *tx; // Transmit ring buffer
    void *rx; // Recieve ring buffer
    usart_wordlength_t mode;
    usart_err_cnt_t errors;
} usart_t;

// Usart API Access Pointers
extern usart_t* usart1;
extern usart_t* usart2;
extern usart_t* usart3;

bool usart_config(usart_t* u, usart_wordlength_t mode, ui32 baud_rate_bps, bool clock_used, bool parity_used, bool parity_selection_odd);

bool usart_tx_push(usart_t* u, ui16 data);
bool usart_rx_pop(usart_t* u, ui16* output);

#endif