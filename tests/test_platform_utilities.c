
#include <stdio.h>
#include "platform_types.h"
#include "platform_timeout.h"
#include "platform_critical.h"

static unsigned int tests_run = 0U;
static unsigned int tests_passed = 0U;
static unsigned int tests_failed = 0U;

#define CHECK_TEST(description, expression)                 \
    do                                                      \
    {                                                       \
        tests_run++;                                        \
        if (expression)                                     \
        {                                                   \
            tests_passed++;                                 \
            printf("[PASS] %s\n", description);             \
        }                                                   \
        else                                                \
        {                                                   \
            tests_failed++;                                 \
            printf("[FAIL] %s (line %d)\n",                 \
                   description, __LINE__);                 \
        }                                                   \
    } while (0)

static void test_timeout(void)
{
    platform_timeout_t start = PLATFORM_GetTick();

    CHECK_TEST("Zero timeout expires immediately",
               PLATFORM_TimeoutExpired(start, 0U) == PLATFORM_TRUE);

    CHECK_TEST("Positive timeout has not expired initially",
               PLATFORM_TimeoutExpired(start, 5U) == PLATFORM_FALSE);

    PLATFORM_DelayTicks(4U);

    CHECK_TEST("Timeout remains active before deadline",
               PLATFORM_TimeoutExpired(start, 5U) == PLATFORM_FALSE);

    PLATFORM_DelayTicks(1U);

    CHECK_TEST("Timeout expires at deadline",
               PLATFORM_TimeoutExpired(start, 5U) == PLATFORM_TRUE);

    CHECK_TEST("Tick counter advances by requested delay",
               PLATFORM_GetTick() == (start + 5U));
}

static void test_critical_sections(void)
{
    platform_critical_state_t previous;

    CHECK_TEST("Critical section starts inactive",
               PLATFORM_IsCriticalActive() == PLATFORM_FALSE);

    previous = PLATFORM_EnterCritical();

    CHECK_TEST("First entry records previous inactive state",
               previous == 0U);

    CHECK_TEST("Critical section becomes active",
               PLATFORM_IsCriticalActive() == PLATFORM_TRUE);

    PLATFORM_ExitCritical(previous);

    CHECK_TEST("Exit restores inactive state",
               PLATFORM_IsCriticalActive() == PLATFORM_FALSE);

    previous = PLATFORM_EnterCritical();
    PLATFORM_ExitCritical(previous);

    CHECK_TEST("Critical section supports repeated entry and exit",
               PLATFORM_IsCriticalActive() == PLATFORM_FALSE);
}

static void test_nested_critical_sections(void)
{
    platform_critical_state_t outer_state;
    platform_critical_state_t inner_state;

    outer_state = PLATFORM_EnterCritical();
    inner_state = PLATFORM_EnterCritical();

    CHECK_TEST("Nested entry records active previous state",
               inner_state != 0U &&
               PLATFORM_IsCriticalActive() == PLATFORM_TRUE);

    PLATFORM_ExitCritical(inner_state);

    CHECK_TEST("Inner exit preserves outer critical section",
               PLATFORM_IsCriticalActive() == PLATFORM_TRUE);

    PLATFORM_ExitCritical(outer_state);

    CHECK_TEST("Outer exit restores inactive state",
               PLATFORM_IsCriticalActive() == PLATFORM_FALSE);
}

int main(void)
{
    printf("\n--- Platform timeout tests ---\n");
    test_timeout();

    printf("\n--- Critical-section tests ---\n");
    test_critical_sections();

    printf("\n--- Nested critical-section tests ---\n");
    test_nested_critical_sections();

    printf("\n====================================\n");
    printf("Platform utility tests: %u\n", tests_run);
    printf("Passed:                 %u\n", tests_passed);
    printf("Failed:                 %u\n", tests_failed);
    printf("====================================\n");

    return (tests_failed == 0U) ? 0 : 1;
}
