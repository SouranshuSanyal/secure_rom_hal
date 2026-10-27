#ifndef PLATFORM_RING_BUFFER_H
#define PLATFORM_RING_BUFFER_H

#include "platform_types.h"
#include "platform_status.h"

#define RING_BUFFER_CAPACITY 8U

typedef struct
{
    platform_uint8_t data[RING_BUFFER_CAPACITY];
    platform_uint32_t head;
    platform_uint32_t tail;
} platform_ring_buffer_t;

void RING_BUFFER_Init(platform_ring_buffer_t *buffer);

platform_status_t RING_BUFFER_Push(
    platform_ring_buffer_t *buffer,
    platform_uint8_t data);

platform_status_t RING_BUFFER_Pop(
    platform_ring_buffer_t *buffer,
    platform_uint8_t *data);

platform_bool_t RING_BUFFER_IsEmpty(
    const platform_ring_buffer_t *buffer);

platform_bool_t RING_BUFFER_IsFull(
    const platform_ring_buffer_t *buffer);

#endif /* PLATFORM_RING_BUFFER_H */