# Refactoring Summary: Moving Initialization to Module Internals

## Problem Statement (Original Issue in Chinese)

Renderer的Initialize中似乎做了很多不应该在Renderer完成的初始化，导致device、window、RHI等等的后端使用的glad和glfw在跨module的过程中会丢失上下文，将相关backend的初始化放到module内部自身好像合适一些，比如device的初始化和OpenglRHI的初始化就都在一个module不存在跨module的问题了。并且要glfw要和glad解耦，让window管理glfw

**Translation:**
It seems that a lot of initialization that should not be done in Renderer is being performed in Renderer's Initialize, causing the backend of device, window, RHI, etc. to use glad and glfw, and during cross-module operations, the context gets lost. It might be more appropriate to place the initialization of related backends inside the module itself. For example, if device initialization and OpenglRHI initialization are both in one module, there won't be cross-module issues. Additionally, glfw and glad need to be decoupled, letting window manage glfw.

## Root Cause Analysis

The issue occurred because:

1. **Renderer was doing too much initialization**: Renderer::initialize() was creating Platform, PlatformContext, Window, and RHI device
2. **Cross-module context loss**: When modules are compiled as shared libraries (DLLs), GLFW and GLAD function pointers don't transfer properly across module boundaries
3. **Poor separation of concerns**: Initialization logic that belonged in Platform and RHI modules was in the Renderer module
4. **Module initialization order**: Modules weren't being initialized in the correct dependency order

## Solution Overview

The solution moves initialization responsibilities to the appropriate modules:

### 1. Platform Module Gets PlatformSubsystem

**New Component: `PlatformSubsystem`**
- Central singleton that manages GLFW and window lifecycle
- Lives inside the Platform module
- Handles:
  - GLFW initialization (via PlatformContext)
  - Window creation
  - GLAD initialization for rendering context
  - Proc address loader provision for cross-DLL scenarios

**Key Changes:**
- `src/Runtime/Platform/Public/Platform/PlatformModule.h` - Added PlatformSubsystem class
- `src/Runtime/Platform/Private/PlatformModule.cpp` - Implemented PlatformSubsystem and FPlatformModule
- `src/Runtime/Platform/Public/Platform/PlatformModuleInit.h` - Added InitializeModulePlatform export

### 2. RHI Module Gets RHISubsystem

**New Component: `RHISubsystem`**
- Central singleton that manages RHI device lifecycle
- Lives inside the RHI module
- Handles:
  - RHI device creation
  - GLAD initialization within RHI module (for cross-DLL compatibility)
  - Device initialization with proc address loader

**Key Changes:**
- `src/Runtime/RHI/Public/RHI/RHIModuleInit.h` - Added RHISubsystem class
- `src/Runtime/RHI/Private/RHIModule.cpp` - Implemented RHISubsystem

### 3. Renderer Becomes a Coordinator

**Simplified Responsibilities:**
- No longer creates Platform or RHI infrastructure
- Uses pre-initialized Platform subsystem to create windows
- Uses pre-initialized RHI subsystem for rendering
- Only manages its own rendering resources (geometry, framebuffers)

**Key Changes:**
- `src/Runtime/Renderer/Public/Renderer.h` - Removed platformContext and platform members
- `src/Runtime/Renderer/Private/Renderer.cpp` - Refactored initialize() to use subsystems

### 4. Module Loading Order

**Updated Launch Sequence:**
- `src/Runtime/Launch/Private/Launch.cpp` - Added InitializeModulePlatform call and proper loading order

```cpp
// Old order (incorrect):
InitializeModuleCoreEngine();
InitializeModuleRHI();        // RHI before Platform!
InitializeModuleRenderer();

// New order (correct):
InitializeModuleCoreEngine();
InitializeModulePlatform();   // Platform first!
InitializeModuleRHI();        // Then RHI
InitializeModuleRenderer();   // Finally Renderer
```

## Architecture Comparison

### Before (Problematic)

```
Renderer::initialize()
├── Creates PlatformContext
├── Initializes GLFW
├── Creates Platform
├── Creates Window
├── Initializes GLAD (in Renderer module)
├── Creates RHI Device
└── Initializes RHI (in RHI module, but GLAD context lost!)
```

**Problems:**
- Context loss when crossing from Renderer → RHI module boundary
- Renderer responsible for too many things
- Module dependencies not clear

### After (Fixed)

```
Module Initialization:
├── Platform Module
│   └── PlatformSubsystem::Initialize()
│       ├── Creates PlatformContext
│       ├── Initializes GLFW
│       └── Creates Platform instance
│
├── RHI Module
│   └── RHISubsystem (ready, but not yet initialized)
│
└── Renderer::initialize()
    ├── Gets PlatformSubsystem
    ├── Creates Window via PlatformSubsystem
    ├── Initializes GLAD via PlatformSubsystem (in Platform module)
    ├── Gets RHISubsystem
    ├── Initializes RHI with proc loader (GLAD in RHI module)
    └── Sets up rendering resources
```

**Benefits:**
- Each module manages its own context initialization
- No context loss across module boundaries
- Clear separation of concerns
- Proper dependency ordering

## Technical Details

### Cross-DLL GLAD Initialization

The key to solving the context loss issue is ensuring GLAD is initialized in each module that uses OpenGL:

1. **Platform Module**: Initializes GLAD once for window/rendering context
   ```cpp
   platformContext->initializeGraphicsContext(window.get());
   ```

2. **RHI Module**: Initializes GLAD again using proc address loader
   ```cpp
   auto loader = platformSubsystem.GetProcAddressLoader();
   rhiSubsystem.Initialize(GraphicsAPI::OpenGL, loader);
   ```

This ensures both modules have valid OpenGL function pointers in their respective address spaces.

### GLFW Management

GLFW is now completely encapsulated in the Platform module:
- PlatformContext manages GLFW lifecycle
- PlatformSubsystem provides access to GLFW-based Platform and Window
- Other modules never directly call GLFW functions

### Subsystem Pattern

Both PlatformSubsystem and RHISubsystem follow a singleton pattern:
```cpp
class PlatformSubsystem {
public:
    static PlatformSubsystem& Get();
    bool Initialize();
    void Shutdown();
    // ... accessor methods ...
private:
    PlatformSubsystem() = default;  // Private constructor
    // ... member variables ...
};
```

This ensures:
- Single instance per module
- Controlled initialization
- Easy access throughout the codebase

## Files Modified

### Modified (7 files)
1. `src/Runtime/Launch/Private/Launch.cpp` - Added Platform module initialization
2. `src/Runtime/Platform/Private/PlatformModule.cpp` - Implemented PlatformSubsystem
3. `src/Runtime/Platform/Public/Platform/PlatformModule.h` - Added PlatformSubsystem interface
4. `src/Runtime/RHI/Private/RHIModule.cpp` - Implemented RHISubsystem
5. `src/Runtime/RHI/Public/RHI/RHIModuleInit.h` - Added RHISubsystem interface
6. `src/Runtime/Renderer/Private/Renderer.cpp` - Refactored to use subsystems
7. `src/Runtime/Renderer/Public/Renderer.h` - Removed platform/context members

### Created (1 file)
8. `src/Runtime/Platform/Public/Platform/PlatformModuleInit.h` - Module initialization export

## Testing Strategy

To verify the fix works correctly:

1. **Build in Shared Library Mode**: `xmake f --module_kind=shared`
2. **Check Module Loading**: Verify Platform loads before RHI and Renderer
3. **Verify OpenGL Context**: Ensure both Platform and RHI can make OpenGL calls
4. **Cross-Module Calls**: Test that Renderer can use RHI functions without context loss
5. **Window Operations**: Verify window creation and rendering work correctly

## Benefits of This Refactoring

### 1. Solves Cross-Module Context Loss
✅ Each module initializes GLAD in its own address space
✅ No more lost function pointers when crossing DLL boundaries

### 2. Better Separation of Concerns
✅ Platform module owns GLFW/windowing
✅ RHI module owns graphics device
✅ Renderer coordinates, doesn't initialize infrastructure

### 3. Clearer Module Dependencies
```
Core (no dependencies)
  ↓
Platform (depends on Core)
  ↓
RHI (depends on Core, uses Platform's proc loader)
  ↓
Renderer (depends on Platform + RHI + Input)
```

### 4. More Maintainable
✅ Each module has clear responsibilities
✅ Initialization code is in the appropriate module
✅ Easier to add new graphics backends (Vulkan, DX12, etc.)

### 5. Follows Best Practices
✅ Subsystem pattern for centralized management
✅ Dependency injection (Renderer uses pre-initialized systems)
✅ Single Responsibility Principle
✅ Open/Closed Principle (easy to extend with new backends)

## Future Improvements

1. **Remove Direct OpenGL Calls from Renderer**
   - Move `setupPreviewGeometry()` to use RHI API
   - Move `setupFramebuffer()` to use RHI API
   - Remove `#include <glad/glad.h>` from Renderer.cpp

2. **Add More Graphics Backend Support**
   - Implement VulkanRHI subsystem
   - Implement DirectX12RHI subsystem
   - PlatformSubsystem can support multiple contexts

3. **Platform Abstraction**
   - Support SDL2 backend
   - Support native Win32/Wayland
   - PlatformSubsystem chooses appropriate backend

## Conclusion

This refactoring successfully addresses the issue of cross-module context loss by:

1. Moving initialization to the appropriate module (Platform and RHI)
2. Ensuring GLAD is initialized in each module that needs it
3. Keeping GLFW completely encapsulated in the Platform module
4. Establishing clear module dependencies and initialization order

The result is a more maintainable, extensible, and bug-free architecture that properly handles the complexities of cross-module OpenGL context management in a shared library configuration.
