#pragma once

// Renderer API export/import macro
#ifndef RENDERER_API
#if defined(_WIN32) || defined(_WIN64)
    #ifdef RENDERER_BUILD_SHARED
        #define RENDERER_API __declspec(dllexport)
    #elif defined(RENDERER_IMPORT_SHARED)
        #define RENDERER_API __declspec(dllimport)
    #else
        #define RENDERER_API
    #endif
#else
    #define RENDERER_API
#endif
#endif
