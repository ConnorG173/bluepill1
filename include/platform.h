#ifndef PLATFORM_H
#define PLATFORM_H

/* Standard types */
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define NULL (void *)0 //Just need NULL, not all of <stdlib.h>

/* STM32F103 device header */
#include "stm32f103xb.h"

/* CMSIS core */
#include "core_cm3.h"



#endif