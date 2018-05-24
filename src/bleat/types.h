/**
 * @copyright MbientLab License
 * @file types.h
 * @brief Macros for defining variable types compatible with C and C++ compilers  
 */
#pragma once

#ifdef __cplusplus

#include <cstdint>
#define BLEAT_UBYTE std::uint8_t
#define BLEAT_USHORT std::uint16_t
#define BLEAT_UINT std::uint32_t
#define BLEAT_INT std::int32_t

#else

#include <stdint.h>
#define BLEAT_UBYTE uint8_t
#define BLEAT_USHORT uint16_t
#define BLEAT_UINT uint32_t
#define BLEAT_INT int32_t

#endif