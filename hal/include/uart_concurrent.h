#ifndef UART_CONCURRENT_H
#define UART_CONCURRENT_H

#include "platform_types.h"
#include "platform_status.h"

/*
 * Task-context APIs for synchronized access to the UART RX buffer.
 *
 * OSAL must be initialized before these functions are called.
 */
platform_status_t UART_CONCURRENT_ReadByte(
    platform_uint8_t *data);

platform_status_t UART_CONCURRENT_InjectRxByte(
    platform_uint8_t data);

#endif /* UART_CONCURRENT_H */