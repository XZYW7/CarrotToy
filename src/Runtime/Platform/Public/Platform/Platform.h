#pragma once

#include "PlatformTypes.h"
#include <memory>
#include <functional>
#include <vector>

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

// Forward declarations
class IPlatformWindow;

// Platform interface - abstracts platform-specific functionality
class IPlatform {
public:
    virtual ~IPlatform() = default;
    
    // Platform initialization and shutdown
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Get platform type
    virtual PlatformType getPlatformType() const = 0;
    
    // Window creation
    virtual std::shared_ptr<IPlatformWindow> createWindow(const WindowDesc& desc) = 0;
    
    // Platform message pump
    virtual void pollEvents() = 0;
    
    // Display/Monitor queries
    virtual std::vector<DisplayInfo> getDisplays() const = 0;
    virtual DisplayInfo getPrimaryDisplay() const = 0;
    
    // Time queries
    virtual double getTime() const = 0;
};

// Window interface - abstracts platform window
class PLATFORM_API IPlatformWindow {
public:
    virtual ~IPlatformWindow() = default;
    
    // Window state
    virtual bool shouldClose() const = 0;
    virtual void setShouldClose(bool value) = 0;
    
    // Window properties
    virtual void setTitle(const char* title) = 0;
    virtual void getSize(uint32_t& width, uint32_t& height) const = 0;
    virtual void setSize(uint32_t width, uint32_t height) = 0;
    virtual void getFramebufferSize(uint32_t& width, uint32_t& height) const = 0;
    
    // Window operations
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void focus() = 0;
    
    // Graphics context operations
    virtual void makeContextCurrent() = 0;
    virtual void swapBuffers() = 0;
    virtual void* getProcAddress(const char* name) = 0;
    
    // Native handle access (for RHI integration)
    virtual WindowHandle getNativeHandle() const = 0;

    // Input polling (Manual query needed for cross-module boundaries)
    virtual void getCursorPos(double& x, double& y) const = 0;
    virtual bool getMouseButton(int button) const = 0;
    
    // Callbacks
    using ResizeCallback = std::function<void(uint32_t width, uint32_t height)>;
    virtual void setResizeCallback(ResizeCallback callback) = 0;
};

// Factory function to create platform instance
PLATFORM_API std::shared_ptr<IPlatform> createPlatform();

// Get current platform type (compile-time detection)
PLATFORM_API PlatformType getCurrentPlatformType();

} // namespace Platform
} // namespace CarrotToy
