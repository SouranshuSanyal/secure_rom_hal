#include "platform_critical.h"

static platform_bool_t critical_active = PLATFORM_FALSE;

platform_critical_state_t PLATFORM_EnterCritical(void)
{
    platform_critical_state_t previous_state;

    previous_state = (platform_critical_state_t)critical_active;

    critical_active = PLATFORM_TRUE;

    return previous_state;
}

void PLATFORM_ExitCritical(
    platform_critical_state_t previous_state)
{
    if (previous_state == 0U)
    {
        critical_active = PLATFORM_FALSE;
    }
    else
    {
        critical_active = PLATFORM_TRUE;
    }
}

platform_bool_t PLATFORM_IsCriticalActive(void)
{
    return critical_active;
}