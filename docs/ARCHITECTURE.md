# CarrotToy Architecture

## Overview

CarrotToy is designed as a modular material editor with clear separation of concerns. The architecture follows a component-based approach with distinct systems for rendering, materials, and UI.

## System Architecture

```
┌─────────────────────────────────────────────────────┐
│                   Application Layer                  │
│                     (main.cpp)                       │
└───────────────┬────────────────────────────┬─────────┘
                │                            │
    ┌───────────▼──────────┐    ┌───────────▼──────────┐
    │   Material Editor    │    │      Renderer        │
    │   (MaterialEditor)   │◄───┤   (Renderer.cpp)    │
    └───────────┬──────────┘    └───────────┬──────────┘
                │                            │
    ┌───────────▼────────────────────────────▼──────────┐
    │            Material System                        │
    │    (Material, Shader, MaterialManager)            │
    └───────────┬───────────────────────────────────────┘
                │
    ┌───────────▼──────────┐    ┌────────────────────┐
    │   RHI Abstraction    │◄───┤  Graphics Backend  │
    │   (RHI.h/cpp)        │    │  (OpenGLRHI.cpp)   │
    └───────────┬──────────┘    └────────────────────┘
                │
    ┌───────────▼──────────┐    ┌────────────────────┐
    │  Platform Layer      │◄───┤   GLFW Backend     │
    │  (Platform.h/cpp)    │    │   Window/Input     │
    └──────────────────────┘    └────────────────────┘
                │
    ┌───────────▼──────────┐
    │   Ray Tracer         │
    │   (RayTracer.cpp)    │
    └──────────────────────┘
```

## Core Components

### 0. Platform Layer (`Platform.h/cpp`)

**Purpose**: Abstracts platform-specific window and input management.

**Responsibilities**:
- Platform initialization and shutdown
- Window creation and management
- Event polling and input handling
- Graphics context creation and management
- Time queries and display information

**Key Interfaces**:

#### IPlatform
- Platform initialization and lifecycle management
- Window creation through `createWindow()`
- Event polling through `pollEvents()`
- Display/monitor queries
- Platform time queries

#### IPlatformWindow
- Window state management (show, hide, focus, close)
- Window property management (title, size, framebuffer size)
- Graphics context operations (make current, swap buffers, get proc address)
- Resize callbacks
- Native handle access for RHI integration

**Implementation**:
- Uses GLFW for cross-platform support (Windows, Linux, macOS)
- Wraps platform-specific functionality behind clean interfaces
- Provides factory function `createPlatform()` for platform creation
- Compile-time platform detection for platform-specific code

**Design Benefits**:
- Decouples rendering code from platform specifics
- Enables future native platform implementations (Win32, X11, Cocoa)
- Simplifies RHI initialization by handling context creation
- Provides consistent interface across all platforms

### 1. Renderer (`Renderer.h/cpp`)

**Purpose**: Manages the rendering pipeline using the Platform abstraction.

**Responsibilities**:
- Platform and window initialization through Platform layer
- OpenGL context initialization via Platform
- Frame rendering loop
- Preview geometry generation (sphere)
- Framebuffer management
- Render mode switching (Rasterization/Ray Tracing)
- RHI device creation and management

**Key Methods**:
- `initialize()`: Sets up platform, window, and graphics context
- `renderMaterialPreview()`: Renders a sphere with the given material
- `renderScene()`: Main scene rendering function
- `exportSceneForRayTracing()`: Exports scene data for offline rendering

**Platform Integration**:
- Uses `Platform::createPlatform()` to create platform instance
- Creates window through `platform->createWindow()`
- Manages graphics context through platform window
- Initializes RHI device after platform setup

**Design Patterns**:
- Singleton-like pattern (one renderer instance per application)
- RAII for resource management
- Dependency injection (uses Platform abstraction)

### 2. Material System (`Material.h/cpp`)

**Purpose**: Manages shaders and materials with their parameters.

**Components**:

#### Shader Class
- Loads and compiles GLSL shaders
- Supports hot-reloading
- Provides uniform setters for all common types
- Handles compilation errors gracefully

#### Material Class
- Associates a shader with parameter values
- Stores material properties (albedo, metallic, roughness, etc.)
- Applies parameters to shader uniforms on bind
- Type-safe parameter storage

#### MaterialManager Class
- Singleton pattern for global material access
- Creates and manages all materials
- Material lookup by name
- Lifecycle management

**Design Patterns**:
- Factory pattern (MaterialManager creates materials)
- Singleton pattern (MaterialManager)
- Observer pattern potential (for material changes)

### 3. Material Editor (`MaterialEditor.h/cpp`)

**Purpose**: Provides ImGui-based UI for material editing.

**UI Panels**:

1. **Materials Panel**
   - Lists all available materials
   - Material selection
   - Material creation dialog

2. **Material Properties Panel**
   - Displays current material parameters
   - Interactive parameter editing (sliders, color pickers)
   - Shader editing button

3. **Shader Editor Panel**
   - Multi-line text editor for vertex/fragment shaders
   - Real-time compilation
   - Error display
   - Save/load functionality

4. **Preview Window**
   - Render mode selection
   - Scene export controls
   - Ray tracing options

**Design Patterns**:
- Observer pattern (callbacks for shader recompilation)
- MVC pattern (Editor is the View/Controller, Material is the Model)

### 4. Ray Tracer (`RayTracer.h/cpp`)

**Purpose**: Offline ray tracing for high-quality renders.

**Features**:
- CPU-based ray tracing
- Simple sphere intersection
- Basic lighting calculations
- PNG output via STB

**Components**:
- `Ray` structure: Origin and direction
- `Hit` structure: Intersection information
- `Scene` structure: Geometry and materials
- Ray-geometry intersection
- Shading calculations

**Design Patterns**:
- Functional programming style
- Data-oriented design (structures of arrays)

### 5. RHI (Render Hardware Interface) (`RHI.h`, `OpenGLRHI.h/cpp`)

**Purpose**: Provides an abstraction layer over graphics APIs, enabling multi-API support.

**Components**:

#### RHI Core Interfaces
- `IRHIDevice`: Main entry point for creating and managing graphics resources
- `IRHIBuffer`: Abstraction for vertex, index, and uniform buffers
- `IRHIShader`: Individual shader stage (vertex, fragment, geometry)
- `IRHIShaderProgram`: Complete shader program with multiple stages
- `IRHITexture`: 2D texture with configurable format and filtering
- `IRHIFramebuffer`: Offscreen render target with color and depth attachments
- `IRHIVertexArray`: Vertex input layout configuration

#### OpenGL Implementation
- `OpenGLRHIDevice`: OpenGL-specific implementation of IRHIDevice
- `OpenGLBuffer`, `OpenGLShader`, etc.: Concrete implementations wrapping OpenGL objects
- Type conversion utilities for mapping RHI enums to OpenGL constants
- Resource lifecycle management with RAII principles

**Key Features**:
- **API Abstraction**: Decouples rendering code from specific graphics API
- **Future Extensibility**: Designed to support Vulkan, DirectX, Metal
- **Type Safety**: Strongly-typed enums for buffer types, texture formats, etc.
- **Resource Management**: Smart pointers and RAII for automatic cleanup
- **State Management**: Centralized rendering state (depth test, blending, culling)

**Design Patterns**:
- Abstract Factory pattern (IRHIDevice creates resources)
- Bridge pattern (separates abstraction from implementation)
- RAII for resource management
- Strategy pattern (different backends for different APIs)

**Usage Example**:
```cpp
// Create RHI device
auto rhiDevice = createRHIDevice(GraphicsAPI::OpenGL);
rhiDevice->initialize();

// Create buffer
BufferDesc desc;
desc.type = BufferType::Vertex;
desc.size = sizeof(vertices);
desc.initialData = vertices;
auto buffer = rhiDevice->createBuffer(desc);

// Set rendering state
rhiDevice->setDepthTest(true);
rhiDevice->setViewport(0, 0, width, height);

// Draw
rhiDevice->draw(PrimitiveTopology::TriangleList, vertexCount);
```

**Benefits**:
1. **Multi-Platform Support**: Easy to add Vulkan, DirectX, or Metal backends
2. **Testing**: Can mock IRHIDevice for unit tests
3. **Profiling**: Single place to instrument all graphics calls
4. **Optimization**: Backend-specific optimizations without changing high-level code
5. **Maintenance**: Changes to one API don't affect others

## Data Flow

### Material Editing Flow

```
User Input (ImGui)
    │
    ▼
MaterialEditor::renderMaterialParameter()
    │
    ▼
Material::setFloat/setVec3/etc()
    │
    ▼
Material parameter storage updated
    │
    ▼
Renderer::renderMaterialPreview()
    │
    ▼
Material::bind()
    │
    ▼
Shader uniforms updated
    │
    ▼
OpenGL rendering
```

### Shader Recompilation Flow

```
User edits shader in ShaderEditor
    │
    ▼
"Compile and Apply" button clicked
    │
    ▼
Shader::compile(vertexSource, fragmentSource)
    │
    ├─► Compile vertex shader
    ├─► Compile fragment shader
    └─► Link program
    │
    ▼
Success/Error feedback to console
    │
    ▼
Next frame renders with new shader
```

### Ray Tracing Flow

```
User clicks "Export Scene"
    │
    ▼
Renderer::exportSceneForRayTracing()
    │
    ▼
Scene data written to file
    │
    ▼
RayTracer::loadScene()
    │
    ▼
RayTracer::render()
    │
    ├─► Generate rays for each pixel
    ├─► Trace rays through scene
    ├─► Compute lighting
    └─► Write to image buffer
    │
    ▼
Save PNG file
```

## Memory Management

### Resource Ownership

1. **Shaders**: `shared_ptr` managed, owned by Materials
2. **Materials**: `shared_ptr` managed, owned by MaterialManager
3. **Material Parameters**: Raw pointers in ShaderParameter, owned by Material
4. **OpenGL Resources**: RAII pattern with cleanup in destructors

### Lifecycle

```
Application Start
    │
    ├─► Renderer created
    ├─► MaterialManager (singleton) initialized
    ├─► Default shaders loaded
    ├─► Default materials created
    └─► MaterialEditor initialized
    │
Main Loop
    │
    ├─► Frame rendering
    ├─► UI updates
    └─► User interactions
    │
Application Shutdown
    │
    ├─► MaterialEditor::shutdown()
    ├─► Materials destroyed (via MaterialManager)
    ├─► Shaders destroyed (via shared_ptr)
    ├─► OpenGL resources cleaned up
    └─► Renderer::shutdown()
```

## Thread Safety

Current implementation is **single-threaded**:
- All operations occur on the main thread
- OpenGL context is not shared across threads
- No mutex protection needed for current design

**Future multi-threading considerations**:
- Shader compilation could be async
- Ray tracing could use thread pool
- Material updates need synchronization if done from background threads

## Extension Points

### Adding New Graphics API Backends

1. Create new header/implementation files (e.g., `VulkanRHI.h/cpp`)
2. Implement all `IRHIDevice` interfaces
3. Implement all resource classes (`IRHIBuffer`, `IRHIShader`, etc.)
4. Add case to `createRHIDevice()` factory function
5. Handle API-specific initialization and cleanup

Example structure for Vulkan:
```cpp
class VulkanRHIDevice : public IRHIDevice {
    VkInstance instance;
    VkDevice device;
    VkQueue graphicsQueue;
    // ... Vulkan-specific resources
    
    bool initialize() override {
        // Create VkInstance, select physical device, create logical device
    }
    
    std::shared_ptr<IRHIBuffer> createBuffer(const BufferDesc& desc) override {
        return std::make_shared<VulkanBuffer>(desc, device);
    }
    // ... implement all virtual methods
};
```

### Adding New Material Parameter Types

1. Add enum value to `ShaderParamType`
2. Add setter in `Material` class
3. Add uniform setter in `Shader` class
4. Add UI rendering in `MaterialEditor::renderMaterialParameter()`

### Adding New Rendering Techniques

1. Extend `Renderer::RenderMode` enum
2. Implement rendering path in `Renderer::renderScene()`
3. Add UI option in `MaterialEditor::showPreviewWindow()`

### Supporting New Geometry Types

1. Add geometry generation function in `Renderer`
2. Extend `RayTracer::Scene` structure
3. Implement ray-geometry intersection

### Texture Support

1. Add `Texture` class
2. Extend `Material` to store texture references
3. Add texture binding in `Material::bind()`
4. Add texture UI in `MaterialEditor`

## Performance Considerations

### Optimization Strategies

1. **Shader Compilation**
   - Cache compiled shaders
   - Async compilation (future)
   - Shader variants/permutations

2. **Rendering**
   - Frustum culling (when scene complexity increases)
   - Level of detail (LOD) for preview geometry
   - Instanced rendering for multiple materials

3. **UI**
   - Lazy updates (only redraw when changed)
   - Cached text rendering
   - Minimize draw calls

4. **Ray Tracing**
   - BVH acceleration structure
   - Parallel ray tracing
   - Progressive rendering

## Dependencies

### Direct Dependencies
- **GLFW**: Window and input
- **GLAD**: OpenGL loading
- **GLM**: Mathematics
- **ImGui**: UI framework
- **STB**: Image I/O

### Platform Dependencies
- **OpenGL 3.3+**: Graphics API
- **C++17**: Language features (shared_ptr, filesystem, etc.)

## Build Systems

### Xmake (Primary)
- Automatic dependency management
- Cross-platform support
- Simple configuration

### CMake (Alternative)
- More widespread adoption
- Better IDE integration
- Manual dependency management

## Testing Strategy

### Unit Testing (Future)
- Shader compilation tests
- Material parameter validation
- Ray-geometry intersection tests

### Integration Testing
- End-to-end material creation
- Shader hot-reload verification
- UI interaction tests

### Manual Testing
- Visual verification of materials
- Performance profiling
- Cross-platform validation

## Security Considerations

1. **Shader Code Injection**: User-provided shaders run on GPU
   - Validated during compilation
   - Limited to GLSL syntax
   - No file system access from shaders

2. **File I/O**: Scene export/import
   - Validate file paths
   - Check file permissions
   - Sanitize file content

3. **Resource Limits**
   - Max material count
   - Max parameter count per material
   - Shader code size limits

## Future Enhancements

### Short-term
- [ ] Texture support
- [ ] More geometry types (cube, plane, custom meshes)
- [ ] Material save/load to file
- [ ] Shader preset library

### Medium-term
- [ ] Node-based shader editor
- [ ] Real-time ray tracing (DXR/OptiX)
- [ ] Post-processing effects
- [ ] Animation system for materials

### Long-term
- [ ] Network-based collaborative editing
- [ ] Plugin system for custom shaders
- [ ] Material marketplace integration
- [ ] VR preview mode
