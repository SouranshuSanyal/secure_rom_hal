#ifndef I2C_H
#define I2C_H

#include "i2c_types.h"
#include "platform_status.h"

platform_status_t I2C_Init(
    const i2c_config_t *config
);

platform_status_t I2C_Start(
    platform_uint8_t address,
    platform_bool_t read
);

platform_status_t I2C_WriteByte(
    platform_uint8_t data
);

platform_status_t I2C_ReadByte(
    platform_uint8_t *data
);

platform_status_t I2C_Stop(void);

platform_status_t I2C_WriteRegister(
    platform_uint8_t address,
    platform_uint8_t reg,
    platform_uint8_t data
);

platform_status_t I2C_ReadRegister(
    platform_uint8_t address,
    platform_uint8_t reg,
    platform_uint8_t *data
);

platform_status_t I2C_StartTimeout(
    platform_uint8_t address,
    platform_bool_t read,
    platform_uint32_t timeout
);

platform_status_t I2C_ForceBusBusy(void);

platform_status_t I2C_ForceNack(void);

platform_status_t I2C_ForceTimeout(void);



platform_status_t I2C_ResetBus(void);

#endif /* I2C_H */