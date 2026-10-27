
#ifndef FIRMWARE_SIGNATURE_H
#define FIRMWARE_SIGNATURE_H

#include "platform_types.h"
#include "platform_status.h"

/*
 * A platform or cryptographic-library implementation supplies
 * this callback. The callback must perform real signature
 * verification in a production implementation.
 */
typedef platform_status_t (*firmware_signature_verifier_t)(
    const platform_uint8_t *message,
    platform_uint32_t message_size,
    const platform_uint8_t *signature,
    platform_uint32_t signature_size,
    const platform_uint8_t *public_key,
    platform_uint32_t public_key_size
);

/*
 * Validate the inputs and delegate cryptographic verification
 * to the supplied verifier.
 */
platform_status_t FIRMWARE_SIGNATURE_Verify(
    const platform_uint8_t *message,
    platform_uint32_t message_size,
    const platform_uint8_t *signature,
    platform_uint32_t signature_size,
    const platform_uint8_t *public_key,
    platform_uint32_t public_key_size,
    firmware_signature_verifier_t verifier
);

#endif
