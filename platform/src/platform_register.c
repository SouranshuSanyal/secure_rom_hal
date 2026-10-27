#include "platform_register.h"

platform_uint32_t PLATFORM_RegisterRead32(
    volatile platform_uint32_t *address)
{
    return *address;
}

void PLATFORM_RegisterWrite32(
    volatile platform_uint32_t *address,
    platform_uint32_t value)
{
    *address = value;
}