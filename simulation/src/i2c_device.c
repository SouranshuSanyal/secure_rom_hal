#include "i2c_device.h"

#define I2C_DEVICE_ADDRESS    0x50U

static platform_uint8_t device_registers[5];

static platform_uint8_t current_register;

static platform_bool_t device_selected;

static platform_bool_t read_operation;

static platform_bool_t register_selected;

void I2C_DeviceReset(void)
{
    device_registers[0] = 0x42U;
    device_registers[1] = 0x01U;
    device_registers[2] = 0x00U;
    device_registers[3] = 0x19U;
    device_registers[4] = 0x80U;

    current_register = 0U;

    device_selected = PLATFORM_FALSE;

    read_operation = PLATFORM_FALSE;

    register_selected = PLATFORM_FALSE;
}

platform_bool_t I2C_DeviceStart(
    platform_uint8_t address,
    platform_bool_t read)
{
    if (address != I2C_DEVICE_ADDRESS)
    {
        return PLATFORM_FALSE;
    }

    device_selected = PLATFORM_TRUE;

    read_operation = read;

    register_selected = PLATFORM_FALSE;

    return PLATFORM_TRUE;
}

platform_bool_t I2C_DeviceWriteByte(
    platform_uint8_t data)
{
    if (device_selected == PLATFORM_FALSE)
    {
        return PLATFORM_FALSE;
    }

    if (read_operation == PLATFORM_TRUE)
    {
        return PLATFORM_FALSE;
    }

    if (register_selected == PLATFORM_FALSE)
    {
        if (data >= 0x05U)
        {
            return PLATFORM_FALSE;
        }

        current_register = data;

        register_selected = PLATFORM_TRUE;

        return PLATFORM_TRUE;
    }

    device_registers[current_register] = data;

    return PLATFORM_TRUE;
}

platform_uint8_t I2C_DeviceReadByte(void)
{
    if (device_selected == PLATFORM_FALSE)
    {
        return 0xFFU;
    }

    if (read_operation == PLATFORM_FALSE)
    {
        return 0xFFU;
    }

    return device_registers[current_register];
}

void I2C_DeviceStop(void)
{
    device_selected = PLATFORM_FALSE;

    read_operation = PLATFORM_FALSE;

    register_selected = PLATFORM_FALSE;
}