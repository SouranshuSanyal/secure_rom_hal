
#include "boot_flow.h"
#include "stdlib.h"

#include "rom_api.h"
#include "platform_types.h"

static boot_state_t boot_state = BOOT_STATE_RESET;

boot_state_t BOOT_GetState(void)
{
    return boot_state;
}

platform_status_t BOOT_Run(const uart_config_t *config)
{
    const rom_api_table_t *api = NULL;
    platform_status_t status;

    /*
     * Start a new boot attempt.
     * Argument validation occurs before ROM initialization,
     * allowing an invalid configuration to be rejected safely.
     */
    boot_state = BOOT_STATE_RESET;

    if ((config == NULL) || (config->baudrate == 0U))
    {
        boot_state = BOOT_STATE_FAILED;
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* Step 1: Initialize ROM services. */
    status = ROM_API_Init();

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        boot_state = BOOT_STATE_FAILED;
        return status;
    }

    boot_state = BOOT_STATE_ROM_READY;

    /* Step 2: Discover the ROM API table. */
    status = ROM_API_GetTable(&api);

    if ((status != PLATFORM_STATUS_SUCCESS) || (api == NULL))
    {
        boot_state = BOOT_STATE_FAILED;

        if (status == PLATFORM_STATUS_SUCCESS)
        {
            return PLATFORM_STATUS_ERROR;
        }

        return status;
    }

    boot_state = BOOT_STATE_API_READY;

    /* Step 3: Initialize the required peripheral. */
    status = api->uart_init(config);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        boot_state = BOOT_STATE_FAILED;
        return status;
    }

    boot_state = BOOT_STATE_PERIPHERAL_READY;

    /* Step 4: Declare the application ready. */
    boot_state = BOOT_STATE_APPLICATION_READY;

    return PLATFORM_STATUS_SUCCESS;
}
