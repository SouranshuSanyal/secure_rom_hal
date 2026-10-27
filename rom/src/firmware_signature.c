
#include "firmware_signature.h"
#include "stdlib.h"

platform_status_t FIRMWARE_SIGNATURE_Verify(
    const platform_uint8_t *message,
    platform_uint32_t message_size,
    const platform_uint8_t *signature,
    platform_uint32_t signature_size,
    const platform_uint8_t *public_key,
    platform_uint32_t public_key_size,
    firmware_signature_verifier_t verifier)
{
    if ((message == NULL) ||
        (signature == NULL) ||
        (public_key == NULL) ||
        (verifier == NULL))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    if ((message_size == 0U) ||
        (signature_size == 0U) ||
        (public_key_size == 0U))
    {
        return PLATFORM_STATUS_INVALID_ARGUMENT;
    }

    /*
     * The verifier owns the cryptographic operation.
     * Propagate its result to the caller.
     */
    return verifier(
        message,
        message_size,
        signature,
        signature_size,
        public_key,
        public_key_size
    );
}
