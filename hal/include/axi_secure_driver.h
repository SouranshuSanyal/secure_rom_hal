
#ifndef AXI_SECURE_DRIVER_H
#define AXI_SECURE_DRIVER_H

#include "periph.h"
#include "platform_status.h"
#include "axi_secure_regs.h"

typedef struct
{
    periph_t peripheral;
} axi_secure_device_t;

platform_status_t AXI_SECURE_Init(
    axi_secure_device_t *device,
    volatile platform_uint32_t *base_address);

platform_status_t AXI_SECURE_Enable(
    axi_secure_device_t *device);

platform_status_t AXI_SECURE_Disable(
    axi_secure_device_t *device);

platform_status_t AXI_SECURE_WriteOutput(
    axi_secure_device_t *device,
    platform_uint32_t value);

platform_status_t AXI_SECURE_ReadOutput(
    axi_secure_device_t *device,
    platform_uint32_t *value);

platform_status_t AXI_SECURE_ReadStatus(
    axi_secure_device_t *device,
    platform_uint32_t *value);

#endif
