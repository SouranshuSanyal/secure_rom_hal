#include <stddef.h>

#include "i2c.h"
#include "i2c_device.h"
#include "platform_timeout.h"

static i2c_config_t i2c_config;

static platform_bool_t i2c_initialized =
    PLATFORM_FALSE;

static platform_bool_t i2c_bus_busy =
    PLATFORM_FALSE;

static platform_bool_t i2c_force_nack =
    PLATFORM_FALSE;

static platform_bool_t i2c_force_timeout =
    PLATFORM_FALSE;

platform_status_t I2C_Init(
    const i2c_config_t *config)
{
    if (config == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (config->clock_frequency == 0U)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((config->speed != I2C_SPEED_STANDARD) &&
        (config->speed != I2C_SPEED_FAST))
    {
        i2c_initialized = PLATFORM_FALSE;
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    i2c_config = *config;

    i2c_bus_busy = PLATFORM_FALSE;

    I2C_DeviceReset();

    i2c_initialized = PLATFORM_TRUE;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t I2C_Start(
    platform_uint8_t address,
    platform_bool_t read)
{
    platform_bool_t acknowledged;

    if (address > 0x7FU)
    {
        return PLATFORM_STATUS_INVALID_ADDRESS;
    }

    if (i2c_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (i2c_bus_busy == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_BUSY;
    }

    if (i2c_force_timeout == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_TIMEOUT;
    }

    if (i2c_force_nack == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    acknowledged =
        I2C_DeviceStart(
            address,
            read
        );

    if (acknowledged == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NACK;
    }

    i2c_bus_busy = PLATFORM_TRUE;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t I2C_WriteByte(
    platform_uint8_t data)
{
    platform_bool_t acknowledged;

    if (i2c_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (i2c_bus_busy == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    acknowledged =
        I2C_DeviceWriteByte(data);

    if (acknowledged == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t I2C_ReadByte(
    platform_uint8_t *data)
{
    if (i2c_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (data == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (i2c_bus_busy == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    *data = I2C_DeviceReadByte();

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t I2C_Stop(void)
{
    if (i2c_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (i2c_bus_busy == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    I2C_DeviceStop();

    i2c_bus_busy = PLATFORM_FALSE;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t I2C_WriteRegister(
    platform_uint8_t address,
    platform_uint8_t reg,
    platform_uint8_t data)
{
    platform_status_t status;

    status =
        I2C_Start(
            address,
            PLATFORM_FALSE
        );

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        return status;
    }

    status =
        I2C_WriteByte(reg);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        I2C_Stop();
        return status;
    }

    status =
        I2C_WriteByte(data);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        I2C_Stop();
        return status;
    }

    status = I2C_Stop();

    return status;
}

platform_status_t I2C_ReadRegister(
    platform_uint8_t address,
    platform_uint8_t reg,
    platform_uint8_t *data)
{
    platform_status_t status;

    if (data == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    status =
        I2C_Start(
            address,
            PLATFORM_FALSE
        );

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        return status;
    }

    status =
        I2C_WriteByte(reg);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        I2C_Stop();
        return status;
    }

    status = I2C_Stop();

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        return status;
    }

    status =
        I2C_Start(
            address,
            PLATFORM_TRUE
        );

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        return status;
    }

    status =
        I2C_ReadByte(data);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        I2C_Stop();
        return status;
    }

    status = I2C_Stop();

    return status;
}

platform_status_t I2C_ForceBusBusy(void)
{
    if (i2c_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    i2c_bus_busy = PLATFORM_TRUE;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t I2C_ForceNack(void)
{
    if (i2c_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    i2c_force_nack = PLATFORM_TRUE;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t I2C_ForceTimeout(void)
{
    if (i2c_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    i2c_force_timeout = PLATFORM_TRUE;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t I2C_ResetBus(void)
{
    if (i2c_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    I2C_DeviceStop();

    i2c_bus_busy = PLATFORM_FALSE;

    i2c_force_nack = PLATFORM_FALSE;

    i2c_force_timeout = PLATFORM_FALSE;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t I2C_StartTimeout(
    platform_uint8_t address,
    platform_bool_t read,
    platform_uint32_t timeout)
{
    platform_timeout_t start;
    platform_bool_t acknowledged;

    if (address > 0x7FU)
    {
        return PLATFORM_STATUS_INVALID_ADDRESS;
    }

    if (i2c_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    start = PLATFORM_GetTick();

    while (i2c_bus_busy == PLATFORM_TRUE)
    {
        if (PLATFORM_TimeoutExpired(start, timeout) == PLATFORM_TRUE)
        {
            return PLATFORM_STATUS_TIMEOUT;
        }

        PLATFORM_DelayTicks(1U);
    }

    if (i2c_force_timeout == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_TIMEOUT;
    }

    if (i2c_force_nack == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    acknowledged = I2C_DeviceStart(address, read);

    if (acknowledged == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    i2c_bus_busy = PLATFORM_TRUE;

    return PLATFORM_STATUS_SUCCESS;
}