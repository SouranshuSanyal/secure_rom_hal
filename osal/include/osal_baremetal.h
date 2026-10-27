
#ifndef OSAL_BAREMETAL_H
#define OSAL_BAREMETAL_H

#include "osal.h"

/*
 * Platform hooks supplied by the embedded target.
 *
 * The critical-section functions must preserve and restore
 * the processor's previous interrupt/critical-section state.
 */
typedef struct
{
    platform_status_t (*delay_ms)(
        platform_uint32_t milliseconds);

    platform_status_t (*critical_enter)(
        platform_uint32_t *state);

    platform_status_t (*critical_exit)(
        platform_uint32_t state);
} osal_baremetal_hooks_t;

/*
 * Convert platform hooks into the generic OSAL operations table.
 * This function does not initialize the OSAL itself.
 */
platform_status_t OSAL_BareMetal_GetOps(
    const osal_baremetal_hooks_t *hooks,
    osal_ops_t *ops);

#endif
