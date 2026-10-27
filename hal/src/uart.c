#include "uart.h"
#include "platform_register.h"
#include "stdlib.h"
#include "stdio.h"
#include "platform_timeout.h"
#include "platform_ring_buffer.h"

#define UART_STATUS_TX_EMPTY_MASK   (1U << 0)
#define UART_STATUS_RX_READY_MASK   (1U << 1)

static platform_uint32_t uart_data_register;
static platform_uint32_t uart_status_register;
static platform_uint32_t uart_control_register;
static platform_ring_buffer_t uart_rx_buffer;
static uart_config_t uart_config;
static platform_bool_t uart_initialized = PLATFORM_FALSE;


platform_status_t UART_Init(
    const uart_config_t *config
){
    if (config == NULL){
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }
    if (config-> baudrate == 0U){
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((config->parity != UART_PARITY_NONE) &&
    (config->parity != UART_PARITY_EVEN) &&
    (config->parity != UART_PARITY_ODD))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((config->stop_bits != UART_STOP_BITS_1) &&
        (config->stop_bits != UART_STOP_BITS_2))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    RING_BUFFER_Init(&uart_rx_buffer);
    uart_config = *config;
    uart_control_register = 0U;
    uart_initialized = PLATFORM_TRUE;
    uart_status_register = UART_STATUS_TX_EMPTY_MASK;


    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t UART_WriteByte(
    platform_uint8_t data
){
    if(uart_initialized == PLATFORM_FALSE){
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if((uart_status_register & UART_STATUS_TX_EMPTY_MASK) == 0U){
        return PLATFORM_STATUS_BUSY;
    }

    uart_data_register = data;

    uart_status_register |= UART_STATUS_TX_EMPTY_MASK;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t UART_WriteString(
    const char *string
){
    if (uart_initialized == PLATFORM_FALSE){
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (string == NULL){
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    while (*string != '\0'){
        UART_WriteByte((platform_uint8_t)*string);
        string++;
    }

    return PLATFORM_STATUS_SUCCESS;
}

// platform_status_t UART_ReadByte( platform_uint8_t *data){
//     if (uart_initialized == PLATFORM_FALSE){
//         return PLATFORM_STATUS_NOT_INITIALIZED;
//     }

//     if (data == NULL){
//         return PLATFORM_STATUS_INVALID_ARGUMENT;
//     }

//     if ((uart_status_register & UART_STATUS_RX_READY_MASK) == 0U)
//     {
//         return PLATFORM_STATUS_BUSY;
//     }

//     *data = (platform_uint8_t) uart_data_register;

//     uart_status_register &= ~UART_STATUS_RX_READY_MASK;

//     return PLATFORM_STATUS_SUCCESS;
// }

platform_status_t UART_ReadByte(platform_uint8_t *data)
{
    if (uart_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (data == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    return RING_BUFFER_Pop(&uart_rx_buffer, data);
}

platform_status_t UART_GetStatus(void){
    if ((uart_status_register & 0x01U) != 0U){
        return UART_STATUS_RX_READY;
    }

    return UART_STATUS_TX_EMPTY;
}


// platform_status_t UART_InjectRxByte(
//     platform_uint8_t data
// ){
//     if (uart_initialized == PLATFORM_FALSE){
//         return PLATFORM_STATUS_NOT_INITIALIZED;
//     }

//     uart_data_register = data;
//     uart_status_register |= UART_STATUS_RX_READY_MASK;

//     return PLATFORM_STATUS_SUCCESS;
// }

platform_status_t UART_InjectRxByte(platform_uint8_t data)
{
    if (uart_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    return RING_BUFFER_Push(&uart_rx_buffer, data);
}

platform_status_t UART_WriteByteTimeout(
    platform_uint8_t data,
    platform_uint32_t timeout
){
    platform_uint32_t count = 0U;

    if (uart_initialized == PLATFORM_FALSE){
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    while ((uart_status_register & UART_STATUS_TX_EMPTY_MASK) == 0U){
        if (count >= timeout){
            return PLATFORM_STATUS_TIMEOUT;
        }

        count++;
    }

    uart_data_register = data;

    uart_status_register |= UART_STATUS_TX_EMPTY_MASK;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t UART_ForceTxBusy(void){
    if (uart_initialized == PLATFORM_FALSE){
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    uart_status_register &= ~UART_STATUS_TX_EMPTY_MASK;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t UART_GetRxBufferStatus(void)
{
    if (uart_initialized == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (RING_BUFFER_IsEmpty(&uart_rx_buffer) == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_BUSY;
    }

    return PLATFORM_STATUS_SUCCESS;
}