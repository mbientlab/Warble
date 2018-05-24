/**
 * @copyright MbientLab License
 * @file dllmarker.h
 * @brief Macros for identifying exported functions
 */
#pragma once

#if defined _WIN32 || defined __CYGWIN__
#define BLEAT_HELPER_DLL_IMPORT __declspec(dllimport)
#define BLEAT_HELPER_DLL_EXPORT __declspec(dllexport)
#define BLEAT_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define BLEAT_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define BLEAT_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define BLEAT_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define BLEAT_HELPER_DLL_IMPORT
#define BLEAT_HELPER_DLL_EXPORT
#define BLEAT_HELPER_DLL_LOCAL
#endif
#endif

#if defined _WINDLL || defined BLEAT_DLL // defined if BLEAT is compiled as a DLL
#ifdef BLEAT_DLL_EXPORTS
/** Indicates the function should be exported to the symbol table  */
#define BLEAT_API BLEAT_HELPER_DLL_EXPORT
#else
#define BLEAT_API BLEAT_HELPER_DLL_IMPORT
#endif // BLEAT_DLL_EXPORTS
/** Indicates the function is only to be used by the API */
#define BLEAT_LOCAL BLEAT_HELPER_DLL_LOCAL
#else // BLEAT_DLL is not defined: this means BLEAT is a static lib.
#define BLEAT_API
#define BLEAT_LOCAL
#endif // BLEAT_DLL
