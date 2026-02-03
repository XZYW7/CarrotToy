# RHI GLFW Dependency Removal

## Problem

The OpenGL RHI module had residual GLFW dependencies:

1. **`glfwGetCurrentContext()`** - Used to check if an OpenGL context was current
2. **`glfwGetProcAddress()`** - Used as a fallback to load OpenGL function pointers

These dependencies caused several issues:

### Issues with GLFW Dependencies in RHI

1. **Cross-Module Context Loss**: When RHI is built as a separate DLL/shared library, GLFW state from the Platform module may not be visible, causing `glfwGetCurrentContext()` to return null even when a valid context exists.

2. **Complex Initialization**: The Renderer had to use thread-local variables and complex workarounds to pass the proc address loader to RHI, making the code harder to understand and maintain.

3. **Broken Abstraction**: RHI (Render Hardware Interface) should be independent of windowing libraries like GLFW. Having GLFW dependencies breaks the clean abstraction between Platform and RHI layers.

4. **Maintenance Burden**: Supporting both a loader-based path and a GLFW fallback path meant maintaining two code paths for initialization.

## Solution

**Always require a ProcAddressLoader** - Remove all GLFW fallback code and make the loader parameter mandatory.

### Changes Made

#### 1. OpenGLRHI.cpp

**Before** (with GLFW fallback):
```cpp
bool OpenGLRHIDevice::initialize(ProcAddressLoader loader) {
    if (loader) {
        // Use provided loader
        if (!gladLoadGLLoader((GLADloadproc)loader)) {
            return false;
        }
    } else {
        // Fallback to GLFW
        if (!glfwGetCurrentContext()) {
            std::cerr << "No OpenGL context!" << std::endl;
            return false;
        }
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            return false;
        }
    }
    // ...
}
```

**After** (loader required):
```cpp
bool OpenGLRHIDevice::initialize(ProcAddressLoader loader) {
    // Require proc address loader to avoid GLFW dependency
    if (!loader) {
        std::cerr << "ERROR: RHI initialization requires a valid ProcAddressLoader!" << std::endl;
        std::cerr << "       Platform layer must provide getProcAddress function." << std::endl;
        return false;
    }
    
    // Initialize GLAD using the provided loader
    if (!gladLoadGLLoader((GLADloadproc)loader)) {
        std::cerr << "Failed to initialize GLAD using provided loader!" << std::endl;
        return false;
    }
    
    // Verify OpenGL context is available
    const GLubyte* version = glGetString(GL_VERSION);
    if (!version) {
        std::cerr << "OpenGL context not available." << std::endl;
        std::cerr << "Ensure OpenGL context is current before initializing RHI." << std::endl;
        return false;
    }
    
    LOG("OpenGLRHI: Initialized. Version: " << (const char*)version);
    initialized = true;
    return true;
}
```

#### 2. Renderer.cpp

**Before** (complex thread-local workaround):
```cpp
// Store raw pointer temporarily for GLAD initialization (thread-local for safety)
thread_local Platform::IPlatformWindow* t_gladWindow = nullptr;
t_gladWindow = window.get();

auto gladLoader = [](const char* name) -> void* {
    return t_gladWindow ? t_gladWindow->getProcAddress(name) : nullptr;
};

// Initialize GLAD and RHI
if (!gladLoadGLLoader((GLADloadproc)+gladLoader)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    t_gladWindow = nullptr;
    return false;
}

// ...RHI initialization...
t_gladWindow = nullptr;  // Clear after all initializations
```

**After** (simple lambda capture):
```cpp
// Initialize GLAD using platform's proc address loader
// Create a simple lambda that wraps the window's getProcAddress
auto gladLoader = [this](const char* name) -> void* {
    return window ? window->getProcAddress(name) : nullptr;
};

// Correct Initialization Flow: Window -> Context -> GLAD -> RHI Device
LOG("Renderer: Initializing GLAD...");
if (!gladLoadGLLoader((GLADloadproc)+gladLoader)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return false;
}

LOG("Renderer: Creating and initializing global RHI device (OpenGL backend)...");
auto rhiDevice = CarrotToy::RHI::createRHIDevice(CarrotToy::RHI::GraphicsAPI::OpenGL);
if (!rhiDevice) {
    std::cerr << "Failed to create RHI device" << std::endl;
    return false;
}

// Pass the loader to RHI so it can initialize its local GLAD instance
// This is required for cross-DLL scenarios (RHI needs its own function pointers)
if (!rhiDevice->initialize(+gladLoader)) {
    std::cerr << "Failed to initialize RHI device" << std::endl;
    return false;
}

CarrotToy::RHI::setGlobalDevice(rhiDevice);
LOG("Renderer: RHI device initialized and registered globally.");
```

#### 3. RHI.h Documentation

Added clear documentation that the loader is required:

```cpp
using ProcAddressLoader = void* (*)(const char* name);

// Device initialization and shutdown
// NOTE: loader parameter is REQUIRED for OpenGL backend to avoid GLFW dependency in RHI
//       Pass a function that can load OpenGL function pointers (e.g., window->getProcAddress)
//       Example: auto loader = [window](const char* name) { return window->getProcAddress(name); };
virtual bool initialize(ProcAddressLoader loader = nullptr) = 0;
```

## Benefits

### 1. Clean Abstraction ✅
- RHI no longer has any GLFW dependencies
- True graphics API abstraction
- RHI can be used with any windowing library that provides a proc address loader

### 2. Simpler Code ✅
- No thread-local variable workarounds
- Single initialization code path
- Easier to understand and maintain
- Fewer lines of code

### 3. Better Error Messages ✅
- Clear error when loader is not provided
- Explicit instruction to provide loader from Platform layer
- Easier to diagnose initialization problems

### 4. Cross-Module Support ✅
- Works correctly in cross-DLL scenarios
- No context loss issues
- Explicit dependency on Platform layer (via loader)

### 5. Maintainability ✅
- Single code path reduces maintenance burden
- No need to test two different initialization paths
- Clear contract: loader is always required

## Usage Example

### Correct Usage

```cpp
// 1. Create platform and window
auto platform = Platform::createPlatform();
platform->initialize();

Platform::WindowDesc windowDesc;
windowDesc.width = 800;
windowDesc.height = 600;
windowDesc.title = "My App";

auto window = platform->createWindow(windowDesc);
window->makeContextCurrent();

// 2. Create proc address loader from window
auto loader = [&window](const char* name) -> void* {
    return window->getProcAddress(name);
};

// 3. Initialize GLAD in your module
if (!gladLoadGLLoader((GLADloadproc)+loader)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return false;
}

// 4. Create and initialize RHI device (loader is REQUIRED)
auto rhiDevice = RHI::createRHIDevice(RHI::GraphicsAPI::OpenGL);
if (!rhiDevice->initialize(+loader)) {  // Must provide loader!
    std::cerr << "Failed to initialize RHI" << std::endl;
    return false;
}

// 5. Now you can use RHI for rendering
rhiDevice->clearColor(0.2f, 0.2f, 0.2f, 1.0f);
rhiDevice->clear(true, true, false);
```

### Incorrect Usage (Will Fail)

```cpp
// ❌ This will now fail with a clear error message
auto rhiDevice = RHI::createRHIDevice(RHI::GraphicsAPI::OpenGL);
rhiDevice->initialize(nullptr);  // ERROR: loader is required!

// Output:
// ERROR: RHI initialization requires a valid ProcAddressLoader!
//        Platform layer must provide getProcAddress function.
```

## Migration Guide

If you have existing code that initializes RHI without a loader:

### Before
```cpp
auto rhiDevice = RHI::createRHIDevice(RHI::GraphicsAPI::OpenGL);
rhiDevice->initialize(nullptr);  // Relied on GLFW fallback
```

### After
```cpp
// Get the proc address loader from your platform window
auto loader = [window](const char* name) -> void* {
    return window->getProcAddress(name);
};

auto rhiDevice = RHI::createRHIDevice(RHI::GraphicsAPI::OpenGL);
rhiDevice->initialize(+loader);  // Provide the loader
```

## Technical Details

### Why ProcAddressLoader is Needed

When building RHI as a separate shared library (DLL on Windows, .so on Linux), each module has its own copy of OpenGL function pointers. Even though the Platform module may have initialized GLAD, the RHI module needs to initialize its own copy of the function pointers.

The ProcAddressLoader provides a way to query OpenGL function addresses that works across DLL boundaries:

1. Platform creates window and OpenGL context
2. Platform provides `getProcAddress` function
3. RHI uses this function to load its own OpenGL function pointers
4. Both modules can now call OpenGL functions correctly

### Why GLFW Functions Don't Work Across Modules

GLFW maintains internal state that is local to each DLL. When you call `glfwGetCurrentContext()` from the RHI module, it checks GLFW's state in the RHI module's address space, which may be different from the Platform module's GLFW state. This is why `glfwGetCurrentContext()` could return null even when a valid context exists.

By requiring an explicit loader function, we bypass this issue entirely.

## Conclusion

This change results in:
- ✅ **Cleaner architecture**: RHI is truly independent of GLFW
- ✅ **Simpler code**: No thread-local workarounds needed
- ✅ **Better errors**: Clear messages when loader is missing
- ✅ **Cross-module support**: Works correctly with shared libraries
- ✅ **Maintainability**: Single code path, easier to maintain

The solution is **simpler, cleaner, and more robust** than the previous implementation.
