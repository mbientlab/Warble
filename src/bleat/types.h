#pragma once

#ifdef __cplusplus

#include <cstdint>
#define UBYTE std::uint8_t
#define USHORT std::uint16_t
#define UINT std::uint32_t
#define INT std::int32_t

#else

#include <stdint.h>
#define UBYTE uint8_t
#define USHORT uint16_t
#define UINT uint32_t
#define INT int32_t

#endif