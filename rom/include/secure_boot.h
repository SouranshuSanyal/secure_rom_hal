
#ifndef SECURE_BOOT_H
#define SECURE_BOOT_H

#include "firmware_image.h"
#include "firmware_signature.h"

typedef enum
{
    SECURE_BOOT_FAILURE_NONE = 0,
    SECURE_BOOT_FAILURE_INPUT,
    SECURE_BOOT_FAILURE_HEADER,
    SECURE_BOOT_FAILURE_INTEGRITY,
    SECURE_BOOT_FAILURE_SIGNATURE
} secure_boot_failure_t;

platform_status_t SECURE_BOOT_ValidateImage(
    const firmware_image_header_t *header,
    const platform_uint8_t *payload,
    platform_uint32_t payload_size,
    const platform_uint8_t *signature,
    platform_uint32_t signature_size,
    const platform_uint8_t *public_key,
    platform_uint32_t public_key_size,
    firmware_signature_verifier_t verifier);

secure_boot_failure_t SECURE_BOOT_GetLastFailure(void);

#endif
