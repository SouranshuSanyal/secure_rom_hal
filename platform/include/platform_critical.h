#ifndef PLATFORM_CRITICAL_H
#define PLATFORM_CRITICAL_H

#include "platform_types.h"

typedef platform_uint32_t platform_critical_state_t;

platform_critical_state_t PLATFORM_EnterCritical(void);

void PLATFORM_ExitCritical(
    platform_critical_state_t previous_state);

platform_bool_t PLATFORM_IsCriticalActive(void);

#endif /* PLATFORM_CRITICAL_H */