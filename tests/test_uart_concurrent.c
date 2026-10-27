#include <stdio.h>
#include <stddef.h>

#include "platform_types.h"
#include "platform_status.h"
#include "osal.h"
#include "uart.h"
#include "uart_concurrent.h"
#include "uart_types.h"
#include "platform_ring_buffer.h"

#define STRESS_CYCLES 100U

static int failures = 0;
static unsigned int enter_calls = 0U;
static unsigned int exit_calls = 0U;
static platform_bool_t fail_enter = PLATFORM_FALSE;
static platform_bool_t fail_exit = PLATFORM_FALSE;
static platform_uint32_t last_saved_state = 0U;

#define CHECK_TEST(name, condition)                 \
    do                                              \
    {                                               \
        if (condition)                              \
        {                                           \
            printf("[PASS] %s\n", name);            \
        }                                           \
        else                                        \
        {                                           \
            printf("[FAIL] %s\n", name);            \
            failures++;                             \
        }                                           \
    } while (0)

static platform_status_t MOCK_CriticalEnter(
    platform_uint32_t *state)
{
    if (state == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    enter_calls++;

    if (fail_enter == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    *state = 0xA5U;

    return PLATFORM_STATUS_SUCCESS;
}

static platform_status_t MOCK_CriticalExit(
    platform_uint32_t state)
{
    exit_calls++;
    last_saved_state = state;

    if (fail_exit == PLATFORM_TRUE)
    {
        return PLATFORM_STATUS_ERROR;
    }

    return PLATFORM_STATUS_SUCCESS;
}

int main(void)
{
    const osal_ops_t ops =
    {
        .delay_ms = NULL,
        .critical_enter = MOCK_CriticalEnter,
        .critical_exit = MOCK_CriticalExit
    };

    const uart_config_t config =
    {
        .baudrate = 115200U,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1
    };

    platform_status_t status;
    platform_uint8_t received = 0U;
    platform_uint32_t cycle;
    platform_uint32_t i;
    platform_uint32_t accepted;
    platform_bool_t order_correct;

    printf("Day 29: UART Ring Buffer Stress Tests\n\n");

    /* Initialize the OSAL and UART once. */
    status = OSAL_Init(&ops);

    CHECK_TEST("OSAL initialized",
               status == PLATFORM_STATUS_SUCCESS);

    status = UART_Init(&config);

    CHECK_TEST("UART initialized",
               status == PLATFORM_STATUS_SUCCESS);

    /* Test 1: Null output pointer. */
    status = UART_CONCURRENT_ReadByte(NULL);

    CHECK_TEST("Null read pointer rejected",
               status == PLATFORM_STATUS_INVALID_ARGUMENT);

    /* Test 2: Enter failure must prevent the operation. */
    fail_enter = PLATFORM_TRUE;

    status = UART_CONCURRENT_InjectRxByte(0x55U);

    CHECK_TEST("Critical-enter failure reported",
               status == PLATFORM_STATUS_ERROR);

    fail_enter = PLATFORM_FALSE;

    status = UART_CONCURRENT_ReadByte(&received);

    CHECK_TEST("Failed entry did not inject a byte",
               status == PLATFORM_STATUS_BUSY);

    /* Test 3: Exit failure is reported after the operation. */
    fail_exit = PLATFORM_TRUE;

    status = UART_CONCURRENT_InjectRxByte(0xC3U);

    CHECK_TEST("Critical-exit failure reported",
               status == PLATFORM_STATUS_ERROR);

    fail_exit = PLATFORM_FALSE;

    received = 0U;
    status = UART_CONCURRENT_ReadByte(&received);

    CHECK_TEST("Operation completed before exit failure",
               (status == PLATFORM_STATUS_SUCCESS) &&
               (received == 0xC3U));

    CHECK_TEST("Saved critical-section state preserved",
               last_saved_state == 0xA5U);

    /*
     * Test 4: Repeatedly fill, overflow, drain and refill.
     * This exercises head/tail wraparound over many cycles.
     */
    order_correct = PLATFORM_TRUE;

    for (cycle = 0U; cycle < STRESS_CYCLES; cycle++)
    {
        accepted = 0U;
        status = PLATFORM_STATUS_SUCCESS;

        /* Fill until the buffer rejects an additional byte. */
        for (i = 0U; i < RING_BUFFER_CAPACITY + 1U; i++)
        {
            platform_uint8_t value =
                (platform_uint8_t)(cycle + i);

            status = UART_CONCURRENT_InjectRxByte(value);

            if (status != PLATFORM_STATUS_SUCCESS)
            {
                break;
            }

            accepted++;
        }

        if ((status != PLATFORM_STATUS_BUSY) ||
            (accepted == 0U))
        {
            order_correct = PLATFORM_FALSE;
            break;
        }

        /* Drain the accepted bytes and check FIFO ordering. */
        for (i = 0U; i < accepted; i++)
        {
            platform_uint8_t expected =
                (platform_uint8_t)(cycle + i);

            received = 0U;
            status = UART_CONCURRENT_ReadByte(&received);

            if ((status != PLATFORM_STATUS_SUCCESS) ||
                (received != expected))
            {
                order_correct = PLATFORM_FALSE;
                break;
            }
        }

        if (order_correct == PLATFORM_FALSE)
        {
            break;
        }

        /* The buffer must be empty after draining. */
        status = UART_CONCURRENT_ReadByte(&received);

        if (status != PLATFORM_STATUS_BUSY)
        {
            order_correct = PLATFORM_FALSE;
            break;
        }
    }

    CHECK_TEST("100 fill/overflow/drain cycles preserve FIFO order",
               order_correct == PLATFORM_TRUE);

    /* Test 5: Every successful critical entry has an exit attempt. */
   CHECK_TEST("Successful critical entries have matching exits",
           enter_calls == (exit_calls + 1U));

    printf("\nTotal tests failed: %d\n", failures);

    if (failures != 0)
    {
        printf("Day 29 tests failed\n");
        return 1;
    }

    printf("All Day 29 tests passed\n");
    return 0;
}