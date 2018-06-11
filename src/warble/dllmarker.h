/**
 * @copyright MbientLab License
 * @file dllmarker.h
 * @brief Macros for identifying exported functions
 */
#pragma once

#if defined _WIN32 || defined __CYGWIN__
#define WARBLE_HELPER_DLL_IMPORT __declspec(dllimport)
#define WARBLE_HELPER_DLL_EXPORT __declspec(dllexport)
#define WARBLE_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define WARBLE_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define WARBLE_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define WARBLE_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define WARBLE_HELPER_DLL_IMPORT
#define WARBLE_HELPER_DLL_EXPORT
#define WARBLE_HELPER_DLL_LOCAL
#endif
#endif

#if defined _WINDLL || defined WARBLE_DLL // defined if WARBLE is compiled as a DLL
#ifdef WARBLE_DLL_EXPORTS
/** Indicates the function should be exported to the symbol table  */
#define WARBLE_API WARBLE_HELPER_DLL_EXPORT
#else
#define WARBLE_API WARBLE_HELPER_DLL_IMPORT
#endif // WARBLE_DLL_EXPORTS
/** Indicates the function is only to be used by the API */
#define WARBLE_LOCAL WARBLE_HELPER_DLL_LOCAL
#else // WARBLE_DLL is not defined: this means WARBLE is a static lib.
#define WARBLE_API
#define WARBLE_LOCAL
#endif // WARBLE_DLL
