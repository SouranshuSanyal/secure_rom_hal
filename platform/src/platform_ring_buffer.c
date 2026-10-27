#include <stddef.h>

#include "platform_ring_buffer.h"

void RING_BUFFER_Init(platform_ring_buffer_t *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    buffer->head = 0U;
    buffer->tail = 0U;
}

platform_bool_t RING_BUFFER_IsEmpty(
    const platform_ring_buffer_t *buffer)
{
    if (buffer == NULL)
    {
        return PLATFORM_TRUE;
    }

    return (buffer->head == buffer->tail)
               ? PLATFORM_TRUE
               : PLATFORM_FALSE;
}

platform_bool_t RING_BUFFER_IsFull(
    const platform_ring_buffer_t *buffer)
{
    platform_uint32_t next_head;

    if (buffer == NULL)
    {
        return PLATFORM_FALSE;
    }

    next_head = (buffer->head + 1U) % RING_BUFFER_CAPACITY;

    return (next_head == buffer->tail)
               ? PLATFORM_TRUE
               : PLATFORM_FALSE;
}

platform_status_t RING_BUFFER_Push(
    platform_ring_buffer_t *buffer,
    platform_uint8_t data)
{
    platform_uint32_t next_head;

    if (buffer == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (RING_BUFFER_IsFull(buffer) == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_BUSY;
    }

    buffer->data[buffer->head] = data;

    next_head = (buffer->head + 1U) % RING_BUFFER_CAPACITY;
    buffer->head = next_head;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t RING_BUFFER_Pop(
    platform_ring_buffer_t *buffer,
    platform_uint8_t *data)
{
    if ((buffer == NULL) || (data == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if (RING_BUFFER_IsEmpty(buffer) == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_BUSY;
    }

    *data = buffer->data[buffer->tail];

    buffer->tail =
        (buffer->tail + 1U) % RING_BUFFER_CAPACITY;

    return PLATFORM_STATUS_SUCCESS;
}