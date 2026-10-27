#include <stdio.h>
#include <stddef.h>

#include "platform_status.h"
#include "gpio.h"
#include "gpio_types.h"
#include "uart.h"
#include "uart_types.h"
#include "spi.h"
#include "spi_types.h"
#include "i2c.h"
#include "i2c_types.h"

static unsigned int tests_run;
static unsigned int tests_failed;

#define CHECK_TEST(description, expression)                  \
    do                                                       \
    {                                                        \
        tests_run++;                                         \
        if (expression)                                      \
        {                                                    \
            printf("[PASS] %s\n", description);              \
        }                                                    \
        else                                                 \
        {                                                    \
            printf("[FAIL] %s (line %d)\n",                 \
                   description, __LINE__);                   \
            tests_failed++;                                  \
        }                                                    \
    } while (0)

static void test_uninitialized_calls(void)
{
    platform_uint8_t data = 0U;

    printf("\n--- Uninitialized driver tests ---\n");

    CHECK_TEST("UART write before init is rejected",
               UART_WriteByte(0x55U) ==
                   PLATFORM_STATUS_NOT_INITIALIZED);

    CHECK_TEST("UART read before init is rejected",
               UART_ReadByte(&data) ==
                   PLATFORM_STATUS_NOT_INITIALIZED);

    CHECK_TEST("SPI byte transfer before init is rejected",
               SPI_TransferByte(0U, &data) ==
                   PLATFORM_STATUS_NOT_INITIALIZED);

    CHECK_TEST("SPI register read before init is rejected",
               SPI_ReadRegister(0U, &data) ==
                   PLATFORM_STATUS_NOT_INITIALIZED);

    CHECK_TEST("I2C write before init is rejected",
               I2C_WriteByte(0x55U) ==
                   PLATFORM_STATUS_NOT_INITIALIZED);

    CHECK_TEST("I2C read before init is rejected",
               I2C_ReadByte(&data) ==
                   PLATFORM_STATUS_NOT_INITIALIZED);

    CHECK_TEST("I2C stop before init is rejected",
               I2C_Stop() ==
                   PLATFORM_STATUS_NOT_INITIALIZED);
}

static void test_gpio_negative(void)
{
    gpio_pin_level_t level = GPIO_PIN_LOW;

    printf("\n--- GPIO negative tests ---\n");

    CHECK_TEST("GPIO rejects pin 32 during init",
               GPIO_Init(32U, GPIO_OUTPUT) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("GPIO rejects pin 32 during read",
               GPIO_Read(32U, &level) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("GPIO rejects pin 32 during toggle",
               GPIO_Toggle(32U) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("GPIO rejects invalid direction",
               GPIO_Init(0U, (gpio_direction_t)99) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("GPIO rejects invalid output level",
               GPIO_Write(0U, (gpio_pin_level_t)99) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);
}

static void test_uart_negative(void)
{
    uart_config_t config = {
        .baudrate = 115200U,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1
    };

    uart_config_t invalid;
    platform_uint8_t data = 0U;

    printf("\n--- UART negative tests ---\n");

    invalid = config;
    invalid.parity = (uart_parity_t)99;

    CHECK_TEST("UART rejects invalid parity",
               UART_Init(&invalid) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    invalid = config;
    invalid.stop_bits = (uart_stop_bits_t)99;

    CHECK_TEST("UART rejects invalid stop-bit setting",
               UART_Init(&invalid) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("UART rejects zero baud rate",
               (invalid = config,
                invalid.baudrate = 0U,
                UART_Init(&invalid)) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("UART initializes after invalid configurations",
               UART_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART rejects null RX pointer",
               UART_ReadByte(NULL) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("UART rejects null string",
               UART_WriteString(NULL) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("UART rejects empty RX buffer read",
               UART_ReadByte(&data) == PLATFORM_STATUS_BUSY);
}

static void test_spi_negative(void)
{
    spi_config_t config = {
        .clock_frequency = 1000000U,
        .mode = SPI_MODE_0,
        .bit_order = SPI_BIT_ORDER_MSB_FIRST
    };

    spi_config_t invalid;
    platform_uint8_t data = 0U;

    printf("\n--- SPI negative tests ---\n");

    invalid = config;
    invalid.clock_frequency = 0U;

    CHECK_TEST("SPI rejects zero clock frequency",
               SPI_Init(&invalid) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    invalid = config;
    invalid.mode = (spi_mode_t)99;

    CHECK_TEST("SPI rejects invalid mode",
               SPI_Init(&invalid) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("SPI remains uninitialized after invalid mode",
               SPI_TransferByte(0U, &data) ==
                   PLATFORM_STATUS_NOT_INITIALIZED);

    invalid = config;
    invalid.bit_order = (spi_bit_order_t)99;

    CHECK_TEST("SPI rejects invalid bit order",
               SPI_Init(&invalid) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("SPI initializes with valid configuration",
               SPI_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("SPI rejects invalid write-register address",
               SPI_WriteRegister(4U, 0x55U) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("SPI rejects null transfer buffers",
               SPI_Transfer(NULL, &data, 1U) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);
}

static void test_i2c_negative(void)
{
    i2c_config_t config = {
        .clock_frequency = 100000U,
        .speed = I2C_SPEED_STANDARD
    };

    i2c_config_t invalid;
    platform_uint8_t data = 0U;

    printf("\n--- I2C negative tests ---\n");

    invalid = config;
    invalid.clock_frequency = 0U;

    CHECK_TEST("I2C rejects zero clock frequency",
               I2C_Init(&invalid) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    invalid = config;
    invalid.speed = (i2c_speed_t)99;

    CHECK_TEST("I2C rejects invalid speed setting",
               I2C_Init(&invalid) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("I2C initializes with valid configuration",
               I2C_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C rejects null read pointer",
               I2C_ReadByte(NULL) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("I2C rejects write without active transaction",
               I2C_WriteByte(0x55U) ==
                   PLATFORM_STATUS_ERROR);

    CHECK_TEST("I2C rejects read without active transaction",
               I2C_ReadByte(&data) ==
                   PLATFORM_STATUS_ERROR);

    CHECK_TEST("I2C rejects stop without active transaction",
               I2C_Stop() == PLATFORM_STATUS_ERROR);

    CHECK_TEST("I2C rejects address outside 7-bit range",
               I2C_Start(0x80U, PLATFORM_FALSE) ==
                   PLATFORM_STATUS_INVALID_ADDRESS);
}

int main(void)
{
    test_uninitialized_calls();
    test_gpio_negative();
    test_uart_negative();
    test_spi_negative();
    test_i2c_negative();

    printf("\n====================================\n");
    printf("Negative tests: %u\n", tests_run);
    printf("Passed:        %u\n", tests_run - tests_failed);
    printf("Failed:        %u\n", tests_failed);
    printf("====================================\n");

    return (tests_failed == 0U) ? 0 : 1;
}