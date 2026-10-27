#include "gpio.h"
#include <stddef.h>
#include "platform_register.h"

static platform_uint32_t gpio_data_register;
static platform_int32_t gpio_direction_register;

platform_status_t GPIO_Write(platform_uint32_t pin, gpio_pin_level_t level)
{
    if (pin >= 32U)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((level != GPIO_PIN_LOW) && (level != GPIO_PIN_HIGH))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (level == GPIO_PIN_HIGH)
    {
        gpio_data_register =
            PLATFORM_RegisterSetBits32(gpio_data_register, (1U << pin));
    }
    else
    {
        gpio_data_register =
            PLATFORM_RegisterClearBits32(gpio_data_register, (1U << pin));
    }

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t GPIO_Read(
    platform_uint32_t pin,
    gpio_pin_level_t *level
){
    if (pin >= 32U || level == NULL){
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((gpio_data_register & (1U << pin)) != 0U){
        *level = GPIO_PIN_HIGH;
    }
    else {
        *level = GPIO_PIN_LOW;
    }
    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t GPIO_Toggle(
    platform_uint32_t pin
){
    if (pin >= 32U){
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    gpio_data_register = PLATFORM_RegisterToggleBits32(
        gpio_data_register, (1U << pin)
    );

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t GPIO_Init(
    platform_uint32_t pin,
    gpio_direction_t direction
){
    if (pin >= 32U){
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((direction != GPIO_INPUT) &&
    (direction != GPIO_OUTPUT))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (direction == GPIO_OUTPUT)
    {
        gpio_direction_register =
            (platform_int32_t)PLATFORM_RegisterSetBits32(
                (platform_uint32_t)gpio_direction_register,
                (1U << pin));
    }
    else
    {
        gpio_direction_register =
            (platform_int32_t)PLATFORM_RegisterClearBits32(
                (platform_uint32_t)gpio_direction_register,
                (1U << pin));
    }

    return PLATFORM_STATUS_SUCCESS;
}