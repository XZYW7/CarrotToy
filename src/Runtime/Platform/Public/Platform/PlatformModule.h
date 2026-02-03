#pragma once

#include "Platform.h"
#include "PlatformContext.h"
#include <memory>

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

/**
 * Platform Subsystem - Manages GLFW initialization and window creation
 * This is the central point for platform/window management within the Platform module
 */
class PLATFORM_API PlatformSubsystem {
public:
    static PlatformSubsystem& Get();
    
    /**
     * Initialize the platform subsystem (GLFW)
     * Must be called before creating windows
     */
    bool Initialize();
    
    /**
     * Create a window with the given descriptor
     * Platform must be initialized first
     */
    std::shared_ptr<IPlatformWindow> CreatePlatformWindow(const WindowDesc& desc);
    
    /**
     * Initialize graphics context (GLAD) for a window
     * Window must be created first and have an active OpenGL context
     */
    bool InitializeGraphicsContext(IPlatformWindow* window);
    
    /**
     * Get the proc address loader for cross-DLL GLAD initialization
     */
    PlatformContext::ProcAddressLoader GetProcAddressLoader() const;
    
    /**
     * Get the platform instance
     */
    std::shared_ptr<IPlatform> GetPlatform() const { return platform; }
    
    /**
     * Get the platform context
     */
    std::shared_ptr<PlatformContext> GetPlatformContext() const { return platformContext; }
    
    /**
     * Poll events - should be called once per frame
     */
    void PollEvents();
    
    /**
     * Check if platform is initialized
     */
    bool IsInitialized() const { return initialized; }
    
    /**
     * Shutdown the platform subsystem
     */
    void Shutdown();
    
private:
    PlatformSubsystem() = default;
    ~PlatformSubsystem() = default;
    PlatformSubsystem(const PlatformSubsystem&) = delete;
    PlatformSubsystem& operator=(const PlatformSubsystem&) = delete;
    
    bool initialized = false;
    std::shared_ptr<PlatformContext> platformContext;
    std::shared_ptr<IPlatform> platform;
};

} // namespace Platform
} // namespace CarrotToy
