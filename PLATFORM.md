# Platform Abstraction Layer Documentation

## Overview

The Platform abstraction layer provides a clean interface for window management, input handling, and graphics context creation across different operating systems. This layer decouples the application code from platform-specific APIs, making it easier to port to new platforms and maintain the codebase.

## Architecture

The Platform layer sits between the application/renderer and the underlying platform APIs (GLFW, Win32, X11, Cocoa, etc.). It provides two main interfaces:

- **IPlatform**: Platform-wide operations (initialization, window creation, event polling)
- **IPlatformWindow**: Individual window operations (resize, context management, input)

## Supported Platforms

Currently implemented:
- **Windows** (via GLFW)
- **Linux** (via GLFW)
- **macOS** (via GLFW)

The GLFW backend provides consistent behavior across all platforms while allowing for platform-specific optimizations in the future.

## Core Interfaces

### IPlatform

Main platform interface for system-wide operations.

```cpp
class IPlatform {
public:
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual PlatformType getPlatformType() const = 0;
    virtual std::shared_ptr<IPlatformWindow> createWindow(const WindowDesc& desc) = 0;
    virtual void pollEvents() = 0;
    virtual std::vector<DisplayInfo> getDisplays() const = 0;
    virtual DisplayInfo getPrimaryDisplay() const = 0;
    virtual double getTime() const = 0;
};
```

**Key Methods**:
- `initialize()`: Initializes the platform subsystem
- `shutdown()`: Cleans up platform resources
- `createWindow()`: Creates a new window with specified parameters
- `pollEvents()`: Processes pending input and system events
- `getDisplays()`: Queries available monitors/displays
- `getTime()`: Returns high-precision time for animations

### IPlatformWindow

Window-specific operations and state management.

```cpp
class IPlatformWindow {
public:
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
    
    // Native handle access
    virtual WindowHandle getNativeHandle() const = 0;
    
    // Callbacks
    using ResizeCallback = std::function<void(uint32_t width, uint32_t height)>;
    virtual void setResizeCallback(ResizeCallback callback) = 0;
};
```

**Key Methods**:
- `shouldClose()`: Checks if the window close button was clicked
- `getSize()`: Gets window size in screen coordinates
- `getFramebufferSize()`: Gets framebuffer size in pixels (may differ on high-DPI displays)
- `makeContextCurrent()`: Makes the OpenGL context current for this thread
- `swapBuffers()`: Swaps front and back buffers
- `getProcAddress()`: Gets OpenGL function pointers for GLAD initialization
- `getNativeHandle()`: Returns platform-specific window handle (e.g., GLFWwindow*)

## Platform Types

```cpp
enum class PlatformType {
    Windows,
    Linux,
    MacOS,
    Unknown
};
```

Detected at compile-time using preprocessor macros:
- `_WIN32` or `_WIN64` → Windows
- `__linux__` → Linux
- `__APPLE__` && `__MACH__` → macOS

## Window Creation

### WindowDesc

Window creation parameters:

```cpp
struct WindowDesc {
    uint32_t width;         // Window width in screen coordinates
    uint32_t height;        // Window height in screen coordinates
    const char* title;      // Window title
    bool fullscreen;        // Fullscreen mode
    bool resizable;         // Can window be resized
    bool vsync;            // Enable vertical sync
};
```

### Example Usage

```cpp
// Create platform
auto platform = Platform::createPlatform();
if (!platform->initialize()) {
    return false;
}

// Create window
Platform::WindowDesc desc;
desc.width = 1280;
desc.height = 720;
desc.title = "My Application";
desc.resizable = true;
desc.vsync = true;

auto window = platform->createWindow(desc);
if (!window) {
    return false;
}

// Set up resize callback
window->setResizeCallback([](uint32_t width, uint32_t height) {
    glViewport(0, 0, width, height);
});

// Make context current for OpenGL
window->makeContextCurrent();

// Initialize GLAD
if (!gladLoadGLLoader((GLADloadproc)window->getProcAddress("glGetString"))) {
    return false;
}

// Main loop
while (!window->shouldClose()) {
    // Render...
    
    window->swapBuffers();
    platform->pollEvents();
}

// Cleanup
platform->shutdown();
```

## Display Information

Query display/monitor information:

```cpp
struct DisplayInfo {
    uint32_t width;          // Display width in pixels
    uint32_t height;         // Display height in pixels
    uint32_t refreshRate;    // Refresh rate in Hz
    const char* name;        // Display name
};
```

Example:
```cpp
// Get all displays
auto displays = platform->getDisplays();
for (const auto& display : displays) {
    std::cout << display.name << ": " 
              << display.width << "x" << display.height 
              << " @ " << display.refreshRate << "Hz\n";
}

// Get primary display
auto primary = platform->getPrimaryDisplay();
```

## Integration with RHI

The Platform layer works seamlessly with the RHI (Render Hardware Interface):

1. Platform creates the window and graphics context
2. Window provides `getProcAddress()` for loading OpenGL functions
3. RHI initializes using the loaded functions
4. RHI doesn't need to know about platform-specific details

```cpp
// Create platform and window
auto platform = Platform::createPlatform();
platform->initialize();
auto window = platform->createWindow(desc);
window->makeContextCurrent();

// Initialize GLAD for OpenGL
gladLoadGLLoader((GLADloadproc)window->getProcAddress("glGetString"));

// Now create and initialize RHI
auto rhiDevice = RHI::createRHIDevice(RHI::GraphicsAPI::OpenGL);
rhiDevice->initialize();  // RHI can now use OpenGL functions
```

## Platform-Specific Notes

### Windows
- Uses OpenGL 4.6 Core Profile by default
- Supports high-DPI displays
- Window resizing handled automatically

### Linux
- Uses OpenGL 4.6 Core Profile
- X11 backend through GLFW
- Wayland support through GLFW's Wayland backend

### macOS
- Uses OpenGL 4.1 Core Profile (macOS limitation)
- Requires `GLFW_OPENGL_FORWARD_COMPAT` hint
- High-DPI (Retina) displays supported

## Future Enhancements

### Native Platform Implementations

While GLFW provides excellent cross-platform support, native implementations could be added for:

- **Windows**: Win32 API for tighter Windows integration
- **Linux**: X11 or Wayland direct integration
- **macOS**: Cocoa API for native macOS features

### Additional Features

Planned additions to the Platform layer:

- **Input System**: Keyboard, mouse, gamepad input abstraction
- **File Dialogs**: Native file open/save dialogs
- **Clipboard**: Cross-platform clipboard access
- **Drag & Drop**: File drag and drop support
- **Multi-Window**: Support for multiple windows
- **Cursor Management**: Custom cursors, cursor hiding
- **Message Boxes**: Native message box dialogs

## Design Patterns

### Abstract Factory Pattern
`IPlatform` acts as an abstract factory for creating windows.

### Strategy Pattern
Different platform implementations can be swapped at runtime or compile-time.

### Bridge Pattern
Separates platform abstraction from platform implementation.

## Best Practices

1. **Always check return values**: Both `initialize()` and `createWindow()` can fail
2. **Make context current**: Before using OpenGL functions, call `makeContextCurrent()`
3. **Poll events regularly**: Call `pollEvents()` in your main loop to keep the window responsive
4. **Use callbacks**: Set resize callbacks instead of polling window size
5. **Clean up properly**: Call `shutdown()` before application exit

## Thread Safety

Current implementation is **single-threaded**:
- All platform operations must be called from the main thread
- OpenGL context is bound to a single thread
- Window operations are not thread-safe

Future versions may add thread-safe queues for cross-thread communication.

## Error Handling

Platform operations use return values and logs for error reporting:

```cpp
if (!platform->initialize()) {
    std::cerr << "Platform initialization failed" << std::endl;
    return false;
}

auto window = platform->createWindow(desc);
if (!window) {
    std::cerr << "Window creation failed" << std::endl;
    return false;
}
```

No exceptions are thrown by the Platform layer.

## References

- **GLFW Documentation**: [https://www.glfw.org/documentation.html](https://www.glfw.org/documentation.html)
- **OpenGL Context Creation**: [https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context](https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context)
- **High-DPI Support**: [https://www.glfw.org/docs/latest/window_guide.html#window_size](https://www.glfw.org/docs/latest/window_guide.html#window_size)

## License

The Platform abstraction layer is part of CarrotToy and is licensed under the MIT License.
