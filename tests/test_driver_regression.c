#include <stdio.h>
#include <stdint.h>

#include "platform_status.h"
#include "gpio_types.h"
#include "gpio.h"
#include "uart.h"
#include "uart_types.h"
#include "spi.h"
#include "spi_types.h"
#include "i2c.h"
#include "i2c_types.h"

static unsigned int tests_run = 0U;
static unsigned int tests_failed = 0U;

#define CHECK_TEST(description, expression)                         \
    do                                                              \
    {                                                               \
        tests_run++;                                                \
        if (expression)                                             \
        {                                                           \
            printf("[PASS] %s\n", description);                     \
        }                                                           \
        else                                                        \
        {                                                           \
            printf("[FAIL] %s (line %d)\n", description, __LINE__); \
            tests_failed++;                                         \
        }                                                           \
    } while (0)

static void test_gpio(void)
{
    gpio_pin_level_t level = GPIO_PIN_LOW;

    printf("\n--- GPIO tests ---\n");

    CHECK_TEST("GPIO output initialization",
               GPIO_Init(3U, GPIO_OUTPUT) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("GPIO write HIGH",
               GPIO_Write(3U, GPIO_PIN_HIGH) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("GPIO read HIGH",
               GPIO_Read(3U, &level) == PLATFORM_STATUS_SUCCESS &&
               level == GPIO_PIN_HIGH);

    CHECK_TEST("GPIO toggle",
               GPIO_Toggle(3U) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("GPIO reads LOW after toggle",
               GPIO_Read(3U, &level) == PLATFORM_STATUS_SUCCESS &&
               level == GPIO_PIN_LOW);

    CHECK_TEST("GPIO rejects pin 32",
               GPIO_Write(32U, GPIO_PIN_HIGH) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("GPIO read rejects null output",
               GPIO_Read(3U, NULL) == PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("GPIO rejects invalid direction",
               GPIO_Init(2U, (gpio_direction_t)99) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);
}

static void test_uart(void)
{
    uart_config_t config = {
        .baudrate = 115200U,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1
    };

    uart_config_t invalid_config = config;
    platform_uint8_t received = 0U;

    printf("\n--- UART tests ---\n");

    CHECK_TEST("UART rejects null configuration",
               UART_Init(NULL) == PLATFORM_STATUS_INVALID_ARGUMENT);

    invalid_config.baudrate = 0U;
    CHECK_TEST("UART rejects zero baud rate",
               UART_Init(&invalid_config) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("UART initializes with valid configuration",
               UART_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART reports empty RX buffer",
               UART_GetRxBufferStatus() == PLATFORM_STATUS_BUSY);

    CHECK_TEST("UART rejects null RX pointer",
               UART_ReadByte(NULL) == PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("UART rejects reading from empty RX buffer",
               UART_ReadByte(&received) == PLATFORM_STATUS_BUSY);

    CHECK_TEST("UART accepts RX byte",
               UART_InjectRxByte(0xA5U) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART reports non-empty RX buffer",
               UART_GetRxBufferStatus() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART reads injected byte",
               UART_ReadByte(&received) == PLATFORM_STATUS_SUCCESS &&
               received == 0xA5U);

    CHECK_TEST("UART RX buffer is empty after read",
               UART_GetRxBufferStatus() == PLATFORM_STATUS_BUSY);

    CHECK_TEST("UART transmits byte",
               UART_WriteByte(0x5AU) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART rejects null string",
               UART_WriteString(NULL) == PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("UART can force TX busy",
               UART_ForceTxBusy() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART write returns BUSY when TX is busy",
               UART_WriteByte(0x33U) == PLATFORM_STATUS_BUSY);

    CHECK_TEST("UART timeout expires when TX is busy",
               UART_WriteByteTimeout(0x33U, 0U) ==
                   PLATFORM_STATUS_TIMEOUT);

    /* Reinitialize to restore the simulated TX-empty state. */
    CHECK_TEST("UART reinitializes after busy test",
               UART_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("UART transmits after reinitialization",
               UART_WriteByte(0x44U) == PLATFORM_STATUS_SUCCESS);
}

static void test_spi(void)
{
    spi_config_t config = {
        .clock_frequency = 1000000U,
        .mode = SPI_MODE_0,
        .bit_order = SPI_BIT_ORDER_MSB_FIRST
    };

    platform_uint8_t received = 0U;

    printf("\n--- SPI tests ---\n");

    CHECK_TEST("SPI rejects null configuration",
               SPI_Init(NULL) == PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("SPI initializes with valid configuration",
               SPI_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("SPI transfer rejects null RX pointer",
               SPI_TransferByte(0x00U, NULL) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("SPI transfer requires chip select",
               SPI_TransferByte(0x00U, &received) ==
                   PLATFORM_STATUS_ERROR);

    CHECK_TEST("SPI chip select enables",
               SPI_ChipSelect(PLATFORM_TRUE) ==
                   PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("SPI reads simulated register 0",
               SPI_ReadRegister(0U, &received) ==
                   PLATFORM_STATUS_SUCCESS &&
               received == 0xA5U);

    CHECK_TEST("SPI reads simulated register 1",
               SPI_ReadRegister(1U, &received) ==
                   PLATFORM_STATUS_SUCCESS &&
               received == 0x01U);

    CHECK_TEST("SPI reads simulated register 3",
               SPI_ReadRegister(3U, &received) ==
                   PLATFORM_STATUS_SUCCESS &&
               received == 0x42U);

    CHECK_TEST("SPI rejects register address 4",
               SPI_ReadRegister(4U, &received) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("SPI register read rejects null pointer",
               SPI_ReadRegister(0U, NULL) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    {
        const platform_uint8_t tx[3] = {0x00U, 0x01U, 0x02U};
        platform_uint8_t rx[3] = {0U, 0U, 0U};

        CHECK_TEST("SPI rejects zero-length transfer",
                   SPI_Transfer(tx, rx, 0U) ==
                       PLATFORM_STATUS_INVALID_ARGUMENT);

        CHECK_TEST("SPI rejects null TX buffer",
                   SPI_Transfer(NULL, rx, 3U) ==
                       PLATFORM_STATUS_INVALID_ARGUMENT);

        CHECK_TEST("SPI rejects null RX buffer",
                   SPI_Transfer(tx, NULL, 3U) ==
                       PLATFORM_STATUS_INVALID_ARGUMENT);
    }

    CHECK_TEST("SPI chip select disables",
               SPI_ChipSelect(PLATFORM_FALSE) ==
                   PLATFORM_STATUS_SUCCESS);
}

static void test_i2c(void)
{
    i2c_config_t config = {
        .clock_frequency = 100000U,
        .speed = I2C_SPEED_STANDARD
    };

    platform_uint8_t received = 0U;

    printf("\n--- I2C tests ---\n");

    CHECK_TEST("I2C rejects null configuration",
               I2C_Init(NULL) == PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("I2C initializes with valid configuration",
               I2C_Init(&config) == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C rejects address above 7-bit range",
               I2C_Start(0x80U, PLATFORM_FALSE) ==
                   PLATFORM_STATUS_INVALID_ADDRESS);

    CHECK_TEST("I2C rejects unsupported device address",
               I2C_Start(0x51U, PLATFORM_FALSE) ==
                   PLATFORM_STATUS_NACK);

    CHECK_TEST("I2C reads simulated register 0",
               I2C_ReadRegister(0x50U, 0U, &received) ==
                   PLATFORM_STATUS_SUCCESS &&
               received == 0x42U);

    CHECK_TEST("I2C reads simulated register 3",
               I2C_ReadRegister(0x50U, 3U, &received) ==
                   PLATFORM_STATUS_SUCCESS &&
               received == 0x19U);

    CHECK_TEST("I2C register read rejects null pointer",
               I2C_ReadRegister(0x50U, 0U, NULL) ==
                   PLATFORM_STATUS_INVALID_ARGUMENT);

    CHECK_TEST("I2C writes register",
               I2C_WriteRegister(0x50U, 1U, 0x5AU) ==
                   PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C reads back written register",
               I2C_ReadRegister(0x50U, 1U, &received) ==
                   PLATFORM_STATUS_SUCCESS &&
               received == 0x5AU);

    CHECK_TEST("I2C rejects invalid device register",
               I2C_WriteRegister(0x50U, 5U, 0x11U) ==
                   PLATFORM_STATUS_ERROR);

    CHECK_TEST("I2C can force bus busy",
               I2C_ForceBusBusy() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C start returns BUSY on busy bus",
               I2C_Start(0x50U, PLATFORM_FALSE) ==
                   PLATFORM_STATUS_BUSY);

    CHECK_TEST("I2C reset clears busy bus",
               I2C_ResetBus() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C can force NACK",
               I2C_ForceNack() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C forced NACK returns error",
               I2C_Start(0x50U, PLATFORM_FALSE) ==
                   PLATFORM_STATUS_ERROR);

    CHECK_TEST("I2C reset clears forced NACK",
               I2C_ResetBus() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C can force timeout",
               I2C_ForceTimeout() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C forced timeout returns TIMEOUT",
               I2C_StartTimeout(0x50U, PLATFORM_FALSE, 0U) ==
                   PLATFORM_STATUS_TIMEOUT);

    CHECK_TEST("I2C reset clears forced timeout",
               I2C_ResetBus() == PLATFORM_STATUS_SUCCESS);

    CHECK_TEST("I2C works after fault recovery",
               I2C_ReadRegister(0x50U, 0U, &received) ==
                   PLATFORM_STATUS_SUCCESS &&
               received == 0x42U);
}

int main(void)
{
    test_gpio();
    test_uart();
    test_spi();
    test_i2c();

    printf("\n====================================\n");
    printf("Driver regression tests: %u\n", tests_run);
    printf("Passed: %u\n", tests_run - tests_failed);
    printf("Failed: %u\n", tests_failed);
    printf("====================================\n");

    return (tests_failed == 0U) ? 0 : 1;
}