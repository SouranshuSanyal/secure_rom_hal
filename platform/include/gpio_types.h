#ifndef GPIO_TYPES_H
#define GPIO_TYPES_H

#include "platform_types.h"

typedef enum{
    GPIO_PIN_LOW = 0,
    GPIO_PIN_HIGH = 1
} gpio_pin_level_t;

typedef enum{
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1
}gpio_direction_t;



#endif  /* GPIO_TYPES_H */