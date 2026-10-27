
#include <stddef.h>

#include "axi_secure_driver.h"

static platform_bool_t AXI_SECURE_IsInitialized(
    const axi_secure_device_t *device)
{
    return (device != NULL) &&
           (device->peripheral.base_address != NULL);
}

platform_status_t AXI_SECURE_Init(
    axi_secure_device_t *device,
    volatile platform_uint32_t *base_address)
{
    if ((device == NULL) || (base_address == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (PERIPH_Init(&device->peripheral, base_address) == PLATFORM_FALSE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t AXI_SECURE_Enable(
    axi_secure_device_t *device)
{
    if (!AXI_SECURE_IsInitialized(device))
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    PERIPH_Enable(&device->peripheral);

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t AXI_SECURE_Disable(
    axi_secure_device_t *device)
{
    if (!AXI_SECURE_IsInitialized(device))
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    PERIPH_Disable(&device->peripheral);

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t AXI_SECURE_WriteOutput(
    axi_secure_device_t *device,
    platform_uint32_t value)
{
    if (!AXI_SECURE_IsInitialized(device))
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    /* The GPIO output contains 8 implemented bits. */
    value &= 0xFFU;

    PERIPH_Write(
        &device->peripheral,
        AXI_SECURE_OUTPUT_OFFSET,
        value);

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t AXI_SECURE_ReadOutput(
    axi_secure_device_t *device,
    platform_uint32_t *value)
{
    if ((device == NULL) || (value == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (!AXI_SECURE_IsInitialized(device))
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    *value = PERIPH_Read(
        &device->peripheral,
        AXI_SECURE_OUTPUT_OFFSET);

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t AXI_SECURE_ReadStatus(
    axi_secure_device_t *device,
    platform_uint32_t *value)
{
    if ((device == NULL) || (value == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (!AXI_SECURE_IsInitialized(device))
    {
        return PLATFORM_STATUS_NOT_INITIALIZED;
    }

    *value = PERIPH_Read(
        &device->peripheral,
        AXI_SECURE_STATUS_OFFSET);

    return PLATFORM_STATUS_SUCCESS;
}
