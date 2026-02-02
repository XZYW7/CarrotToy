#pragma once

// Editor API export/import macro
#ifndef EDITOR_API
#if defined(_WIN32) || defined(_WIN64)
    #ifdef EDITOR_BUILD_SHARED
        #define EDITOR_API __declspec(dllexport)
    #elif defined(EDITOR_IMPORT_SHARED)
        #define EDITOR_API __declspec(dllimport)
    #else
        #define EDITOR_API
    #endif
#else
    #define EDITOR_API
#endif
#endif
