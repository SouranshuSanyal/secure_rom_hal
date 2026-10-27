
#include <stdio.h>
#include <stddef.h>

#include "axi_secure_driver.h"

static int failures = 0;

#define CHECK(condition, message)                  \
    do                                             \
    {                                              \
        if (condition)                             \
        {                                          \
            printf("PASS: %s\n", message);         \
        }                                          \
        else                                       \
        {                                          \
            printf("FAIL: %s\n", message);         \
            failures++;                            \
        }                                          \
    } while (0)

int main(void)
{
    axi_secure_device_t device = {0};
    volatile platform_uint32_t registers[3] = {0U, 0U, 0U};
    platform_uint32_t value = 0U;

    printf("AXI Secure Driver Negative Tests\n\n");

    /* Invalid arguments */
    CHECK(
        AXI_SECURE_Init(NULL, registers) ==
            PLATFORM_STATUS_INVALID_ARGUMENT,
        "Reject null device");

    CHECK(
        AXI_SECURE_Init(&device, NULL) ==
            PLATFORM_STATUS_INVALID_ARGUMENT,
        "Reject null register base");

    CHECK(
        AXI_SECURE_ReadOutput(NULL, &value) ==
            PLATFORM_STATUS_INVALID_ARGUMENT,
        "Reject null device in read");

    CHECK(
        AXI_SECURE_ReadStatus(&device, NULL) ==
            PLATFORM_STATUS_INVALID_ARGUMENT,
        "Reject null status destination");

    /* Uninitialized device */
    CHECK(
        AXI_SECURE_Enable(&device) ==
            PLATFORM_STATUS_NOT_INITIALIZED,
        "Reject enable before initialization");

    CHECK(
        AXI_SECURE_Disable(&device) ==
            PLATFORM_STATUS_NOT_INITIALIZED,
        "Reject disable before initialization");

    CHECK(
        AXI_SECURE_WriteOutput(&device, 0x55U) ==
            PLATFORM_STATUS_NOT_INITIALIZED,
        "Reject output write before initialization");

    CHECK(
        AXI_SECURE_ReadOutput(&device, &value) ==
            PLATFORM_STATUS_NOT_INITIALIZED,
        "Reject output read before initialization");

    /* Recovery after invalid arguments */
    CHECK(
        AXI_SECURE_Init(&device, registers) ==
            PLATFORM_STATUS_SUCCESS,
        "Initialize successfully after invalid calls");

    CHECK(
        AXI_SECURE_WriteOutput(&device, 0xA6U) ==
            PLATFORM_STATUS_SUCCESS,
        "Valid write succeeds after initialization");

    CHECK(
        registers[2] == 0xA6U,
        "Output register contains expected value");

    CHECK(
        AXI_SECURE_ReadOutput(&device, &value) ==
            PLATFORM_STATUS_SUCCESS &&
        value == 0xA6U,
        "Valid read succeeds after negative tests");

    /* Low-level driver must reject STATUS writes. */
    PERIPH_Write(&device.peripheral, PERIPH_STATUS_OFFSET, 0xFFFFFFFFU);

    CHECK(
        registers[1] == 0U,
        "Low-level HAL ignores writes to STATUS");

    /* Invalid offset must not modify a valid register. */
    PERIPH_Write(&device.peripheral, 0x0CU, 0xFFFFFFFFU);

    CHECK(
        registers[0] == 0U &&
        registers[1] == 0U &&
        registers[2] == 0xA6U,
        "Unsupported offset does not modify valid registers");

    printf("\n");

    if (failures == 0)
    {
        printf("ALL AXI SECURE NEGATIVE TESTS PASSED\n");
        return 0;
    }

    printf("AXI SECURE NEGATIVE TESTS FAILED: %d\n", failures);
    return 1;
}
