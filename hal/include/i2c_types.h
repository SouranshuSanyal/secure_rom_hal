#ifndef I2C_TYPES_H
#define I2C_TYPES_H

#include "platform_types.h"

typedef enum
{
    I2C_SPEED_STANDARD = 0,
    I2C_SPEED_FAST
} i2c_speed_t;

typedef struct
{
    platform_uint32_t clock_frequency;
    i2c_speed_t speed;
} i2c_config_t;

typedef enum
{
    I2C_STATUS_IDLE = 0,
    I2C_STATUS_BUSY,
    I2C_STATUS_NACK,
    I2C_STATUS_TIMEOUT
} i2c_status_t;

#endif /* I2C_TYPES_H */