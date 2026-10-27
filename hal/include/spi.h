#ifndef SPI_H
#define SPI_H

#include "spi_types.h"
#include "platform_status.h"
#include "stdint.h"

platform_status_t SPI_Init(
    const spi_config_t *config
);

platform_status_t SPI_TransferByte(
    platform_uint8_t tx_data,
    platform_uint8_t *rx_data
);

platform_status_t SPI_Transfer(
    const platform_uint8_t *tx_buffer,
    platform_uint8_t *rx_buffer,
    platform_uint32_t length
);

platform_status_t SPI_ChipSelect(
    platform_bool_t enable
);

platform_status_t SPI_WriteRegister(
    platform_uint8_t address,
    platform_uint8_t data
);

platform_status_t SPI_ReadRegister(
    platform_uint8_t address,
    platform_uint8_t *data
);

#endif /* SPI_H */