
#include <stdio.h>
#include <stddef.h>

#include "periph.h"

int main(void)
{
    volatile platform_uint32_t mock_registers[3] = {0U, 0U, 0U};
    periph_t peripheral;

    if (PERIPH_Init(&peripheral, mock_registers) == PLATFORM_FALSE)
    {
        printf("FAIL: initialization\n");
        return 1;
    }

    PERIPH_Enable(&peripheral);

    if (mock_registers[0] != 1U)
    {
        printf("FAIL: enable\n");
        return 1;
    }

    PERIPH_Write(&peripheral, PERIPH_OUTPUT_OFFSET, 0x5AU);

    if (PERIPH_Read(&peripheral, PERIPH_OUTPUT_OFFSET) != 0x5AU)
    {
        printf("FAIL: output register read/write\n");
        return 1;
    }

    PERIPH_Disable(&peripheral);

    if ((mock_registers[0] & 1U) != 0U)
    {
        printf("FAIL: disable\n");
        return 1;
    }

    PERIPH_Write(&peripheral, PERIPH_STATUS_OFFSET, 0xFFFFFFFFU);

    if (mock_registers[1] != 0U)
    {
        printf("FAIL: read-only status protection\n");
        return 1;
    }

    printf("PASS: peripheral HAL mock-register tests\n");
    return 0;
}
