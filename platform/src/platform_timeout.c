#include "platform_timeout.h"

static platform_timeout_t platform_tick = 0U;

platform_timeout_t PLATFORM_GetTick(void)
{
    return platform_tick;
}

platform_bool_t PLATFORM_TimeoutExpired(
    platform_timeout_t start,
    platform_timeout_t timeout)
{
    platform_timeout_t current;

    current = PLATFORM_GetTick();

    if ((current - start) >= timeout)
    {
        return PLATFORM_TRUE;
    }

    return PLATFORM_FALSE;
}

void PLATFORM_DelayTicks(platform_timeout_t ticks)
{
    platform_tick += ticks;
}