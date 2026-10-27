
#include "periph.h"
#include "stdlib.h"

platform_bool_t PERIPH_Init(
    periph_t *peripheral,
    volatile platform_uint32_t *base_address)
{
    if ((peripheral == NULL) || (base_address == NULL))
    {
        return PLATFORM_FALSE;
    }

    peripheral->base_address = base_address;

    return PLATFORM_TRUE;
}

void PERIPH_Write(
    periph_t *peripheral,
    platform_uint32_t offset,
    platform_uint32_t value)
{
    if ((peripheral == NULL) ||
        (peripheral->base_address == NULL))
    {
        return;
    }

    if ((offset != PERIPH_CONTROL_OFFSET) &&
        (offset != PERIPH_OUTPUT_OFFSET))
    {
        return;
    }

    peripheral->base_address[offset / sizeof(platform_uint32_t)] = value;
}

platform_uint32_t PERIPH_Read(
    periph_t *peripheral,
    platform_uint32_t offset)
{
    if ((peripheral == NULL) ||
        (peripheral->base_address == NULL))
    {
        return 0U;
    }

    if ((offset != PERIPH_CONTROL_OFFSET) &&
        (offset != PERIPH_STATUS_OFFSET) &&
        (offset != PERIPH_OUTPUT_OFFSET))
    {
        return 0U;
    }

    return peripheral->base_address[offset / sizeof(platform_uint32_t)];
}

void PERIPH_Enable(periph_t *peripheral)
{
    platform_uint32_t value = PERIPH_Read(
        peripheral, PERIPH_CONTROL_OFFSET);

    PERIPH_Write(
        peripheral,
        PERIPH_CONTROL_OFFSET,
        value | 0x01U);
}

void PERIPH_Disable(periph_t *peripheral)
{
    platform_uint32_t value = PERIPH_Read(
        peripheral, PERIPH_CONTROL_OFFSET);

    PERIPH_Write(
        peripheral,
        PERIPH_CONTROL_OFFSET,
        value & ~0x01U);
}
