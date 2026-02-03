# Module Architecture Documentation

## Overview

CarrotToy has been reorganized into a clean, modular architecture with well-defined module boundaries and dependencies. This document describes the new structure and the responsibilities of each module.

## Recent Improvements

**RHI GLFW Dependency Removal** (Latest): The RHI module no longer has any GLFW dependencies. ProcAddressLoader is now required for initialization, resulting in cleaner code and better abstraction. See [RHI_GLFW_REMOVAL.md](RHI_GLFW_REMOVAL.md) for details.

## Module Structure

```
CarrotToy/
├── src/Runtime/
│   ├── Platform/        # Platform abstraction layer (NEW)
│   ├── Input/          # Input handling module (NEW)
│   ├── Core/           # Core engine utilities
│   ├── RHI/            # Render Hardware Interface
│   ├── Renderer/       # High-level renderer
│   └── Launch/         # Application launcher
├── src/Editor/         # Material editor UI
├── src/DefaultGame/    # Main application
├── src/TestRHIApp/     # RHI testing app
└── src/CustomModule/   # Custom module example
```

## Module Dependency Graph

```
┌──────────────────────────────────────────────────────────┐
│                      Applications                          │
│  DefaultGame │ TestRHIApp │ CustomModule │ Editor         │
└──────────────────────────────────────────────────────────┘
                            │
                            v
                    ┌──────────────┐
                    │    Launch    │
                    └──────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
        v                   v                   v
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│   Renderer   │    │    Editor    │    │     ...      │
└──────────────┘    └──────────────┘    └──────────────┘
        │
        ├──────────────┬──────────────┬──────────────┐
        v              v              v              v
┌──────────────┐ ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
│   Platform   │ │     Input    │ │     Core     │ │     RHI      │
└──────────────┘ └──────────────┘ └──────────────┘ └──────────────┘
                        │              │              │
                        v              │              │
                 ┌──────────────┐     │              │
                 │   Platform   │     │              │
                 └──────────────┘     │              │
                                      v              v
                               ┌──────────────┐ ┌──────────────┐
                               │   Platform   │ │     Core     │
                               └──────────────┘ └──────────────┘
```

## Module Details

### Platform Module
**Location**: `src/Runtime/Platform/`
**Purpose**: Abstract platform-specific functionality (windowing, events)
**Dependencies**: GLFW (external)
**Exports**:
- `IPlatform`: Platform abstraction interface
- `IPlatformWindow`: Window abstraction interface
- `PlatformTypes`: Platform-related types

**Key Features**:
- Window creation and management
- Event polling
- Graphics context operations
- Platform-independent window interface

**API Example**:
```cpp
auto platform = Platform::createPlatform();
platform->initialize();
auto window = platform->createWindow(windowDesc);
```

### Input Module
**Location**: `src/Runtime/Input/`
**Purpose**: Handle input from keyboard, mouse, and other devices
**Dependencies**: Platform module
**Exports**:
- `IInputDevice`: Input device interface
- `InputTypes`: Input-related types (MouseButton, KeyCode, etc.)

**Key Features**:
- Abstract input polling from windows
- Mouse and keyboard state tracking
- Platform-independent input interface

**API Example**:
```cpp
auto inputDevice = Input::createInputDevice(window);
auto cursorPos = inputDevice->getCursorPosition();
bool leftPressed = inputDevice->isMouseButtonPressed(Input::MouseButton::Left);
```

### Core Module
**Location**: `src/Runtime/Core/`
**Purpose**: Core engine utilities and data structures
**Dependencies**: Platform module
**Exports**:
- Core utilities (logging, paths, etc.)
- Scene objects
- Material and shader abstractions
- Module system

**Key Features**:
- Engine-wide utilities
- Material and shader management
- Scene graph structures
- Module loading system

### RHI Module
**Location**: `src/Runtime/RHI/`
**Purpose**: Graphics API abstraction layer
**Dependencies**: Core module (for basic types only - NO GLFW)
**Exports**:
- `IRHIDevice`: Graphics device interface
- `IRHIBuffer`, `IRHIShader`, etc.: Resource interfaces
- OpenGL implementation

**Key Features**:
- Graphics API abstraction (OpenGL, future: Vulkan, DirectX, Metal)
- Resource management (buffers, shaders, textures, etc.)
- Rendering state management
- Platform-independent graphics interface
- **Zero GLFW dependency** - requires ProcAddressLoader for initialization

**API Example**:
```cpp
// Loader is REQUIRED for OpenGL backend
auto loader = [window](const char* name) { 
    return window->getProcAddress(name); 
};

auto device = RHI::createRHIDevice(RHI::GraphicsAPI::OpenGL);
device->initialize(+loader);  // Loader is mandatory
auto buffer = device->createBuffer(bufferDesc);
auto shader = device->createShader(shaderDesc);
```

### Renderer Module
**Location**: `src/Runtime/Renderer/`
**Purpose**: High-level rendering system
**Dependencies**: Core, Platform, Input, RHI
**Exports**:
- `Renderer`: Main renderer class
- Material system
- Shader system

**Key Features**:
- Material preview rendering
- Scene rendering
- Integration with RHI for graphics operations
- Material and shader management

**API Example**:
```cpp
Renderer renderer;
renderer.initialize(800, 600, "CarrotToy");
renderer.beginFrame();
renderer.renderMaterialPreview(material);
renderer.endFrame();
```

### Launch Module
**Location**: `src/Runtime/Launch/`
**Purpose**: Application lifecycle and main loop
**Dependencies**: All runtime modules
**Exports**:
- `FMainLoop`: Main application loop
- Launch API

**Key Features**:
- Application initialization
- Main loop with fixed timestep
- Module loading and management
- Integration point for all subsystems

## Design Principles

### 1. Separation of Concerns
- Each module has a single, well-defined responsibility
- Platform abstraction is separate from core utilities
- Input handling is separate from platform windowing
- Graphics API abstraction (RHI) is separate from high-level rendering

### 2. Minimal Dependencies
- Modules depend only on what they need
- Lower-level modules have fewer dependencies
- Platform and Input are foundational with minimal deps
- RHI depends only on Core, not on windowing or input

### 3. Clear Abstraction Layers
- **Layer 1 (Foundation)**: Platform - OS/window abstraction
- **Layer 2 (Core)**: Core, Input - Engine utilities and input
- **Layer 3 (Graphics)**: RHI - Graphics API abstraction
- **Layer 4 (High-Level)**: Renderer - High-level rendering
- **Layer 5 (Application)**: Launch, Editor - Application logic

### 4. API Stability
- Public interfaces are in `Public/` folders
- Implementation details are in `Private/` folders
- Export macros control symbol visibility
- Clean module boundaries enable independent development

## Migration Notes

### For Code Using Old Structure

**Old Platform Access**:
```cpp
#include "CoreUtils.h"  // Had Platform included
// Platform was in Core
```

**New Platform Access**:
```cpp
#include "Platform/Platform.h"  // Direct include
// Platform is separate module
```

**Old Input Access**:
```cpp
renderer->getCursorPos(x, y);
renderer->getMouseButton(button);
```

**New Input Access**:
```cpp
auto inputDevice = renderer->getInputDevice();
auto pos = inputDevice->getCursorPosition();
bool pressed = inputDevice->isMouseButtonPressed(Input::MouseButton::Left);
```

### Backward Compatibility

The Renderer still provides input proxy methods (`getCursorPos`, `getMouseButton`) for backward compatibility, but these are marked as deprecated. New code should use the Input module directly.

## Benefits of New Architecture

1. **Cleaner Dependencies**: Each module depends only on what it needs
2. **Better Testing**: Modules can be tested independently
3. **Easier Maintenance**: Clear boundaries make changes safer
4. **Future Graphics APIs**: RHI abstraction makes adding Vulkan/DX12 easier
5. **Platform Portability**: Platform abstraction enables cross-platform development
6. **Input Flexibility**: Input module can be extended for gamepad, touch, etc.

## Future Enhancements

1. **Graphics Module**: Add Window module separate from Platform
2. **Multiple Graphics APIs**: Implement Vulkan, DirectX 12, Metal backends
3. **Advanced Input**: Add gamepad, touch, gesture support
4. **Resource Module**: Centralized asset loading and management
5. **Audio Module**: Sound and music system
6. **Physics Module**: Physics simulation
7. **Networking Module**: Multiplayer support

## Building

Each module is built as a shared library (by default) or static library:

```bash
# Build all modules
xmake

# Build specific module
xmake build Platform
xmake build Input
xmake build RHI
xmake build Renderer

# Change to static libraries
xmake f --module_kind=static
xmake
```

## Module Authors

- Platform Module: Extracted from Core, enhanced with better abstractions
- Input Module: New module for input handling
- RHI Module: Cleaned up and made truly independent
- Renderer Module: Updated to use new module structure
