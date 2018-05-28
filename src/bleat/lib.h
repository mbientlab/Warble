/**
 * @copyright MbientLab License
 * @file lib.h
 * @brief Bleat library attributes
 */
#pragma once

#include "dllmarker.h"
#include "types.h"

#ifdef __cplusplus
struct BleatObject;
#else
typedef struct BleatObject BleatObject;
#endif

BLEAT_API const char* bleat_lib_version();
BLEAT_API const char* bleat_lib_config();
BLEAT_API void bleat_lib_init(BLEAT_INT nopts, const BleatOption* opts);