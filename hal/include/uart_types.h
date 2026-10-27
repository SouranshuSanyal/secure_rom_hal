#ifndef UART_TYPES_H
#define UART_TYPES_H

#include "platform_types.h"

typedef enum{
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN,
    UART_PARITY_ODD
} uart_parity_t;

typedef enum{
    UART_STOP_BITS_1 = 0,
    UART_STOP_BITS_2
} uart_stop_bits_t;

typedef struct{
    platform_uint32_t baudrate;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
}uart_config_t;


typedef enum{
    UART_STATUS_TX_EMPTY = 0,
    UART_STATUS_TX_BUSY,
    UART_STATUS_RX_READY
} uart_status_t;




#endif  /* UART_TYPES_H */