#pragma once

#include <cstdint>

namespace CarrotToy {
namespace Platform {

// Platform types
enum class PlatformType {
    Windows,
    Linux,
    MacOS,
    Unknown
};

// Window creation parameters
struct WindowDesc {
    uint32_t width;
    uint32_t height;
    const char* title;
    bool fullscreen;
    bool resizable;
    bool vsync;
    
    WindowDesc()
        : width(800)
        , height(600)
        , title("CarrotToy")
        , fullscreen(false)
        , resizable(true)
        , vsync(true) {}
};

// Window handle (opaque pointer to platform-specific window)
using WindowHandle = void*;

// Display/Monitor information
struct DisplayInfo {
    uint32_t width;
    uint32_t height;
    uint32_t refreshRate;
    const char* name;
};

} // namespace Platform
} // namespace CarrotToy
