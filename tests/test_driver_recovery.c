
#include <stdio.h>
#include "platform_status.h"
#include "uart.h"
#include "uart_types.h"
#include "spi.h"
#include "spi_types.h"
#include "i2c.h"
#include "i2c_types.h"

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
                   description, __LINE__);                  \
        }                                                   \
    } while (0)

static void test_uart_recovery(void)
{
    uart_config_t config = {
        .baudrate = 115200U,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1
    };

    CHECK_TEST("UART initializes",
               UART_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART can force TX busy",
               UART_ForceTxBusy() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART timeout is reported while TX remains busy",
               UART_WriteByteTimeout(0x55U, 3U) ==
               PLATFORM_STATUS_TIMEOUT);

    CHECK_TEST("UART reinitialization succeeds",
               UART_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART transmits after recovery",
               UART_WriteByte(0xA5U) == PLATFORM_STATUS_SUCCESS);
}

static void test_spi_recovery(void)
{
    spi_config_t config = {
        .clock_frequency = 1000000U,
        .mode = SPI_MODE_0,
        .bit_order = SPI_BIT_ORDER_MSB_FIRST
    };

    platform_uint8_t received = 0U;

    CHECK_TEST("SPI initializes",
               SPI_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("SPI rejects transfer without chip select",
               SPI_TransferByte(0x00U, &received) ==
               PLATFORM_STATUS_ERROR);

    CHECK_TEST("SPI register read works after rejected transfer",
               SPI_ReadRegister(0U, &received) ==
               PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("SPI register zero contains expected reset value",
               received == 0xA5U);

    CHECK_TEST("SPI can write a register after recovery",
               SPI_WriteRegister(1U, 0x5AU) ==
               PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("SPI can read back the written register",
               SPI_ReadRegister(1U, &received) ==
               PLATFORM_STATUS_SUCCESS &&
               received == 0x5AU);
}

static void test_i2c_nack_recovery(void)
{
    i2c_config_t config = {
        .clock_frequency = 100000U,
        .speed = I2C_SPEED_STANDARD
    };

    CHECK_TEST("I2C initializes",
               I2C_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C forced NACK is enabled",
               I2C_ForceNack() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C reports failure when NACK is forced",
               I2C_Start(0x50U, PLATFORM_FALSE) ==
               PLATFORM_STATUS_ERROR);

    CHECK_TEST("I2C reset clears NACK condition",
               I2C_ResetBus() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C transaction starts after NACK recovery",
               I2C_Start(0x50U, PLATFORM_FALSE) ==
               PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C transaction stops after recovery",
               I2C_Stop() == PLATFORM_STATUS_SUCCESS);
}

static void test_i2c_timeout_recovery(void)
{
    i2c_config_t config = {
        .clock_frequency = 100000U,
        .speed = I2C_SPEED_STANDARD
    };

    CHECK_TEST("I2C initializes for timeout test",
               I2C_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C forced timeout is enabled",
               I2C_ForceTimeout() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C reports forced timeout",
               I2C_Start(0x50U, PLATFORM_FALSE) ==
               PLATFORM_STATUS_TIMEOUT);

    CHECK_TEST("I2C reset clears timeout condition",
               I2C_ResetBus() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C transaction succeeds after timeout recovery",
               I2C_Start(0x50U, PLATFORM_FALSE) ==
               PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C stops after timeout recovery",
               I2C_Stop() == PLATFORM_STATUS_SUCCESS);
}

static void test_i2c_busy_bus_recovery(void)
{
    i2c_config_t config = {
        .clock_frequency = 100000U,
        .speed = I2C_SPEED_STANDARD
    };

    CHECK_TEST("I2C initializes for bus-busy test",
               I2C_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C bus can be forced busy",
               I2C_ForceBusBusy() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C start detects busy bus",
               I2C_Start(0x50U, PLATFORM_FALSE) ==
               PLATFORM_STATUS_BUSY);

    CHECK_TEST("I2C reset clears busy bus",
               I2C_ResetBus() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C starts transaction after bus recovery",
               I2C_Start(0x50U, PLATFORM_FALSE) ==
               PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C stops after bus recovery",
               I2C_Stop() == PLATFORM_STATUS_SUCCESS);
}

int main(void)
{
    printf("\n--- UART recovery tests ---\n");
    test_uart_recovery();

    printf("\n--- SPI recovery tests ---\n");
    test_spi_recovery();

    printf("\n--- I2C NACK recovery tests ---\n");
    test_i2c_nack_recovery();

    printf("\n--- I2C timeout recovery tests ---\n");
    test_i2c_timeout_recovery();

    printf("\n--- I2C busy-bus recovery tests ---\n");
    test_i2c_busy_bus_recovery();

    printf("\n====================================\n");
    printf("Recovery tests: %u\n", tests_run);
    printf("Passed:         %u\n", tests_passed);
    printf("Failed:         %u\n", tests_failed);
    printf("====================================\n");

    return (tests_failed == 0U) ? 0 : 1;
}
