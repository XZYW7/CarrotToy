# Future Module Recommendations

Based on the new modular architecture, this document outlines additional modules that would benefit the CarrotToy engine.

## Immediate Recommendations

### 1. Window Module (High Priority)
**Purpose**: Separate windowing concerns from platform abstraction

**Current State**: Window management is currently mixed in Platform module

**Benefits**:
- Cleaner separation between OS platform and windowing
- Enable headless rendering mode
- Support multiple window management libraries (GLFW, SDL, native OS APIs)
- Better testing (mock windows)

**Structure**:
```
src/Runtime/Window/
├── Public/Window/
│   ├── IWindow.h           # Window interface
│   ├── WindowTypes.h       # Window types
│   └── WindowModule.h      # Module interface
└── Private/
    ├── GLFWWindow.cpp      # GLFW implementation
    └── WindowModule.cpp    # Module implementation
```

**Dependencies**: Platform (for OS integration)

**API Example**:
```cpp
auto windowManager = Window::createWindowManager(platform);
auto window = windowManager->createWindow(desc);
```

### 2. Asset/Resource Module (High Priority)
**Purpose**: Centralized asset loading, caching, and management

**Current State**: Asset loading is scattered across modules

**Benefits**:
- Unified asset loading pipeline
- Asset caching and reference counting
- Hot-reloading support
- Async loading
- Virtual file system support

**Structure**:
```
src/Runtime/Resource/
├── Public/Resource/
│   ├── IResourceManager.h  # Resource manager interface
│   ├── IResourceLoader.h   # Loader interface
│   ├── ResourceTypes.h     # Resource types
│   └── ResourceModule.h    # Module interface
└── Private/
    ├── ResourceManager.cpp # Manager implementation
    ├── TextureLoader.cpp   # Texture loader
    ├── ShaderLoader.cpp    # Shader loader
    └── ResourceModule.cpp  # Module implementation
```

**Dependencies**: Core, Platform (for file I/O)

**API Example**:
```cpp
auto texture = resourceManager->load<Texture>("textures/wood.png");
auto shader = resourceManager->load<Shader>("shaders/pbr.hlsl");
```

## Near-Term Recommendations

### 3. Audio Module (Medium Priority)
**Purpose**: Sound and music playback system

**Benefits**:
- 3D positional audio
- Music streaming
- Sound effect playback
- Audio mixing

**Structure**:
```
src/Runtime/Audio/
├── Public/Audio/
│   ├── IAudioDevice.h      # Audio device interface
│   ├── IAudioSource.h      # Audio source interface
│   ├── AudioTypes.h        # Audio types
│   └── AudioModule.h       # Module interface
└── Private/
    ├── OpenALAudio.cpp     # OpenAL implementation
    └── AudioModule.cpp     # Module implementation
```

**Dependencies**: Core, Resource (for audio file loading)

**Suggested Libraries**: OpenAL, FMOD, or Wwise

### 4. Scene Module (Medium Priority)
**Purpose**: Scene graph management and entity system

**Current State**: Basic scene objects exist in Core

**Benefits**:
- Hierarchical scene graph
- Entity-Component-System (ECS)
- Spatial queries
- Scene serialization

**Structure**:
```
src/Runtime/Scene/
├── Public/Scene/
│   ├── IScene.h            # Scene interface
│   ├── IEntity.h           # Entity interface
│   ├── IComponent.h        # Component interface
│   ├── SceneTypes.h        # Scene types
│   └── SceneModule.h       # Module interface
└── Private/
    ├── Scene.cpp           # Scene implementation
    ├── Entity.cpp          # Entity implementation
    └── SceneModule.cpp     # Module implementation
```

**Dependencies**: Core, Platform (for spatial structures)

### 5. Physics Module (Medium Priority)
**Purpose**: Physics simulation and collision detection

**Benefits**:
- Rigid body dynamics
- Collision detection
- Physics constraints
- Ragdoll physics

**Structure**:
```
src/Runtime/Physics/
├── Public/Physics/
│   ├── IPhysicsWorld.h     # Physics world interface
│   ├── IRigidBody.h        # Rigid body interface
│   ├── ICollider.h         # Collider interface
│   ├── PhysicsTypes.h      # Physics types
│   └── PhysicsModule.h     # Module interface
└── Private/
    ├── BulletPhysics.cpp   # Bullet implementation
    └── PhysicsModule.cpp   # Module implementation
```

**Dependencies**: Core, Scene

**Suggested Libraries**: Bullet Physics, PhysX, or Jolt Physics

## Long-Term Recommendations

### 6. Network Module (Lower Priority)
**Purpose**: Networking and multiplayer support

**Benefits**:
- Client-server architecture
- State synchronization
- Network prediction
- Latency compensation

**Structure**:
```
src/Runtime/Network/
├── Public/Network/
│   ├── INetworkManager.h   # Network manager
│   ├── INetworkPeer.h      # Network peer
│   ├── NetworkTypes.h      # Network types
│   └── NetworkModule.h     # Module interface
└── Private/
    ├── NetworkManager.cpp  # Manager implementation
    └── NetworkModule.cpp   # Module implementation
```

**Dependencies**: Core

**Suggested Libraries**: ENet, RakNet, or custom UDP/TCP

### 7. Animation Module (Lower Priority)
**Purpose**: Character and object animation system

**Benefits**:
- Skeletal animation
- Blend trees
- Animation states
- IK (Inverse Kinematics)

**Structure**:
```
src/Runtime/Animation/
├── Public/Animation/
│   ├── IAnimator.h         # Animator interface
│   ├── ISkeleton.h         # Skeleton interface
│   ├── AnimationTypes.h    # Animation types
│   └── AnimationModule.h   # Module interface
└── Private/
    ├── Animator.cpp        # Animator implementation
    └── AnimationModule.cpp # Module implementation
```

**Dependencies**: Core, Scene, Resource

### 8. Script Module (Lower Priority)
**Purpose**: Scripting support for gameplay logic

**Benefits**:
- Hot-reloading of gameplay code
- Rapid iteration
- Modding support
- Designer-friendly

**Structure**:
```
src/Runtime/Script/
├── Public/Script/
│   ├── IScriptEngine.h     # Script engine interface
│   ├── IScript.h           # Script interface
│   ├── ScriptTypes.h       # Script types
│   └── ScriptModule.h      # Module interface
└── Private/
    ├── LuaScript.cpp       # Lua implementation
    └── ScriptModule.cpp    # Module implementation
```

**Dependencies**: Core, Scene

**Suggested Languages**: Lua, Python, or JavaScript (V8)

### 9. UI Module (Lower Priority)
**Purpose**: Separate UI system from Editor

**Current State**: UI is in Editor using ImGui

**Benefits**:
- Runtime UI separate from editor UI
- Multiple UI backends
- UI layout system
- UI styling and theming

**Structure**:
```
src/Runtime/UI/
├── Public/UI/
│   ├── IUISystem.h         # UI system interface
│   ├── IUIWidget.h         # Widget interface
│   ├── UITypes.h           # UI types
│   └── UIModule.h          # Module interface
└── Private/
    ├── ImGuiUI.cpp         # ImGui implementation
    └── UIModule.cpp        # Module implementation
```

**Dependencies**: Core, Platform, Input, Renderer

## Module Priority Matrix

| Module    | Priority | Complexity | Impact | Dependencies          |
|-----------|----------|------------|--------|-----------------------|
| Window    | High     | Low        | High   | Platform              |
| Resource  | High     | Medium     | High   | Core, Platform        |
| Audio     | Medium   | Medium     | Medium | Core, Resource        |
| Scene     | Medium   | Medium     | High   | Core, Platform        |
| Physics   | Medium   | High       | Medium | Core, Scene           |
| Network   | Low      | High       | Medium | Core                  |
| Animation | Low      | High       | Low    | Core, Scene, Resource |
| Script    | Low      | Medium     | Medium | Core, Scene           |
| UI        | Low      | Medium     | Low    | Core, Platform, Input |

## Implementation Strategy

### Phase 1: Foundation Modules (3-6 months)
1. Window Module - Separate windowing from platform
2. Resource Module - Asset management system

### Phase 2: Core Gameplay Modules (6-12 months)
3. Scene Module - Entity-Component-System
4. Physics Module - Physics simulation
5. Audio Module - Sound system

### Phase 3: Advanced Features (12+ months)
6. Animation Module - Character animation
7. Script Module - Scripting support
8. Network Module - Multiplayer
9. UI Module - Runtime UI system

## Design Considerations

### Module Independence
Each module should be:
- Independently buildable
- Independently testable
- Optional (can be excluded from builds)
- Well-documented

### Interface Stability
Module interfaces should:
- Have minimal API surface
- Use versioning
- Maintain backward compatibility
- Document breaking changes

### Performance
Modules should:
- Minimize overhead
- Support multithreading
- Cache aggressively
- Profile regularly

### Cross-Platform
Modules should:
- Abstract platform differences
- Support Windows, Linux, macOS
- Consider mobile platforms
- Use standard C++17

## Integration with Current Architecture

New modules should follow the existing pattern:

```
src/Runtime/NewModule/
├── Public/NewModule/
│   ├── ModuleInterface.h   # Main interface
│   ├── ModuleTypes.h       # Types
│   └── ModuleAPI.h         # Module interface
├── Private/
│   ├── Implementation.cpp  # Implementation
│   └── Module.cpp          # Module boilerplate
└── xmake.lua               # Build config
```

Dependencies should follow the layer structure:
- Layer 1: Platform, Window
- Layer 2: Core, Input, Resource
- Layer 3: RHI, Audio, Scene, Physics
- Layer 4: Renderer, Animation, Script, UI
- Layer 5: Launch, Editor, Network

## Conclusion

The modular architecture enables incremental addition of these features. Start with high-priority modules (Window, Resource) and expand based on project needs.

Each module should:
1. Follow the established patterns
2. Have clear documentation
3. Include examples
4. Be tested independently
5. Integrate cleanly with existing modules

This approach ensures maintainability and allows the engine to grow organically while maintaining code quality.
