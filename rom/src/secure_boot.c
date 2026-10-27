
#include "secure_boot.h"
#include "stdlib.h"

#include "firmware_integrity.h"

static secure_boot_failure_t last_failure =
    SECURE_BOOT_FAILURE_NONE;

secure_boot_failure_t SECURE_BOOT_GetLastFailure(void)
{
    return last_failure;
}

platform_status_t SECURE_BOOT_ValidateImage(
    const firmware_image_header_t *header,
    const platform_uint8_t *payload,
    platform_uint32_t payload_size,
    const platform_uint8_t *signature,
    platform_uint32_t signature_size,
    const platform_uint8_t *public_key,
    platform_uint32_t public_key_size,
    firmware_signature_verifier_t verifier)
{
    platform_status_t status;

    last_failure = SECURE_BOOT_FAILURE_NONE;

    /* Reject missing inputs before performing any verification. */
    if ((header == NULL) ||
        (payload == NULL) ||
        (signature == NULL) ||
        (public_key == NULL) ||
        (verifier == NULL))
    {
        last_failure = SECURE_BOOT_FAILURE_INPUT;
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /* Stage 1: Validate firmware metadata. */
    status = FIRMWARE_IMAGE_Validate(header);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        last_failure = SECURE_BOOT_FAILURE_HEADER;
        return status;
    }

    /* Stage 2: Verify payload integrity. */
    status = FIRMWARE_IMAGE_VerifyIntegrity(
        header,
        payload,
        payload_size);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        last_failure = SECURE_BOOT_FAILURE_INTEGRITY;
        return status;
    }

    /*
     * Stage 3: Verify the signature through the supplied verifier.
     *
     * This demonstration verifies the payload bytes. A production
     * format must cryptographically bind the security-relevant
     * header fields and payload together.
     */
    status = FIRMWARE_SIGNATURE_Verify(
        payload,
        payload_size,
        signature,
        signature_size,
        public_key,
        public_key_size,
        verifier);

    if (status != PLATFORM_STATUS_SUCCESS)
    {
        last_failure = SECURE_BOOT_FAILURE_SIGNATURE;
        return status;
    }

    /* All configured validation stages passed. */
    last_failure = SECURE_BOOT_FAILURE_NONE;

    return PLATFORM_STATUS_SUCCESS;
}
