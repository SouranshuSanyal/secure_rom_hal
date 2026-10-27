#ifndef PLATFORM_TIMEOUT_H
#define PLATFORM_TIMEOUT_H

#include "platform_types.h"

typedef platform_uint32_t platform_timeout_t;

platform_timeout_t PLATFORM_GetTick(void);

platform_bool_t PLATFORM_TimeoutExpired(
    platform_timeout_t start,
    platform_timeout_t timeout);


void PLATFORM_DelayTicks(platform_timeout_t ticks);

#endif /* PLATFORM_TIMEOUT_H */