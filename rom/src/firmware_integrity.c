
#include "firmware_integrity.h"
#include "stdlib.h"

platform_status_t FIRMWARE_CRC32_Calculate(
    const platform_uint8_t *data,
    platform_uint32_t length,
    platform_uint32_t *crc)
{
    platform_uint32_t value = 0xFFFFFFFFU;
    platform_uint32_t i;
    platform_uint32_t bit;

    if ((data == NULL) || (crc == NULL) || (length == 0U))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    for (i = 0U; i < length; i++)
    {
        value ^= (platform_uint32_t)data[i];

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((value & 1U) != 0U)
            {
                value = (value >> 1U) ^ 0xEDB88320U;
            }
            else
            {
                value >>= 1U;
            }
        }
    }

    *crc = value ^ 0xFFFFFFFFU;

    return PLATFORM_STATUS_SUCCESS;
}

platform_status_t FIRMWARE_IMAGE_VerifyIntegrity(
    const firmware_image_header_t *header,
    const platform_uint8_t *payload,
    platform_uint32_t payload_size)
{
    platform_uint32_t calculated_crc;
    platform_status_t status;

    if ((header == NULL) || (payload == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* Validate metadata before checking the payload. */
    status = FIRMWARE_IMAGE_Validate(header);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        return status;
    }

    /* The supplied payload length must match the header. */
    if (payload_size != header->image_size)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    status = FIRMWARE_CRC32_Calculate(
        payload,
        payload_size,
        &calculated_crc);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        return status;
    }

    if (calculated_crc != header->crc32)
    {
        return PLATFORM_STATUS_ERROR;
    }

    return PLATFORM_STATUS_SUCCESS;
}
