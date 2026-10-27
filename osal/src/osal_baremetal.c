
#include "osal_baremetal.h"
#include "stdlib.h"

platform_status_t OSAL_BareMetal_GetOps(
    const osal_baremetal_hooks_t *hooks,
    osal_ops_t *ops)
{
    if ((hooks == NULL) || (ops == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Require all hooks for this backend.
     * A real bare-metal target must provide each operation.
     */
    if ((hooks->delay_ms == NULL) ||
        (hooks->critical_enter == NULL) ||
        (hooks->critical_exit == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    ops->delay_ms = hooks->delay_ms;
    ops->critical_enter = hooks->critical_enter;
    ops->critical_exit = hooks->critical_exit;

    return PLATFORM_STATUS_SUCCESS;
}
