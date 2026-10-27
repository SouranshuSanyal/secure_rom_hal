
#include <stdio.h>
#include <stddef.h>

#include "osal_rtos.h"
#include "uart.h"
#include "uart_concurrent.h"
#include "platform_ring_buffer.h"

static int failures = 0;
static platform_uint32_t recorded_ticks = 0U;
static platform_uint32_t recorded_state = 0U;
static unsigned int delay_calls = 0U;
static unsigned int enter_calls = 0U;
static unsigned int exit_calls = 0U;

#define CHECK_TEST(name, condition)                  \
    do                                               \
    {                                                \
        if (condition)                               \
        {                                            \
            printf("[PASS] %s\n", name);             \
        }                                            \
        else                                         \
        {                                            \
            printf("[FAIL] %s\n", name);             \
            failures++;                              \
        }                                            \
    } while (0)

static platform_status_t MOCK_RtosDelayTicks(
    platform_uint32_t ticks)
{
    recorded_ticks = ticks;
    delay_calls++;
    return PLATFORM_STATUS_SUCCESS;
}

static platform_status_t MOCK_RtosCriticalEnter(
    platform_uint32_t *state)
{
    if (state == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    *state = 0x37U;
    enter_calls++;

    return PLATFORM_STATUS_SUCCESS;
}

static platform_status_t MOCK_RtosCriticalExit(
    platform_uint32_t state)
{
    recorded_state = state;
    exit_calls++;

    return PLATFORM_STATUS_SUCCESS;
}

int main(void)
{
    const osal_rtos_hooks_t hooks =
    {
        .tick_rate_hz = 100U,
        .delay_ticks = MOCK_RtosDelayTicks,
        .critical_enter = MOCK_RtosCriticalEnter,
        .critical_exit = MOCK_RtosCriticalExit
    };

    const osal_rtos_hooks_t invalid_hooks =
    {
        .tick_rate_hz = 0U,
        .delay_ticks = MOCK_RtosDelayTicks,
        .critical_enter = MOCK_RtosCriticalEnter,
        .critical_exit = MOCK_RtosCriticalExit
    };

    const osal_rtos_hooks_t high_rate_hooks =
    {
        .tick_rate_hz = 1000000U,
        .delay_ticks = MOCK_RtosDelayTicks,
        .critical_enter = MOCK_RtosCriticalEnter,
        .critical_exit = MOCK_RtosCriticalExit
    };

    const uart_config_t uart_config =
    {
        .baudrate = 115200U,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1
    };

    osal_ops_t ops = {0};
    osal_ops_t high_rate_ops = {0};

    platform_status_t status;
    platform_uint32_t state = 0U;
    platform_uint8_t received = 0U;
    platform_uint32_t accepted = 0U;
    platform_uint32_t i;
    unsigned int enter_before;
    unsigned int exit_before;

    printf("Day 27: RTOS OSAL Backend Tests\n\n");

    /* Day 27, Test 1: Validate arguments. */
    status = OSAL_RTOS_GetOps(NULL, &ops);

    CHECK_TEST("Null RTOS hooks rejected",
               status == PLATFORM_STATUS_INVALID_ARGUMENT);

    status = OSAL_RTOS_GetOps(&hooks, NULL);

    CHECK_TEST("Null operations output rejected",
               status == PLATFORM_STATUS_INVALID_ARGUMENT);

    /* Test 2: Reject zero tick rate. */
    status = OSAL_RTOS_GetOps(&invalid_hooks, &ops);

    CHECK_TEST("Zero tick rate rejected",
               status == PLATFORM_STATUS_INVALID_ARGUMENT);

    /* Test 3: Initialize OSAL. */
    status = OSAL_RTOS_GetOps(&hooks, &ops);

    CHECK_TEST("Valid RTOS hooks accepted",
               status == PLATFORM_STATUS_SUCCESS);

    status = OSAL_Init(&ops);

    CHECK_TEST("OSAL initialized with RTOS backend",
               status == PLATFORM_STATUS_SUCCESS);

    /* Test 4: Convert milliseconds to ticks. */
    status = OSAL_DelayMs(15U);

    CHECK_TEST("15 ms converted to 2 ticks",
               (status == PLATFORM_STATUS_SUCCESS) &&
               (recorded_ticks == 2U) &&
               (delay_calls == 1U));

    status = OSAL_DelayMs(1U);

    CHECK_TEST("1 ms converted to 1 tick",
               (status == PLATFORM_STATUS_SUCCESS) &&
               (recorded_ticks == 1U) &&
               (delay_calls == 2U));

    status = OSAL_DelayMs(0U);

    CHECK_TEST("Zero delay rejected",
               status == PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("Zero delay not forwarded",
               delay_calls == 2U);

    /* Test 5: Reject tick conversion overflow. */
    status = OSAL_RTOS_GetOps(&high_rate_hooks, &high_rate_ops);

    CHECK_TEST("High-rate RTOS configuration accepted",
               status == PLATFORM_STATUS_SUCCESS);

    status = high_rate_ops.delay_ms(0xFFFFFFFFU);

    CHECK_TEST("Tick conversion overflow rejected",
               status == PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("Overflow not forwarded to RTOS",
               delay_calls == 2U);

    /* Test 6: Critical-section hooks preserve state. */
    status = OSAL_CriticalEnter(&state);

    CHECK_TEST("RTOS critical enter forwarded",
               (status == PLATFORM_STATUS_SUCCESS) &&
               (state == 0x37U) &&
               (enter_calls == 1U));

    status = OSAL_CriticalExit(state);

    CHECK_TEST("RTOS critical exit restores saved state",
               (status == PLATFORM_STATUS_SUCCESS) &&
               (recorded_state == 0x37U) &&
               (exit_calls == 1U));

    printf("\nDay 28: UART Concurrency Integration Tests\n\n");

    enter_before = enter_calls;
    exit_before = exit_calls;

    /* Test 7: Initialize the UART HAL. */
    status = UART_Init(&uart_config);

    CHECK_TEST("UART initialized",
               status == PLATFORM_STATUS_SUCCESS);

    /* Test 8: Reading an empty RX buffer. */
    status = UART_CONCURRENT_ReadByte(&received);

    CHECK_TEST("Empty RX buffer returns BUSY",
               status == PLATFORM_STATUS_BUSY);

    /* Test 9: Inject and retrieve a byte. */
    status = UART_CONCURRENT_InjectRxByte(0xA5U);

    CHECK_TEST("RX byte injected inside critical section",
               status == PLATFORM_STATUS_SUCCESS);

    received = 0U;
    status = UART_CONCURRENT_ReadByte(&received);

    CHECK_TEST("RX byte read successfully",
               (status == PLATFORM_STATUS_SUCCESS) &&
               (received == 0xA5U));

    /* Test 10: Fill the buffer until it reports BUSY. */
    accepted = 0U;
    status = PLATFORM_STATUS_SUCCESS;

    for (i = 0U; i < RING_BUFFER_CAPACITY + 1U; i++)
    {
        status = UART_CONCURRENT_InjectRxByte(
            (platform_uint8_t)i);

        if (status != PLATFORM_STATUS_SUCCESS)
        {
            break;
        }

        accepted++;
    }

    CHECK_TEST("Full RX buffer rejects another byte",
               (status == PLATFORM_STATUS_BUSY) &&
               (accepted > 0U));

    /* Test 11: Verify FIFO order while draining the buffer. */
    {
        platform_bool_t order_correct = PLATFORM_TRUE;

        for (i = 0U; i < accepted; i++)
        {
            received = 0U;
            status = UART_CONCURRENT_ReadByte(&received);

            if ((status != PLATFORM_STATUS_SUCCESS) ||
                (received != (platform_uint8_t)i))
            {
                order_correct = PLATFORM_FALSE;
                break;
            }
        }

        CHECK_TEST("RX buffer preserves byte order",
                   order_correct == PLATFORM_TRUE);
    }

    /* Test 12: Buffer is empty again after draining. */
    status = UART_CONCURRENT_ReadByte(&received);

    CHECK_TEST("Drained RX buffer returns BUSY",
               status == PLATFORM_STATUS_BUSY);

    /* Test 13: Every protected operation enters and exits. */
    CHECK_TEST("Critical-section entries and exits are balanced",
               ((enter_calls - enter_before) ==
                (exit_calls - exit_before)) &&
               ((enter_calls - enter_before) > 0U));

    printf("\nTotal tests failed: %d\n", failures);

    if (failures != 0)
    {
        printf("Regression tests failed\n");
        return 1;
    }

    printf("All Day 27 and Day 28 tests passed\n");
    return 0;
}
