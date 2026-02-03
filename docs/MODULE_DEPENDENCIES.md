# Module Dependency Diagram

## Visual Representation

```
┌─────────────────────────────────────────────────────────────────┐
│                         Applications Layer                       │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ DefaultGame │  │ TestRHIApp  │  │CustomModule │  ...        │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ depends on
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                      Application Layer                           │
│  ┌──────────────────┐            ┌──────────────────┐          │
│  │      Launch      │            │      Editor      │          │
│  │  (Main Loop)     │            │  (Material UI)   │          │
│  └──────────────────┘            └──────────────────┘          │
└─────────────────────────────────────────────────────────────────┘
            │                                 │
            │ depends on                      │ depends on
            ▼                                 ▼
┌─────────────────────────────────────────────────────────────────┐
│                      High-Level Systems                          │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                       Renderer                             │ │
│  │  (Material Preview, Scene Rendering, Integration)         │ │
│  └───────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
            │
            │ depends on (4 modules)
            │
    ┌───────┼───────┬───────────┬───────────┐
    │       │       │           │           │
    ▼       ▼       ▼           ▼           ▼
┌────────┐ ┌─────┐ ┌─────────┐ ┌─────────┐
│Platform│ │Input│ │  Core   │ │   RHI   │
└────────┘ └─────┘ └─────────┘ └─────────┘
    ▲         │         ▲           │
    │         │         │           │
    │         └─────────┘           │
    │        depends on             │
    │                                │
    └────────────────────────────────┘
              depends on

┌─────────────────────────────────────────────────────────────────┐
│                      Foundation Layers                           │
│                                                                   │
│  ┌──────────────────┐  ┌──────────────────┐                    │
│  │    Platform      │  │      Core        │                    │
│  │  (Windows/OS)    │  │   (Utilities)    │                    │
│  │                  │  │                  │                    │
│  │  - IPlatform     │  │  - CoreUtils     │                    │
│  │  - IPlatformWnd  │  │  - Materials     │                    │
│  │  - PlatformTypes │  │  - Shaders       │                    │
│  │  - GLFW Impl     │  │  - Scene Objects │                    │
│  └──────────────────┘  │  - Module System │                    │
│          ▲             └──────────────────┘                    │
│          │                      ▲                               │
│          │                      │                               │
│          │                      │ depends on                    │
│          └──────────────────────┘                               │
│                                                                   │
│  ┌──────────────────┐  ┌──────────────────┐                    │
│  │      Input       │  │       RHI        │                    │
│  │   (Devices)      │  │  (Graphics API)  │                    │
│  │                  │  │                  │                    │
│  │  - IInputDevice  │  │  - IRHIDevice    │                    │
│  │  - MouseButton   │  │  - IRHIBuffer    │                    │
│  │  - KeyCode       │  │  - IRHIShader    │                    │
│  │  - Platform Impl │  │  - OpenGL Impl   │                    │
│  └──────────────────┘  └──────────────────┘                    │
│          │                      │                               │
│          │ depends on           │ depends on                    │
│          ▼                      ▼                               │
│  ┌──────────────────┐  ┌──────────────────┐                    │
│  │    Platform      │  │      Core        │                    │
│  └──────────────────┘  └──────────────────┘                    │
└─────────────────────────────────────────────────────────────────┘

External Dependencies:
  - Platform → GLFW (windowing)
  - Core → STB, GLM, ImGui (utilities)
  - RHI → GLAD, GLM (graphics)
  - Renderer → GLAD (OpenGL, should be RHI-only in future)
```

## Dependency Matrix

| Module   | Depends On                        | External Deps          |
|----------|-----------------------------------|------------------------|
| Platform | (none)                            | GLFW                   |
| Core     | Platform                          | STB, GLM, ImGui        |
| Input    | Platform                          | (none)                 |
| RHI      | Core                              | GLAD, GLM              |
| Renderer | Core, Platform, Input, RHI        | GLAD                   |
| Launch   | Renderer, Editor                  | (none)                 |
| Editor   | Renderer                          | ImGui (via Core)       |
| Apps     | Launch, Editor                    | (none)                 |

## Layer Breakdown

### Layer 1: Foundation (No internal dependencies)
- **Platform**: OS abstraction, windowing
  - Only depends on GLFW (external)

### Layer 2: Core Systems (Depends on Platform)
- **Core**: Engine utilities, materials, shaders
  - Depends on: Platform
  
- **Input**: Input device abstraction
  - Depends on: Platform

### Layer 3: Graphics Abstraction (Depends on Core)
- **RHI**: Graphics API abstraction
  - Depends on: Core
  - Independent of Platform and Input

### Layer 4: High-Level Systems
- **Renderer**: Rendering system
  - Depends on: Core, Platform, Input, RHI
  - Integrates all lower layers

- **Editor**: Material editor UI
  - Depends on: Renderer (and transitively all below)

### Layer 5: Application Framework
- **Launch**: Main loop and initialization
  - Depends on: Renderer, Editor

### Layer 6: Applications
- **DefaultGame, TestRHIApp, CustomModule**
  - Depend on: Launch

## Key Design Decisions

### 1. Platform Separation
Platform is now separate from Core to enable:
- Independent platform development
- Easier platform porting
- Clean abstraction layer

### 2. Input Module
Input is separate to enable:
- Independent input system development
- Support for multiple input devices
- Platform-independent input API

### 3. RHI Purity
RHI depends only on Core (not Platform) to:
- Keep graphics API abstraction pure
- Enable headless rendering
- Support multiple graphics backends easily

### 4. Renderer Integration
Renderer depends on all system modules to:
- Integrate platform, input, and graphics
- Provide high-level rendering API
- Serve as integration point

## Module Communication

### Direct Dependencies (Compile-time)
Modules directly depend on and link against their dependencies.

### Runtime Communication
- Applications → Launch → Renderer → RHI
- User Input → Input Module → Renderer → Application Logic
- Platform Events → Platform Module → Application Loop

### No Circular Dependencies
The architecture strictly enforces a directed acyclic graph (DAG) of dependencies.

## Future Expansion

Planned additional modules:
- **Window**: Separate windowing from Platform
- **Audio**: Sound and music system
- **Physics**: Physics simulation
- **Resource**: Centralized asset management
- **Network**: Multiplayer support

These will fit into the existing layer structure:
- Window: Layer 2 (depends on Platform)
- Audio: Layer 3 (depends on Core, Platform)
- Physics: Layer 4 (depends on Core)
- Resource: Layer 2 (depends on Core, Platform)
- Network: Layer 2 (depends on Core, Platform)
