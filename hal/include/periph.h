
#ifndef PERIPH_H
#define PERIPH_H

#include "platform_types.h"

#define PERIPH_CONTROL_OFFSET  0x00U
#define PERIPH_STATUS_OFFSET   0x04U
#define PERIPH_OUTPUT_OFFSET   0x08U

typedef struct
{
    volatile platform_uint32_t *base_address;
} periph_t;

platform_bool_t PERIPH_Init(
    periph_t *peripheral,
    volatile platform_uint32_t *base_address);

void PERIPH_Write(
    periph_t *peripheral,
    platform_uint32_t offset,
    platform_uint32_t value);

platform_uint32_t PERIPH_Read(
    periph_t *peripheral,
    platform_uint32_t offset);

void PERIPH_Enable(periph_t *peripheral);
void PERIPH_Disable(periph_t *peripheral);

#endif
