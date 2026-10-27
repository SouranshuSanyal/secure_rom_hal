#ifndef PLATFORM_REGISTER_H
#define PLATFORM_REGISTER_H

#include "platform_types.h"

platform_uint32_t PLATFORM_RegisterRead32(volatile platform_uint32_t *address);

void PLATFORM_RegisterWrite32( volatile platform_uint32_t *address, platform_uint32_t value);

static inline platform_uint32_t PLATFORM_RegisterSetBits32(platform_uint32_t register_val, platform_uint32_t mask){
    return (register_val | mask);
}

static inline platform_uint32_t PLATFORM_RegisterClearBits32(platform_uint32_t register_val, platform_uint32_t mask){
    return (register_val & ~mask);
}

static inline platform_uint32_t PLATFORM_RegisterToggleBits32(
    platform_uint32_t register_val,
    platform_uint32_t mask)
{
    return register_val ^ mask;
}

#endif  /* PLATFORM_REGISTER_H */