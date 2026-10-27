
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

    /* Simulated memory-mapped registers:
       index 0 = CONTROL
       index 1 = STATUS
       index 2 = OUTPUT
    */
    volatile platform_uint32_t registers[3] = {0U, 0U, 0U};
    platform_uint32_t value = 0U;

    CHECK(
        AXI_SECURE_Init(NULL, registers) ==
            PLATFORM_STATUS_INVALID_ARGUMENT,
        "Reject null device");

    CHECK(
        AXI_SECURE_Init(&device, NULL) ==
            PLATFORM_STATUS_INVALID_ARGUMENT,
        "Reject null base address");

    CHECK(
        AXI_SECURE_WriteOutput(&device, 0x55U) ==
            PLATFORM_STATUS_NOT_INITIALIZED,
        "Reject access before initialization");

    CHECK(
        AXI_SECURE_Init(&device, registers) ==
            PLATFORM_STATUS_SUCCESS,
        "Initialize device");

    CHECK(
        AXI_SECURE_Enable(&device) ==
            PLATFORM_STATUS_SUCCESS,
        "Enable peripheral");

    CHECK(
        registers[0] == 1U,
        "CONTROL enable bit set");

    CHECK(
        AXI_SECURE_Disable(&device) ==
            PLATFORM_STATUS_SUCCESS,
        "Disable peripheral");

    CHECK(
        registers[0] == 0U,
        "CONTROL enable bit cleared");

    CHECK(
        AXI_SECURE_WriteOutput(&device, 0xA5U) ==
            PLATFORM_STATUS_SUCCESS,
        "Write GPIO output");

    CHECK(
        registers[2] == 0xA5U,
        "OUTPUT register contains expected value");

    CHECK(
        AXI_SECURE_WriteOutput(&device, 0x1234U) ==
            PLATFORM_STATUS_SUCCESS,
        "Write value wider than GPIO");

    CHECK(
        registers[2] == 0x34U,
        "GPIO output limited to 8 bits");

    CHECK(
        AXI_SECURE_ReadOutput(&device, &value) ==
            PLATFORM_STATUS_SUCCESS &&
        value == 0x34U,
        "Read OUTPUT register");

    /* Host mock: emulate STATUS reflecting CONTROL[0]. */
    registers[1] = registers[0] & 1U;

    CHECK(
        AXI_SECURE_ReadStatus(&device, &value) ==
            PLATFORM_STATUS_SUCCESS &&
        value == 0U,
        "Read STATUS register");

    CHECK(
        AXI_SECURE_ReadOutput(&device, NULL) ==
            PLATFORM_STATUS_INVALID_ARGUMENT,
        "Reject null output pointer");

    CHECK(
        AXI_SECURE_ReadStatus(&device, NULL) ==
            PLATFORM_STATUS_INVALID_ARGUMENT,
        "Reject null status pointer");

    printf("\n");

    if (failures == 0)
    {
        printf("ALL AXI SECURE DRIVER TESTS PASSED\n");
        return 0;
    }

    printf("AXI SECURE DRIVER TESTS FAILED: %d\n", failures);
    return 1;
}
