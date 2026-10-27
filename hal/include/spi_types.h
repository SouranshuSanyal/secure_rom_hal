#ifndef SPI_TYPES_H
#define SPI_TYPES_H

#include "platform_types.h"

typedef enum
{
    SPI_MODE_0 = 0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3
} spi_mode_t;

typedef enum
{
    SPI_BIT_ORDER_MSB_FIRST = 0,
    SPI_BIT_ORDER_LSB_FIRST
} spi_bit_order_t;

typedef struct
{
    platform_uint32_t clock_frequency;
    spi_mode_t mode;
    spi_bit_order_t bit_order;
} spi_config_t;

#endif /* SPI_TYPES_H */