#pragma once

#include "PlatformTypes.h"
#include <memory>
#include <functional>

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

// Forward declaration
class IPlatformWindow;

/**
 * PlatformContext - Manages platform and graphics context initialization
 * 
 * This class encapsulates GLFW initialization and GLAD loading to provide
 * a single point of initialization for the graphics subsystem.
 * 
 * Usage:
 *   1. Create context via createPlatformContext()
 *   2. Call initializeGraphicsContext() with a window
 *   3. Graphics context (GLAD) is ready for use
 */
class PLATFORM_API PlatformContext {
public:
    using ProcAddressLoader = void* (*)(const char* name);

    virtual ~PlatformContext() = default;
    
    /**
     * Initialize GLFW and platform-specific systems
     * Must be called before creating any windows
     */
    virtual bool initializePlatform() = 0;
    
    /**
     * Shutdown platform and cleanup resources
     */
    virtual void shutdownPlatform() = 0;
    
    /**
     * Initialize graphics context (GLAD) for the given window
     * Window must have an active OpenGL context before calling this
     * 
     * @param window Window with active OpenGL context
     * @return true if GLAD was successfully initialized
     */
    virtual bool initializeGraphicsContext(IPlatformWindow* window) = 0;
    
    /**
     * Get the procedure address loader function
     * This can be passed to other systems (like RHI) that need
     * to load OpenGL functions in separate DLLs
     */
    virtual ProcAddressLoader getProcAddressLoader() const = 0;
    
    /**
     * Check if platform is initialized
     */
    virtual bool isPlatformInitialized() const = 0;
    
    /**
     * Check if graphics context is initialized
     */
    virtual bool isGraphicsContextInitialized() const = 0;
};

/**
 * Factory function to create platform context
 */
PLATFORM_API std::shared_ptr<PlatformContext> createPlatformContext();

} // namespace Platform
} // namespace CarrotToy
