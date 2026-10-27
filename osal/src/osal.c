
#include "osal.h"
#include "stdlib.h"

static osal_ops_t g_osal_ops;
static platform_bool_t g_osal_initialized = PLATFORM_FALSE;

platform_status_t OSAL_Init(const osal_ops_t *ops)
{
    if (ops == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* Critical-section callbacks must be provided together. */
    if ((ops->critical_enter == NULL) ||
        (ops->critical_exit == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (g_osal_initialized == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_BUSY;
    }

    g_osal_ops = *ops;
    g_osal_initialized = PLATFORM_TRUE;

    return PLATFORM_STATUS_SUCCESS;
}

platform_bool_t OSAL_IsInitialized(void)
{
    return g_osal_initialized;
}

platform_status_t OSAL_DelayMs(platform_uint32_t milliseconds)
{
    if (g_osal_initialized != PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (milliseconds == 0U)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (g_osal_ops.delay_ms == NULL)
    {
        return PLATFORM_STATUS_NOT_SUPPORTED;
    }

    return g_osal_ops.delay_ms(milliseconds);
}

platform_status_t OSAL_CriticalEnter(platform_uint32_t *state)
{
    if (g_osal_initialized != PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    if (state == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    return g_osal_ops.critical_enter(state);
}

platform_status_t OSAL_CriticalExit(platform_uint32_t state)
{
    if (g_osal_initialized != PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    return g_osal_ops.critical_exit(state);
}
