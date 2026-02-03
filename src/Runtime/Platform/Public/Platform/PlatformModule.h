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

namespace CarrotToy {
namespace Platform {

// Platform module interface
class IPlatformModule {
public:
    virtual ~IPlatformModule() = default;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual const char* getName() const = 0;
};

// Factory function to create platform module
PLATFORM_API IPlatformModule* createPlatformModule();

} // namespace Platform
} // namespace CarrotToy
