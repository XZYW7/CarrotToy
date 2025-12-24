# RHI (Render Hardware Interface) Documentation

## Overview

The RHI (Render Hardware Interface) is a graphics API abstraction layer that decouples rendering code from specific graphics APIs like OpenGL, Vulkan, DirectX, or Metal. This design pattern, popularized by game engines like Unreal Engine and Unity, provides several key benefits:

- **Multi-API Support**: Write rendering code once, run on multiple graphics APIs
- **Platform Flexibility**: Easy to add support for new platforms and APIs
- **Testing**: Mock interfaces for unit testing without requiring GPU
- **Profiling**: Single instrumentation point for all graphics calls
- **Optimization**: Backend-specific optimizations without changing high-level code

## Architecture

### Core Interfaces

The RHI consists of several key interfaces defined in `include/RHI.h`:

#### IRHIDevice
The main entry point for all RHI operations. Responsibilities include:
- Creating graphics resources (buffers, shaders, textures, etc.)
- Managing rendering state (viewport, depth test, blending, etc.)
- Issuing draw calls
- Clearing render targets

#### IRHIBuffer
Represents GPU buffer objects for storing vertex, index, or uniform data.

#### IRHIShader
Individual shader stage (vertex, fragment, geometry, compute).

#### IRHIShaderProgram
Complete shader program linking multiple shader stages together.

#### IRHITexture
2D texture with configurable format, filtering, and wrapping modes.

#### IRHIFramebuffer
Offscreen render target with color and depth attachments.

#### IRHIVertexArray
Vertex input layout configuration describing how vertex data is laid out.

## Type System

The RHI uses strongly-typed enums defined in `include/RHITypes.h`:

- `GraphicsAPI`: OpenGL, Vulkan, DirectX11, DirectX12, Metal
- `BufferType`: Vertex, Index, Uniform
- `BufferUsage`: Static, Dynamic, Stream
- `ShaderType`: Vertex, Fragment, Geometry, Compute
- `TextureFormat`: RGB8, RGBA8, RGBA16F, RGBA32F, Depth24Stencil8, etc.
- `PrimitiveTopology`: TriangleList, TriangleStrip, LineList, etc.
- `CompareFunc`: Never, Less, Equal, LessEqual, Greater, etc.
- `BlendFactor`: Zero, One, SrcColor, SrcAlpha, etc.
- `CullMode`: None, Front, Back

## OpenGL Implementation

The OpenGL backend is implemented in `include/OpenGLRHI.h` and `src/OpenGLRHI.cpp`. It provides concrete implementations of all RHI interfaces using OpenGL 3.3+ Core Profile.

### Key Classes

- `OpenGLRHIDevice`: Main device implementation
- `OpenGLBuffer`: Wraps OpenGL buffer objects
- `OpenGLShader`: Wraps OpenGL shader objects
- `OpenGLShaderProgram`: Wraps OpenGL program objects
- `OpenGLTexture`: Wraps OpenGL texture objects
- `OpenGLFramebuffer`: Wraps OpenGL framebuffer objects
- `OpenGLVertexArray`: Wraps OpenGL vertex array objects

### Type Conversion

The OpenGL implementation includes helper functions to convert RHI types to OpenGL constants:
- `toGLBufferType()`: BufferType → GL_ARRAY_BUFFER, etc.
- `toGLShaderType()`: ShaderType → GL_VERTEX_SHADER, etc.
- `toGLTextureFormat()`: TextureFormat → GL_RGBA, etc.
- `toGLPrimitiveTopology()`: PrimitiveTopology → GL_TRIANGLES, etc.

## Usage Example

See `src/RHIExample.cpp` for a complete working example. Here's a quick overview:

```cpp
// 1. Create RHI device
auto rhiDevice = RHI::createRHIDevice(RHI::GraphicsAPI::OpenGL);
rhiDevice->initialize();

// 2. Create vertex buffer
float vertices[] = { /* vertex data */ };
RHI::BufferDesc bufferDesc;
bufferDesc.type = RHI::BufferType::Vertex;
bufferDesc.usage = RHI::BufferUsage::Static;
bufferDesc.size = sizeof(vertices);
bufferDesc.initialData = vertices;
auto vertexBuffer = rhiDevice->createBuffer(bufferDesc);

// 3. Create shaders
RHI::ShaderDesc vertexShaderDesc;
vertexShaderDesc.type = RHI::ShaderType::Vertex;
vertexShaderDesc.source = vertexShaderCode;
vertexShaderDesc.sourceSize = strlen(vertexShaderCode);
auto vertexShader = rhiDevice->createShader(vertexShaderDesc);
vertexShader->compile();

// 4. Create shader program
auto shaderProgram = rhiDevice->createShaderProgram();
shaderProgram->attachShader(vertexShader.get());
shaderProgram->attachShader(fragmentShader.get());
shaderProgram->link();

// 5. Set up vertex array
auto vertexArray = rhiDevice->createVertexArray();
vertexArray->bind();
vertexArray->setVertexBuffer(vertexBuffer.get(), 0);

RHI::VertexAttribute posAttr;
posAttr.location = 0;
posAttr.componentCount = 3;
posAttr.offset = 0;
posAttr.stride = 0;  // 0 = automatically calculated
vertexArray->setVertexAttribute(posAttr);

// 6. Render
rhiDevice->setViewport(0, 0, width, height);
rhiDevice->setDepthTest(true);
rhiDevice->clearColor(0.2f, 0.2f, 0.2f, 1.0f);
rhiDevice->clear(true, true, false);

shaderProgram->bind();
shaderProgram->setUniformFloat("time", currentTime);
vertexArray->bind();
rhiDevice->draw(RHI::PrimitiveTopology::TriangleList, vertexCount);

// 7. Cleanup (automatic via smart pointers)
rhiDevice->shutdown();
```

## Adding a New Backend

To add support for a new graphics API (e.g., Vulkan):

1. **Create header file** (`include/VulkanRHI.h`):
   - Declare `VulkanRHIDevice` implementing `IRHIDevice`
   - Declare resource classes: `VulkanBuffer`, `VulkanShader`, etc.

2. **Create implementation file** (`src/VulkanRHI.cpp`):
   - Implement all interface methods
   - Add type conversion functions (RHI types → Vulkan types)
   - Manage Vulkan-specific resources (VkInstance, VkDevice, etc.)

3. **Update factory function** in `src/OpenGLRHI.cpp`:
   ```cpp
   std::shared_ptr<IRHIDevice> createRHIDevice(GraphicsAPI api) {
       switch (api) {
           case GraphicsAPI::OpenGL:
               return std::make_shared<OpenGLRHIDevice>();
           case GraphicsAPI::Vulkan:
               return std::make_shared<VulkanRHIDevice>();
           // ...
       }
   }
   ```

4. **Test the implementation**:
   - Create unit tests for each resource type
   - Verify rendering output matches other backends
   - Performance benchmark against other APIs

## Design Patterns

The RHI uses several design patterns:

### Abstract Factory
`IRHIDevice` acts as an abstract factory, creating platform-specific resources through a common interface.

### Bridge Pattern
Separates the abstraction (RHI interfaces) from implementation (OpenGL, Vulkan, etc.), allowing them to vary independently.

### Strategy Pattern
Different rendering backends can be swapped at runtime by changing the graphics API.

### RAII (Resource Acquisition Is Initialization)
Resources are managed through smart pointers and destructors, ensuring proper cleanup.

## Resource Management

### Ownership
- All RHI resources are returned as `std::shared_ptr`
- The RHI device doesn't track resources after creation
- Application is responsible for keeping resources alive while in use
- Resources are automatically released when reference count reaches zero

### Lifecycle
```
Create → Initialize → Use → Release
   ↓         ↓         ↓        ↓
Device   Compile    Bind    Destructor
         Link       Draw    (automatic)
         Attach
```

## Performance Considerations

### Uniform Location Caching
The OpenGL implementation caches uniform locations to avoid repeated `glGetUniformLocation` calls:
```cpp
std::unordered_map<std::string, int> uniformLocationCache;
```

### State Changes
Minimize state changes by:
- Batching draw calls with same shader program
- Grouping objects by material
- Sorting by render state to minimize transitions

### Buffer Updates
Choose appropriate `BufferUsage`:
- `Static`: Data set once, drawn many times (geometry)
- `Dynamic`: Data changes occasionally (transform matrices)
- `Stream`: Data changes every frame (particle systems)

## Thread Safety

Current implementation is **single-threaded**:
- OpenGL context is bound to a single thread
- No synchronization primitives needed
- Resources must be created and used on same thread

Future considerations for multi-threading:
- Command list recording on worker threads
- Resource upload on background threads
- Async shader compilation

## Future Enhancements

### Planned Features
- **Vulkan Backend**: High-performance modern API support
- **DirectX 12 Backend**: Windows-native rendering
- **Metal Backend**: macOS/iOS optimized rendering
- **Compute Shaders**: GPU compute support
- **Ray Tracing**: Hardware-accelerated ray tracing (DXR, VK_KHR_ray_tracing)
- **Multi-Threading**: Command buffer recording on multiple threads
- **Resource Barriers**: Explicit resource state transitions
- **Pipeline State Objects**: Pre-compiled rendering state

### API Differences to Consider

When implementing new backends, be aware of API differences:

| Feature | OpenGL | Vulkan | DirectX 12 |
|---------|--------|--------|------------|
| State Model | State machine | Command buffers | Command lists |
| Shader Format | GLSL | SPIR-V | DXIL/DXBC |
| Memory Management | Automatic | Manual | Manual |
| Resource Barriers | Implicit | Explicit | Explicit |
| Pipeline Creation | Runtime | Pre-compiled | Pre-compiled |

## Best Practices

1. **Check for Errors**: Always check return values from compile(), link(), isComplete()
2. **Validate Resources**: Call isValid() before using resources
3. **Release Resources**: Use RAII or explicitly call release() when done
4. **Cache Uniforms**: Don't set uniforms every frame if values don't change
5. **Minimize State Changes**: Group draw calls to reduce overhead
6. **Use Appropriate Buffer Usage**: Match BufferUsage to actual usage pattern
7. **Set Vertex Stride**: Set the stride field in VertexAttribute (0 for automatic calculation, or specify bytes between vertices)
8. **Attach Shaders Before Linking**: Always attach all shaders to a program before calling link()

## Known Limitations

1. **External Texture Ownership**: When attaching external textures to framebuffers, caller must keep them alive
   - Internal textures created by framebuffer constructor are automatically managed

2. **Backend Type Safety**: Shaders and resources must be created from the same RHI device
   - Mixing resources from different backends (e.g., OpenGL shader with Vulkan program) will fail gracefully with error message

## Debugging

### Shader Compilation Errors
```cpp
if (!shader->compile()) {
    std::cerr << "Shader error: " << shader->getCompileErrors() << std::endl;
}
```

### Shader Linking Errors
```cpp
if (!program->link()) {
    std::cerr << "Linking error: " << program->getLinkErrors() << std::endl;
}
```

### Framebuffer Completeness
```cpp
if (!framebuffer->isComplete()) {
    std::cerr << "Framebuffer is not complete!" << std::endl;
}
```

## References

- **Unreal Engine RHI**: [Unreal Engine Documentation](https://docs.unrealengine.com/)
- **Unity Graphics API**: [Unity Manual](https://docs.unity3d.com/)
- **BGFX**: [BGFX Library](https://github.com/bkaradzic/bgfx)
- **The-Forge**: [The-Forge Renderer](https://github.com/ConfettiFX/The-Forge)
- **OpenGL 3.3 Core**: [OpenGL Specification](https://www.opengl.org/registry/)

## Contributing

To contribute to the RHI:

1. Follow existing code style and patterns
2. Add comprehensive error checking
3. Document new interfaces and functions
4. Create unit tests for new functionality
5. Update this documentation with your changes
6. Submit a pull request with detailed description

## License

The RHI implementation is part of CarrotToy and is licensed under the MIT License.
