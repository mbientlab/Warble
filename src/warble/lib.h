/**
 * @copyright MbientLab License
 * @file lib.h
 * @brief Warble library attributes
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
WARBLE_API const char* warble_lib_version();
/**
 * Returns the build configuration
 * @return Either 'Release' or 'Debug'
 */
WARBLE_API const char* warble_lib_config();
/**
 * Initializes the warble library
 * @param nopts     Number of options being passed
 * @param opts      Array of config options
 */
WARBLE_API void warble_lib_init(WARBLE_INT nopts, const WarbleOption* opts);

#ifdef __cplusplus
}
#endif
