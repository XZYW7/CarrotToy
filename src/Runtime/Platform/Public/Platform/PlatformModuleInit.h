#pragma once

#ifndef PLATFORM_API
    #if defined(_WIN32) || defined(_WIN64)
        #ifdef PLATFORM_BUILD_SHARED
            #define PLATFORM_API __declspec(dllexport)
        #elif defined(PLATFORM_IMPORT_SHARED)
            #define PLATFORM_API __declspec(dllimport)
        #else
            #define PLATFORM_API
        #endif
    #else
        #define PLATFORM_API
    #endif
#endif

/**
 * Initialize the Platform module and register it with ModuleManager.
 * Must be called explicitly to ensure proper DLL loading and module registration.
 */
extern "C" PLATFORM_API void InitializeModulePlatform();
