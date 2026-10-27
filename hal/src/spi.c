#include "spi.h"
#include <stddef.h>
#include "platform_register.h"
#include "spi_device.h"

#define SPI_STATUS_TX_EMPTY_MASK   (1U << 0)
#define SPI_STATUS_RX_READY_MASK   (1U << 1)
#define SPI_STATUS_BUSY_MASK       (1U << 2)

static platform_uint32_t spi_data_register;
static platform_uint32_t spi_status_register;
static platform_uint32_t spi_control_register;

static platform_bool_t spi_chip_select;
static platform_bool_t spi_initialized = PLATFORM_FALSE;
static spi_config_t spi_config;

platform_status_t SPI_Init(const spi_config_t *config)
{
    if (config == NULL)
    {
        spi_initialized = PLATFORM_FALSE;
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (config->clock_frequency == 0U)
    {
        spi_initialized = PLATFORM_FALSE;
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((config->mode != SPI_MODE_0) &&
        (config->mode != SPI_MODE_1) &&
        (config->mode != SPI_MODE_2) &&
        (config->mode != SPI_MODE_3))
    {
        spi_initialized = PLATFORM_FALSE;
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((config->bit_order != SPI_BIT_ORDER_MSB_FIRST) &&
        (config->bit_order != SPI_BIT_ORDER_LSB_FIRST))
    {
        spi_initialized = PLATFORM_FALSE;
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* Commit configuration only after all validation succeeds. */
    SPI_DeviceReset();

    spi_config = *config;

    spi_control_register = 0U;
    spi_status_register = 0U;
    spi_chip_select = PLATFORM_FALSE;

    spi_initialized = PLATFORM_TRUE;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t SPI_ChipSelect(
    platform_bool_t enable)
{
    if (spi_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    spi_chip_select = enable;
    SPI_DeviceChipSelect(enable);

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t SPI_TransferByte(
    platform_uint8_t tx_data,
    platform_uint8_t *rx_data)
{
    if (spi_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (rx_data == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (spi_chip_select == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    spi_status_register |= SPI_STATUS_BUSY_MASK;

    spi_data_register = tx_data;

    /*
     * Simulated slave response.
     * For now, loop back the transmitted byte.
     */

    *rx_data =
        SPI_DeviceTransfer(tx_data);

    spi_status_register &= ~SPI_STATUS_BUSY_MASK;

    spi_status_register |= SPI_STATUS_TX_EMPTY_MASK;

    return PLATFORM_STATUS_SUCCESS;
}


platform_status_t SPI_Transfer(
    const platform_uint8_t *tx_buffer,
    platform_uint8_t *rx_buffer,
    platform_uint32_t length)
{
    platform_uint32_t i;
    platform_status_t status;

    if (spi_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (tx_buffer == NULL ||
        rx_buffer == NULL ||
        length == 0U)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    for (i = 0U; i < length; i++)
    {
        status =
            SPI_TransferByte(
                tx_buffer[i],
                &rx_buffer[i]
            );

        if (status != PLATFORM_STATUS_SUCCESS)
        {
            return status;
        }
    }

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t SPI_ReadRegister(
    platform_uint8_t address,
    platform_uint8_t *data)
{
    platform_uint8_t command;
    platform_uint8_t dummy;
    platform_status_t status;

    if (spi_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (data == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (address >= 0x04U)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    command = 0x80U | address;

    SPI_ChipSelect(PLATFORM_TRUE);

    status =
        SPI_TransferByte(
            command,
            &dummy
        );

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        SPI_ChipSelect(PLATFORM_FALSE);
        return status;
    }

    status =
        SPI_TransferByte(
            0x00U,
            data
        );

    SPI_ChipSelect(PLATFORM_FALSE);

    return status;
}

platform_status_t SPI_WriteRegister(
    platform_uint8_t address,
    platform_uint8_t data)
{
    platform_uint8_t command;
    platform_uint8_t dummy;
    platform_status_t status;

    if (spi_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (address >= 0x04U)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    command = address & 0x7FU;

    SPI_ChipSelect(PLATFORM_TRUE);

    status =
        SPI_TransferByte(
            command,
            &dummy
        );

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        SPI_ChipSelect(PLATFORM_FALSE);
        return status;
    }

    status =
        SPI_TransferByte(
            data,
            &dummy
        );

    SPI_ChipSelect(PLATFORM_FALSE);

    return status;
}