#ifndef UPRINTF_H
#define UPRINTF_H

#include <stdarg.h>
#include "usart_stm32f103.h"

#define UPRINTF_MAX_CHARS 128U

int uprintf(usart_t* u, const char* fmt, ...);

#endif