#ifndef GPIO_H
#define GPIO_H

#include "gpio_types.h"
#include "platform_status.h"

platform_status_t GPIO_Init(
    platform_uint32_t pin,
    gpio_direction_t gpio_direction_t
);

platform_status_t GPIO_Write(
    platform_uint32_t pin,
    gpio_pin_level_t level
);

platform_status_t GPIO_Read(
    platform_uint32_t pin,
    gpio_pin_level_t *level
);

platform_status_t GPIO_Toggle(
    platform_uint32_t pin
);

#endif  /* GPIO_H */