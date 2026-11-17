#pragma once

#if defined(_WIN32) || defined(_WIN64)

#ifdef JCBASEJSON_EXPORTS
#define wheels __declspec(dllexport)
#else
#define wheels __declspec(dllimport)
#endif

#else

#if defined(__GNUC__) || defined(__clang__)

#ifdef JCBASEJSON_EXPORTS
#define wheels __attribute__((visibility("default")))
#else
#define wheels
#endif

#else
#define wheels
#endif

#endif
