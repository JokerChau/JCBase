#pragma once

#ifdef WHEELS_EXPORTS
#define wheels __declspec(dllexport)
#else
#define wheels __declspec(dllimport)
#endif