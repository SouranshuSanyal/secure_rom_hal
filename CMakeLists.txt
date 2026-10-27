cmake_minimum_required(VERSION 3.16)

project(
    secure_rom_hal
    VERSION 0.1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(
    secure_rom_hal
    app/main.c
    platform/src/platform_register.c
    platform/src/platform_timeout.c
    hal/src/gpio.c
    hal/src/uart.c
    hal/src/spi.c
    simulation/src/spi_device.c
    hal/src/i2c.c
    simulation/src/i2c_device.c
    platform/src/platform_critical.c
    platform/src/platform_ring_buffer.c
    rom/src/rom_api.c
    rom/src/boot_flow.c
    rom/src/firmware_image.c
    rom/src/firmware_integrity.c
    rom/src/firmware_signature.c
    rom/src/secure_boot.c
    osal/src/osal.c
    osal/src/osal_baremetal.c
    osal/src/osal_rtos.c
    hal/src/uart_concurrent.c
)

target_include_directories(
    secure_rom_hal
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/include
        ${CMAKE_CURRENT_SOURCE_DIR}/rom/include
        ${CMAKE_CURRENT_SOURCE_DIR}/osal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/osal/include
)

add_executable(
    test_uart_concurrent
    tests/test_uart_concurrent.c
    platform/src/platform_ring_buffer.c
    hal/src/uart.c
    hal/src/uart_concurrent.c
    osal/src/osal.c
)

target_include_directories(
    test_uart_concurrent
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/osal/include
)

add_executable(
    test_ring_buffer
    tests/test_ring_buffer.c
    platform/src/platform_ring_buffer.c
)

target_include_directories(
    test_ring_buffer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
)


add_executable(
    test_driver_regression
    tests/test_driver_regression.c
    platform/src/platform_register.c
    platform/src/platform_timeout.c
    platform/src/platform_ring_buffer.c
    hal/src/gpio.c
    hal/src/uart.c
    hal/src/spi.c
    hal/src/i2c.c
    simulation/src/spi_device.c
    simulation/src/i2c_device.c
)

target_include_directories(
    test_driver_regression
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/include
)

add_executable(
    test_driver_negative
    tests/test_driver_negative.c
    platform/src/platform_register.c
    platform/src/platform_timeout.c
    platform/src/platform_ring_buffer.c
    hal/src/gpio.c
    hal/src/uart.c
    hal/src/spi.c
    hal/src/i2c.c
    simulation/src/spi_device.c
    simulation/src/i2c_device.c
)

target_include_directories(
    test_driver_negative
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/include
)

add_executable(
    test_driver_recovery
    tests/test_driver_recovery.c
    platform/src/platform_register.c
    platform/src/platform_timeout.c
    platform/src/platform_ring_buffer.c
    hal/src/gpio.c
    hal/src/uart.c
    hal/src/spi.c
    hal/src/i2c.c
    simulation/src/spi_device.c
    simulation/src/i2c_device.c
)

target_include_directories(
    test_driver_recovery
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/include
)


# Day 34: Automated test integration with CTest

enable_testing()

add_test(
    NAME UART_Concurrent_Test
    COMMAND test_uart_concurrent
)

add_test(
    NAME Ring_Buffer_Test
    COMMAND test_ring_buffer
)

add_test(
    NAME Driver_Regression_Test
    COMMAND test_driver_regression
)

add_test(
    NAME Driver_Negative_Test
    COMMAND test_driver_negative
)

add_test(
    NAME Driver_Recovery_Test
    COMMAND test_driver_recovery
)

# Day 35: Strict compiler warnings
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
)

add_executable(
    test_platform_utilities
    tests/test_platform_utilities.c
    platform/src/platform_timeout.c
    platform/src/platform_critical.c
)

target_include_directories(
    test_platform_utilities
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
)

add_test(
    NAME Platform_Utilities_Test
    COMMAND test_platform_utilities
)

add_executable(
    test_periph
    tests/test_periph.c
    hal/src/periph.c
)

target_include_directories(
    test_periph
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
)

add_test(
    NAME Peripheral_HAL_Test
    COMMAND test_periph
)

add_executable(test_axi_secure_driver
    tests/test_axi_secure_driver.c
    hal/src/axi_secure_driver.c
    hal/src/periph.c
)

target_include_directories(test_axi_secure_driver PRIVATE
    platform/include
    hal/include
)

add_test(
    NAME test_axi_secure_driver
    COMMAND test_axi_secure_driver
)

add_executable(test_axi_secure_negative
    tests/test_axi_secure_negative.c
    hal/src/axi_secure_driver.c
    hal/src/periph.c
)

target_include_directories(test_axi_secure_negative PRIVATE
    platform/include
    hal/include
)

add_test(
    NAME test_axi_secure_negative
    COMMAND test_axi_secure_negative
)