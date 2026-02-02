#pragma once

// Launch API export/import macro
#ifndef LAUNCH_API
#if defined(_WIN32) || defined(_WIN64)
    #ifdef LAUNCH_BUILD_SHARED
        #define LAUNCH_API __declspec(dllexport)
    #elif defined(LAUNCH_IMPORT_SHARED)
        #define LAUNCH_API __declspec(dllimport)
    #else
        #define LAUNCH_API
    #endif
#else
    #define LAUNCH_API
#endif
#endif
