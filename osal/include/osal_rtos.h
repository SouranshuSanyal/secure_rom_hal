
#ifndef OSAL_RTOS_H
#define OSAL_RTOS_H

#include "osal.h"

typedef struct
{
    /* RTOS tick frequency, in ticks per second. */
    platform_uint32_t tick_rate_hz;

    /* Delay the current task for the requested number of ticks. */
    platform_status_t (*delay_ticks)(
        platform_uint32_t ticks);

    /* RTOS/processor-specific critical-section hooks. */
    platform_status_t (*critical_enter)(
        platform_uint32_t *state);

    platform_status_t (*critical_exit)(
        platform_uint32_t state);
} osal_rtos_hooks_t;

platform_status_t OSAL_RTOS_GetOps(
    const osal_rtos_hooks_t *hooks,
    osal_ops_t *ops);

#endif
