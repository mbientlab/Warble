/**
 * @copyright MbientLab License
 * @file lib.h
 * @brief Bleat library attributes
 */
#pragma once

#include "dllmarker.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

BLEAT_API const char* bleat_lib_version();
BLEAT_API const char* bleat_lib_config();
BLEAT_API void bleat_lib_init(BLEAT_INT nopts, const BleatOption* opts);

#ifdef __cplusplus
}
#endif
