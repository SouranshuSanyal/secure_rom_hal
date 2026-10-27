
#include "osal_rtos.h"
#include "stdlib.h"

static platform_status_t RTOS_DelayMs(
    platform_uint32_t milliseconds);

static platform_status_t RTOS_CriticalEnter(
    platform_uint32_t *state);

static platform_status_t RTOS_CriticalExit(
    platform_uint32_t state);

static osal_rtos_hooks_t g_rtos_hooks;

static platform_status_t RTOS_DelayMs(
    platform_uint32_t milliseconds)
{
    platform_uint64_t ticks;

    if (g_rtos_hooks.delay_ticks == NULL)
    {
        return PLATFORM_STATUS_NOT_SUPPORTED;
    }

    if ((milliseconds == 0U) ||
        (g_rtos_hooks.tick_rate_hz == 0U))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Round up so a nonzero delay does not become zero ticks.
     *
     * ticks = ceil(milliseconds * tick_rate_hz / 1000)
     */
    ticks =
        (((platform_uint64_t)milliseconds *
          g_rtos_hooks.tick_rate_hz) + 999U) / 1000U;

    if (ticks > 0xFFFFFFFFULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    return g_rtos_hooks.delay_ticks((platform_uint32_t)ticks);
}

static platform_status_t RTOS_CriticalEnter(
    platform_uint32_t *state)
{
    if (g_rtos_hooks.critical_enter == NULL)
    {
        return PLATFORM_STATUS_NOT_SUPPORTED;
    }

    return g_rtos_hooks.critical_enter(state);
}

static platform_status_t RTOS_CriticalExit(
    platform_uint32_t state)
{
    if (g_rtos_hooks.critical_exit == NULL)
    {
        return PLATFORM_STATUS_NOT_SUPPORTED;
    }

    return g_rtos_hooks.critical_exit(state);
}

platform_status_t OSAL_RTOS_GetOps(
    const osal_rtos_hooks_t *hooks,
    osal_ops_t *ops)
{
    if ((hooks == NULL) || (ops == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((hooks->tick_rate_hz == 0U) ||
        (hooks->delay_ticks == NULL) ||
        (hooks->critical_enter == NULL) ||
        (hooks->critical_exit == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Copy configuration so the caller's hooks structure
     * does not need to remain alive.
     */
    g_rtos_hooks = *hooks;

    ops->delay_ms = RTOS_DelayMs;
    ops->critical_enter = RTOS_CriticalEnter;
    ops->critical_exit = RTOS_CriticalExit;

    return PLATFORM_STATUS_SUCCESS;
}
