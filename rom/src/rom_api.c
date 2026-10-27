
#include "rom_api.h"
#include "uart.h"
#include "stdlib.h"

/* ROM service state is independent of UART driver state. */
static rom_state_t rom_state = ROM_STATE_UNINITIALIZED;

const rom_api_table_t g_rom_api_table =
{
    .api_version = ROM_API_VERSION,
    .table_size = (platform_uint32_t)sizeof(rom_api_table_t),

    .uart_init = ROM_UART_Init,
    .uart_send_byte = ROM_UART_SendByte,
    .uart_receive_byte = ROM_UART_ReceiveByte
};

rom_state_t ROM_API_GetState(void)
{
    return rom_state;
}

platform_status_t ROM_API_Init(void)
{
    platform_status_t status;

    if (rom_state == ROM_STATE_INITIALIZED)
    {
        return PLATFORM_STATUS_BUSY;
    }

    status = ROM_API_Validate(&g_rom_api_table);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        return status;
    }

    rom_state = ROM_STATE_INITIALIZED;

    return PLATFORM_STATUS_SUCCESS;
}


platform_status_t ROM_UART_Init(const uart_config_t *config)
{
    if (rom_state != ROM_STATE_INITIALIZED)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (config == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (config->baudrate == 0U)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    return UART_Init(config);
}

platform_status_t ROM_UART_SendByte(platform_uint8_t data)
{
    if (rom_state != ROM_STATE_INITIALIZED)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    return UART_WriteByte(data);
}

platform_status_t ROM_UART_ReceiveByte(platform_uint8_t *data)
{
    if (rom_state != ROM_STATE_INITIALIZED)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (data == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    return UART_ReadByte(data);
}


platform_status_t ROM_API_Validate(const rom_api_table_t *table)
{
    if (table == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (table->api_version != ROM_API_VERSION)
    {
        return PLATFORM_STATUS_NOT_SUPPORTED;
    }

    if (table->table_size < (platform_uint32_t)sizeof(rom_api_table_t))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((table->uart_init == NULL) ||
        (table->uart_send_byte == NULL) ||
        (table->uart_receive_byte == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    return PLATFORM_STATUS_SUCCESS;
}





platform_status_t ROM_API_GetTable(const rom_api_table_t **table)
{
    platform_status_t status;

    if (table == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* Do not expose the table before ROM initialization. */
    *table = NULL;

    if (rom_state != ROM_STATE_INITIALIZED)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    status = ROM_API_Validate(&g_rom_api_table);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        return status;
    }

    *table = &g_rom_api_table;

    return PLATFORM_STATUS_SUCCESS;
}
