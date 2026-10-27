
#include "spi_device.h"

static platform_uint8_t device_registers[4];
static platform_uint8_t current_register;
static platform_bool_t read_operation;
static platform_bool_t write_operation;

void SPI_DeviceReset(void)
{
    device_registers[0] = 0xA5U;
    device_registers[1] = 0x01U;
    device_registers[2] = 0x00U;
    device_registers[3] = 0x42U;

    current_register = 0U;
    read_operation = PLATFORM_FALSE;
    write_operation = PLATFORM_FALSE;
}

void SPI_DeviceChipSelect(platform_bool_t enable)
{
    if (enable == PLATFORM_FALSE)
    {
        current_register = 0U;
        read_operation = PLATFORM_FALSE;
        write_operation = PLATFORM_FALSE;
    }
}

platform_uint8_t SPI_DeviceTransfer(
    platform_uint8_t tx_data)
{
    if (read_operation == PLATFORM_TRUE)
    {
        return device_registers[current_register];
    }

    if (write_operation == PLATFORM_TRUE)
    {
        device_registers[current_register] = tx_data;
        write_operation = PLATFORM_FALSE;
        return 0x00U;
    }

    if ((tx_data & 0x80U) != 0U)
    {
        current_register = tx_data & 0x7FU;
        read_operation = PLATFORM_TRUE;
        write_operation = PLATFORM_FALSE;
        return 0x00U;
    }

    current_register = tx_data & 0x7FU;
    read_operation = PLATFORM_FALSE;
    write_operation = PLATFORM_TRUE;

    return 0x00U;
}
