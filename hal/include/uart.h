#ifndef UART_H
#define UART_H

#include "uart_types.h"
#include "platform_status.h"

platform_status_t UART_Init(
    const uart_config_t *config
);

platform_status_t UART_WriteByte(
    platform_uint8_t data
);

platform_status_t UART_ReadByte(
    platform_uint8_t *data
);

platform_status_t UART_WriteString(
    const char *string
);

platform_status_t UART_GetStatus(void);

platform_status_t UART_InjectRxByte(
    platform_uint8_t data
);

platform_status_t UART_WriteByteTimeout(
    platform_uint8_t data,
    platform_uint32_t timeout
);

platform_status_t UART_ForceTxBusy(void);

platform_status_t UART_GetRxBufferStatus(void);

#endif  /* UART_H */