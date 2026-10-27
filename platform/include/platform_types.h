#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t platform_uint8_t;
typedef uint16_t platform_uint16_t;
typedef uint32_t platform_uint32_t;
typedef uint64_t platform_uint64_t;

typedef int8_t platform_int8_t;
typedef int16_t platform_int16_t;
typedef int32_t platform_int32_t;
typedef int64_t platform_int64_t;

typedef bool platform_bool_t;

#define PLATFORM_TRUE true
#define PLATFORM_FALSE false

#endif  /*PLATFORM_TYPES_H*/