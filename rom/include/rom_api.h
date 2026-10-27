
#ifndef ROM_API_H
#define ROM_API_H

#include "platform_status.h"
#include "platform_types.h"
#include "uart_types.h"

#define ROM_API_VERSION_1  1U
#define ROM_API_VERSION    ROM_API_VERSION_1

typedef struct
{
    platform_uint32_t api_version;
    platform_uint32_t table_size;

    platform_status_t (*uart_init)(const uart_config_t *config);
    platform_status_t (*uart_send_byte)(platform_uint8_t data);
    platform_status_t (*uart_receive_byte)(platform_uint8_t *data);

} rom_api_table_t;

platform_status_t ROM_API_GetTable(
    const rom_api_table_t **table);

/* Validate API version, size, and required function pointers. */
platform_status_t ROM_API_Validate(const rom_api_table_t *table);

platform_status_t ROM_UART_Init(const uart_config_t *config);

platform_status_t ROM_UART_SendByte(platform_uint8_t data);

platform_status_t ROM_UART_ReceiveByte(platform_uint8_t *data);


typedef enum
{
    ROM_STATE_UNINITIALIZED = 0,
    ROM_STATE_INITIALIZED
} rom_state_t;

/* Initialize the ROM service layer. */
platform_status_t ROM_API_Init(void);

/* Query the current ROM service state. */
rom_state_t ROM_API_GetState(void);


#endif /* ROM_API_H */
