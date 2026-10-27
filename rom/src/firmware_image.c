
#include "firmware_image.h"
#include "stdlib.h"

platform_status_t FIRMWARE_IMAGE_Validate(
    const firmware_image_header_t *header)
{
    if (header == NULL)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* Check the image identifier. */
    if (header->magic != FIRMWARE_IMAGE_MAGIC)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* Reject firmware formats this implementation does not support. */
    if (header->format_version != FIRMWARE_IMAGE_FORMAT_VERSION)
    {
        return PLATFORM_STATUS_NOT_SUPPORTED;
    }

    /* The header must match the current structure size. */
    if (header->header_size !=
        (platform_uint32_t)sizeof(firmware_image_header_t))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* An empty payload is not a valid firmware image. */
    if (header->image_size == 0U)
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* A load address of zero is not supported by this format. */
    if (header->load_address == 0U)
    {
        return PLATFORM_STATUS_INVALID_ADDRESS;
    }

    /*
     * Avoid calculating load_address + image_size, which could overflow.
     * The entry point must lie inside the payload's address range.
     */
    if (header->entry_point < header->load_address)
    {
        return PLATFORM_STATUS_INVALID_ADDRESS;
    }

    if ((header->entry_point - header->load_address) >=
        header->image_size)
    {
        return PLATFORM_STATUS_INVALID_ADDRESS;
    }

    /* Reject flags that this version of the format does not define. */
    if (header->flags != FIRMWARE_IMAGE_SUPPORTED_FLAGS)
    {
        return PLATFORM_STATUS_NOT_SUPPORTED;
    }

    return PLATFORM_STATUS_SUCCESS;
}
