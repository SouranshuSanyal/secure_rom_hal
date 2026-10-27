
#ifndef BOOT_FLOW_H
#define BOOT_FLOW_H

#include "platform_status.h"
#include "uart_types.h"

typedef enum
{
    BOOT_STATE_RESET = 0,
    BOOT_STATE_ROM_READY,
    BOOT_STATE_API_READY,
    BOOT_STATE_PERIPHERAL_READY,
    BOOT_STATE_APPLICATION_READY,
    BOOT_STATE_FAILED
} boot_state_t;

platform_status_t BOOT_Run(const uart_config_t *config);

boot_state_t BOOT_GetState(void);

#endif
