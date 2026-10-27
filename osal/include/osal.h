
#ifndef OSAL_H
#define OSAL_H

#include "platform_types.h"
#include "platform_status.h"

/*
 * Platform-provided operations.
 *
 * A backend supplies these functions for its environment.
 * Delay support is optional; critical-section callbacks
 * must be supplied as a pair.
 */
typedef struct
{
    platform_status_t (*delay_ms)(platform_uint32_t milliseconds);

    platform_status_t (*critical_enter)(
        platform_uint32_t *state);

    platform_status_t (*critical_exit)(
        platform_uint32_t state);
} osal_ops_t;

platform_status_t OSAL_Init(const osal_ops_t *ops);

platform_bool_t OSAL_IsInitialized(void);

platform_status_t OSAL_DelayMs(platform_uint32_t milliseconds);

platform_status_t OSAL_CriticalEnter(platform_uint32_t *state);

platform_status_t OSAL_CriticalExit(platform_uint32_t state);

#endif
