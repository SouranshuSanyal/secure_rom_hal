
#include <stdio.h>

#include "platform_ring_buffer.h"

static int failures = 0;

#define CHECK_TEST(name, condition)             \
    do                                          \
    {                                           \
        if (condition)                          \
        {                                       \
            printf("[PASS] %s\n", name);        \
        }                                       \
        else                                    \
        {                                       \
            printf("[FAIL] %s\n", name);        \
            failures++;                         \
        }                                       \
    } while (0)

int main(void)
{
    platform_ring_buffer_t buffer;
    platform_status_t status;
    platform_uint8_t value;
    platform_uint32_t i;
    platform_bool_t order_correct;

    printf("Day 30: Ring Buffer Correctness Tests\n\n");

    RING_BUFFER_Init(&buffer);

    CHECK_TEST("Buffer initializes empty",
               RING_BUFFER_IsEmpty(&buffer) == PLATFORM_TRUE);

    /* Test 1: Exactly seven slots are available. */
    for (i = 0U; i < RING_BUFFER_CAPACITY - 1U; i++)
    {
        status = RING_BUFFER_Push(&buffer, (platform_uint8_t)i);

        if (status != PLATFORM_STATUS_SUCCESS)
        {
            break;
        }
    }

    CHECK_TEST("Seven bytes accepted",
               i == RING_BUFFER_CAPACITY - 1U);

    CHECK_TEST("Buffer reports full",
               RING_BUFFER_IsFull(&buffer) == PLATFORM_TRUE);

    status = RING_BUFFER_Push(&buffer, 99U);

    CHECK_TEST("Additional byte rejected when full",
               status == PLATFORM_STATUS_BUSY);

    /* Test 2: Pop three bytes to advance tail. */
    order_correct = PLATFORM_TRUE;

    for (i = 0U; i < 3U; i++)
    {
        value = 0xFFU;
        status = RING_BUFFER_Pop(&buffer, &value);

        if ((status != PLATFORM_STATUS_SUCCESS) ||
            (value != (platform_uint8_t)i))
        {
            order_correct = PLATFORM_FALSE;
            break;
        }
    }

    CHECK_TEST("Initial FIFO reads correct",
               order_correct == PLATFORM_TRUE);

    /* Test 3: Push more bytes to force head wraparound. */
    order_correct = PLATFORM_TRUE;

    for (i = 7U; i <= 9U; i++)
    {
        status = RING_BUFFER_Push(&buffer, (platform_uint8_t)i);

        if (status != PLATFORM_STATUS_SUCCESS)
        {
            order_correct = PLATFORM_FALSE;
            break;
        }
    }

    CHECK_TEST("Push succeeds across wraparound",
               order_correct == PLATFORM_TRUE);

    /* Remaining FIFO order should be 3, 4, 5, 6, 7, 8, 9. */
    order_correct = PLATFORM_TRUE;

    for (i = 3U; i <= 9U; i++)
    {
        value = 0xFFU;
        status = RING_BUFFER_Pop(&buffer, &value);

        if ((status != PLATFORM_STATUS_SUCCESS) ||
            (value != (platform_uint8_t)i))
        {
            order_correct = PLATFORM_FALSE;
            break;
        }
    }

    CHECK_TEST("FIFO order preserved across wraparound",
               order_correct == PLATFORM_TRUE);

    CHECK_TEST("Buffer empty after draining",
               RING_BUFFER_IsEmpty(&buffer) == PLATFORM_TRUE);

    /* Test 4: Empty pop must not modify the output byte. */
    value = 0xAAU;
    status = RING_BUFFER_Pop(&buffer, &value);

    CHECK_TEST("Empty pop returns BUSY",
               status == PLATFORM_STATUS_BUSY);

    CHECK_TEST("Empty pop leaves output unchanged",
               value == 0xAAU);

    /* Test 5: Invalid arguments. */
    status = RING_BUFFER_Push(NULL, 1U);

    CHECK_TEST("Null buffer push rejected",
               status == PLATFORM_STATUS_INVALID_ARGUMENT);

    status = RING_BUFFER_Pop(&buffer, NULL);

    CHECK_TEST("Null output pointer rejected",
               status == PLATFORM_STATUS_INVALID_ARGUMENT);

    status = RING_BUFFER_Pop(NULL, &value);

    CHECK_TEST("Null buffer pop rejected",
               status == PLATFORM_STATUS_INVALID_ARGUMENT);

    printf("\nTotal tests failed: %d\n", failures);

    if (failures != 0)
    {
        printf("Ring buffer tests failed\n");
        return 1;
    }

    printf("All Day 30 tests passed\n");
    return 0;
}
