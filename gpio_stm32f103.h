#ifndef GPIO_STM32F103_H
#define GPIO_STM32F103_H
#include <string.h>
#include <stdbool.h>


  enum GPIO_MODES { INPUT, OUTPUT_10MHZ, OUTPUT_2MHZ, OUTPUT_50MHZ };
  enum GPIO_CONFIGS { ANALOG, FLOATING, PULLUP_PULLDOWN, RESERVED_DONOTUSE, GP_PUSH_PULL, GP_OPEN_DRAIN, AF_PUSH_PULL, AF_OPEN_DRAIN };
  unsigned int* get_gpio_cr_base(char port);
  void configure_gpio_pin(char port, int pin, enum GPIO_CONFIGS, enum GPIO_MODES);
  void drive_pin(char port, int pin, bool onoff);




#endif