#ifndef SPI_DEVICE_H
#define SPI_DEVICE_H

#include "platform_types.h"

void SPI_DeviceReset(void);
void SPI_DeviceChipSelect(platform_bool_t enable);

platform_uint8_t SPI_DeviceTransfer(
    platform_uint8_t tx_data
);

#endif /* SPI_DEVICE_H */