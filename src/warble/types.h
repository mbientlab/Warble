/**
 * @copyright MbientLab License
 * @file types.h
 * @brief Macros for defining variable types compatible with C and C++ compilers  
 */
#pragma once

#ifdef __cplusplus

#include <cstdint>
#define WARBLE_UBYTE std::uint8_t
#define WARBLE_USHORT std::uint16_t
#define WARBLE_UINT std::uint32_t
#define WARBLE_INT std::int32_t

#else

#include <stdint.h>
#define WARBLE_UBYTE uint8_t
#define WARBLE_USHORT uint16_t
#define WARBLE_UINT uint32_t
#define WARBLE_INT int32_t

#endif

/**
 * Simple string pair used to configure the API
 */
typedef struct {
    const char* key;            ///< Option identifier
    const char* value;          ///< Option value
} WarbleOption;