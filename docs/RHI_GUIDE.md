# RHI (Render Hardware Interface) System

## Overview

The RHI (Render Hardware Interface) is a graphics API abstraction layer that allows CarrotToy to support multiple rendering backends including OpenGL, DirectX 12, and Vulkan. This abstraction enables cross-platform rendering and makes it easy to switch between different graphics APIs at runtime.

## Architecture

### Core Components

1. **IRHIDevice** - Main device interface for creating resources and issuing rendering commands
2. **IRHIResource** - Base interface for all GPU resources
3. **IRHIBuffer** - Buffer resources (vertex, index, uniform buffers)
4. **IRHIShader** - Individual shader stages
5. **IRHIShaderProgram** - Complete shader programs/pipeline states
6. **IRHITexture** - Texture resources
7. **IRHIFramebuffer** - Render targets
8. **IRHIVertexArray** - Input assembly/vertex layout
9. **IRHIUniformBuffer** - Uniform buffer objects (constant buffers in D3D12)

### Supported Backends

#### OpenGL (Fully Implemented)
- **Status**: ✅ Production Ready
- **Platform**: Windows, Linux, macOS
- **Version**: OpenGL 3.3+
- **Implementation**: `OpenGLRHI.h/cpp`

#### Vulkan (Stub Implementation)
- **Status**: 🔧 Interface Complete, Implementation Stub
- **Platform**: Windows, Linux, Android
- **Version**: Vulkan 1.0+
- **Implementation**: `VulkanRHI.h/cpp`
- **Requirements**: Vulkan SDK must be linked for full implementation

#### DirectX 12 (Stub Implementation)
- **Status**: 🔧 Interface Complete, Implementation Stub
- **Platform**: Windows 10+
- **Version**: DirectX 12
- **Implementation**: `D3D12RHI.h/cpp`
- **Requirements**: D3D12.lib and DXGI.lib must be linked for full implementation

## Usage

### Basic Initialization

```cpp
#include "RHI/RHI.h"

// Create RHI device for desired graphics API
auto rhiDevice = CarrotToy::RHI::createRHIDevice(CarrotToy::RHI::GraphicsAPI::OpenGL);

// Initialize the device
if (!rhiDevice->initialize()) {
    std::cerr << "Failed to initialize RHI device" << std::endl;
    return false;
}

// Set as global device (optional, for convenience)
CarrotToy::RHI::setGlobalDevice(rhiDevice);
```

### Creating Resources

#### Buffers

```cpp
// Create vertex buffer
CarrotToy::RHI::BufferDesc vbDesc;
vbDesc.type = CarrotToy::RHI::BufferType::Vertex;
vbDesc.usage = CarrotToy::RHI::BufferUsage::Static;
vbDesc.size = vertices.size() * sizeof(float);
vbDesc.initialData = vertices.data();

auto vertexBuffer = rhiDevice->createBuffer(vbDesc);

// Create index buffer
CarrotToy::RHI::BufferDesc ibDesc;
ibDesc.type = CarrotToy::RHI::BufferType::Index;
ibDesc.usage = CarrotToy::RHI::BufferUsage::Static;
ibDesc.size = indices.size() * sizeof(uint32_t);
ibDesc.initialData = indices.data();

auto indexBuffer = rhiDevice->createBuffer(ibDesc);
```

#### Textures

```cpp
CarrotToy::RHI::TextureDesc texDesc;
texDesc.width = 512;
texDesc.height = 512;
texDesc.format = CarrotToy::RHI::TextureFormat::RGBA8;
texDesc.minFilter = CarrotToy::RHI::TextureFilter::Linear;
texDesc.magFilter = CarrotToy::RHI::TextureFilter::Linear;
texDesc.initialData = imageData;

auto texture = rhiDevice->createTexture(texDesc);
```

#### Shaders

```cpp
// Create vertex shader
CarrotToy::RHI::ShaderDesc vsDesc;
vsDesc.type = CarrotToy::RHI::ShaderType::Vertex;
vsDesc.source = vertexShaderSource.c_str();
vsDesc.sourceSize = vertexShaderSource.size();

auto vertexShader = rhiDevice->createShader(vsDesc);
if (!vertexShader->compile()) {
    std::cerr << "Vertex shader compilation failed: " 
              << vertexShader->getCompileErrors() << std::endl;
}

// Create fragment shader
CarrotToy::RHI::ShaderDesc fsDesc;
fsDesc.type = CarrotToy::RHI::ShaderType::Fragment;
fsDesc.source = fragmentShaderSource.c_str();
fsDesc.sourceSize = fragmentShaderSource.size();

auto fragmentShader = rhiDevice->createShader(fsDesc);
fragmentShader->compile();

// Create shader program
auto shaderProgram = rhiDevice->createShaderProgram();
shaderProgram->attachShader(vertexShader.get());
shaderProgram->attachShader(fragmentShader.get());

if (!shaderProgram->link()) {
    std::cerr << "Shader program linking failed: " 
              << shaderProgram->getLinkErrors() << std::endl;
}
```

#### Vertex Arrays

```cpp
// Create vertex array
auto vertexArray = rhiDevice->createVertexArray();
vertexArray->bind();

// Set vertex buffer
vertexArray->setVertexBuffer(vertexBuffer.get(), 0);

// Set index buffer
vertexArray->setIndexBuffer(indexBuffer.get());

// Define vertex attributes
CarrotToy::RHI::VertexAttribute posAttr;
posAttr.location = 0;
posAttr.binding = 0;
posAttr.offset = 0;
posAttr.componentCount = 3;
posAttr.stride = 6 * sizeof(float);
posAttr.normalized = false;
vertexArray->setVertexAttribute(posAttr);

CarrotToy::RHI::VertexAttribute normalAttr;
normalAttr.location = 1;
normalAttr.binding = 0;
normalAttr.offset = 3 * sizeof(float);
normalAttr.componentCount = 3;
normalAttr.stride = 6 * sizeof(float);
normalAttr.normalized = false;
vertexArray->setVertexAttribute(normalAttr);

vertexArray->unbind();
```

### Rendering

```cpp
// Begin frame
rhiDevice->clearColor(0.2f, 0.2f, 0.2f, 1.0f);
rhiDevice->clear(true, true, false);

// Set viewport
rhiDevice->setViewport(0, 0, width, height);

// Set render state
rhiDevice->setDepthTest(true);
rhiDevice->setDepthWrite(true);
rhiDevice->setCullMode(CarrotToy::RHI::CullMode::Back);

// Bind shader and resources
shaderProgram->bind();
vertexArray->bind();

// Set uniforms
shaderProgram->setUniformMatrix4("model", glm::value_ptr(modelMatrix));
shaderProgram->setUniformMatrix4("view", glm::value_ptr(viewMatrix));
shaderProgram->setUniformMatrix4("projection", glm::value_ptr(projectionMatrix));

// Draw
rhiDevice->drawIndexed(
    CarrotToy::RHI::PrimitiveTopology::TriangleList, 
    indexCount, 
    0
);

// Cleanup
vertexArray->unbind();
shaderProgram->unbind();
```

## Extending the RHI

### Implementing a New Backend

To implement a new graphics API backend:

1. **Create header file** (e.g., `MetalRHI.h`)
   - Define backend-specific resource classes inheriting from RHI interfaces
   - Define backend-specific device class inheriting from `IRHIDevice`

2. **Create implementation file** (e.g., `MetalRHI.cpp`)
   - Implement all virtual methods from base interfaces
   - Handle backend-specific initialization and resource management

3. **Update factory function** in `OpenGLRHI.cpp`:
```cpp
std::shared_ptr<IRHIDevice> createRHIDevice(GraphicsAPI api) {
    switch (api) {
        case GraphicsAPI::Metal:
            return std::make_shared<MetalRHIDevice>();
        // ... other cases
    }
}
```

4. **Add to build system** - Update `xmake.lua` or CMakeLists.txt to include new files

### Adding New RHI Features

To add new functionality to the RHI:

1. **Update base interface** in `RHI.h`:
```cpp
class IRHIDevice {
public:
    // Add new method
    virtual void newFeature() = 0;
};
```

2. **Implement in all backends**:
   - OpenGL: `OpenGLRHI.cpp`
   - Vulkan: `VulkanRHI.cpp`
   - DirectX 12: `D3D12RHI.cpp`

3. **Update documentation** - Add usage examples to this README

## Backend-Specific Notes

### OpenGL Backend

The OpenGL backend is fully implemented and tested. It supports:
- OpenGL 3.3+ core profile
- Vertex Array Objects (VAO)
- Uniform Buffer Objects (UBO)
- Framebuffer Objects (FBO)
- GLSL shader compilation
- SPIR-V shader loading (with GL_ARB_gl_spirv)

### Vulkan Backend (Stub)

The Vulkan backend provides a complete interface but returns stub implementations. To complete:

1. Link Vulkan SDK (`vulkan-1.lib` on Windows, `libvulkan.so` on Linux)
2. Include `vulkan/vulkan.h`
3. Implement:
   - Instance and device creation
   - Memory allocation and management
   - Command buffer recording
   - Pipeline creation
   - Descriptor set management
   - Synchronization primitives

### DirectX 12 Backend (Stub)

The D3D12 backend provides a complete interface but returns stub implementations. To complete:

1. Link D3D12 libraries (`d3d12.lib`, `dxgi.lib`, `d3dcompiler.lib`)
2. Include DirectX headers
3. Implement:
   - Device and command queue creation
   - Command allocator and list management
   - Root signature creation
   - Pipeline state objects (PSO)
   - Descriptor heaps
   - Resource barriers and transitions

## Performance Considerations

- **Resource Creation**: Create resources during initialization, not per-frame
- **Uniform Updates**: Use Uniform Buffer Objects for frequently updated data
- **State Changes**: Minimize state changes; batch draw calls with same state
- **Buffer Updates**: Use appropriate buffer usage flags (Static/Dynamic/Stream)

## Migration from Direct OpenGL

If you have existing OpenGL code:

1. Replace `glGen*` calls with `rhiDevice->create*` calls
2. Replace `glBind*` calls with `resource->bind()` calls
3. Replace `glUniform*` calls with `shaderProgram->setUniform*` calls
4. Replace `glDraw*` calls with `rhiDevice->draw*` calls
5. Replace state-setting `gl*` calls with `rhiDevice->set*` calls

## Future Enhancements

- [ ] Compute shader support
- [ ] Geometry shader support
- [ ] Tessellation shader support
- [ ] Multi-threaded command recording
- [ ] Resource binding groups/descriptor sets
- [ ] Push constants
- [ ] Query objects (occlusion, timer, etc.)
- [ ] Indirect drawing
- [ ] Instance rendering

## Troubleshooting

### "RHI device failed to initialize"
- Ensure graphics drivers are up to date
- Verify platform supports required graphics API version
- Check console output for specific error messages

### "Shader compilation failed"
- Check shader syntax for target API
- Ensure shader version matches API requirements
- Review compilation errors from `getCompileErrors()`

### Backend not available
- Stub implementations will log warnings when used
- Check that required SDK/libraries are linked
- Verify platform supports the graphics API

## References

- [OpenGL Reference](https://www.khronos.org/opengl/)
- [Vulkan Reference](https://www.khronos.org/vulkan/)
- [DirectX 12 Reference](https://docs.microsoft.com/en-us/windows/win32/direct3d12/)
- [SPIR-V](https://www.khronos.org/spir/)
