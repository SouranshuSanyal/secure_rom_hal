
#ifndef FIRMWARE_INTEGRITY_H
#define FIRMWARE_INTEGRITY_H

#include "firmware_image.h"

platform_status_t FIRMWARE_CRC32_Calculate(
    const platform_uint8_t *data,
    platform_uint32_t length,
    platform_uint32_t *crc);

platform_status_t FIRMWARE_IMAGE_VerifyIntegrity(
    const firmware_image_header_t *header,
    const platform_uint8_t *payload,
    platform_uint32_t payload_size);

#endif
