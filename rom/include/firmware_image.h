
#ifndef FIRMWARE_IMAGE_H
#define FIRMWARE_IMAGE_H

#include "platform_types.h"
#include "platform_status.h"

/* Firmware image identification. */
#define FIRMWARE_IMAGE_MAGIC          0x5348524DU
#define FIRMWARE_IMAGE_FORMAT_VERSION 1U

/* No optional flags are currently defined. */
#define FIRMWARE_IMAGE_SUPPORTED_FLAGS 0U

typedef struct
{
    platform_uint32_t magic;
    platform_uint32_t format_version;
    platform_uint32_t header_size;
    platform_uint32_t image_size;
    platform_uint32_t load_address;
    platform_uint32_t entry_point;
    platform_uint32_t firmware_version;
    platform_uint32_t flags;
    platform_uint32_t crc32;

} firmware_image_header_t;

/* Validate the metadata in a firmware image header. */
platform_status_t FIRMWARE_IMAGE_Validate(
    const firmware_image_header_t *header);

#endif
