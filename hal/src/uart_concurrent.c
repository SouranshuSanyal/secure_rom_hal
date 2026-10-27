#include "uart_concurrent.h"
#include "uart.h"
#include "osal.h"
#include "stddef.h"

/*
 * Executes the UART operation inside an OSAL critical section.
 *
 * If the operation fails, its error is preserved.
 * If the operation succeeds but exiting the critical section fails,
 * the exit error is returned.
 */
static platform_status_t UART_CONCURRENT_Execute(
    platform_status_t (*operation)(void *context),
    void *context)
{
    platform_uint32_t critical_state = 0U;
    platform_status_t operation_status;
    platform_status_t exit_status;

    if (operation == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    operation_status = OSAL_CriticalEnter(&critical_state);

    if (operation_status != PLATFORM_STATUS_SUCCESS)
    {
        return operation_status;
    }

    operation_status = operation(context);

    exit_status = OSAL_CriticalExit(critical_state);

    if (operation_status != PLATFORM_STATUS_SUCCESS)
    {
        return operation_status;
    }

    return exit_status;
}

static platform_status_t UART_CONCURRENT_ReadOperation(
    void *context)
{
    platform_uint8_t *data = (platform_uint8_t *)context;

    return UART_ReadByte(data);
}

static platform_status_t UART_CONCURRENT_InjectOperation(
    void *context)
{
    platform_uint8_t data = *(platform_uint8_t *)context;

    return UART_InjectRxByte(data);
}

platform_status_t UART_CONCURRENT_ReadByte(
    platform_uint8_t *data)
{
    if (data == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    return UART_CONCURRENT_Execute(
        UART_CONCURRENT_ReadOperation,
        data);
}

platform_status_t UART_CONCURRENT_InjectRxByte(
    platform_uint8_t data)
{
    return UART_CONCURRENT_Execute(
        UART_CONCURRENT_InjectOperation,
        &data);
}