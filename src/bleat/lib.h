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

/**
 * Returns the current library version
 * @return Version string in `x.y.z` format
 */
BLEAT_API const char* bleat_lib_version();
/**
 * Returns the build configuration
 * @return Either 'Release' or 'Debug'
 */
BLEAT_API const char* bleat_lib_config();
/**
 * Initializes the bleat library
 * @param nopts     Number of options being passed
 * @param opts      Array of config options
 */
BLEAT_API void bleat_lib_init(BLEAT_INT nopts, const BleatOption* opts);

#ifdef __cplusplus
}
#endif
