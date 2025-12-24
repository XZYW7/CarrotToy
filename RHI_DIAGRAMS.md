# RHI Architecture Diagrams

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Application Layer                        │
│                  (Renderer, Material, etc.)                  │
└────────────────────────┬────────────────────────────────────┘
                         │
                         │ Uses
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    RHI Interface Layer                       │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ IRHIDevice   │  │ IRHIBuffer   │  │ IRHIShader   │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ IRHITexture  │  │ IRHIFBO      │  │ IRHIVArray   │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
└────────────────────────┬────────────────────────────────────┘
                         │
                         │ Implements
                         ▼
┌─────────────────────────────────────────────────────────────┐
│              Backend Implementation Layer                    │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │              OpenGL Backend                          │  │
│  │  ┌──────────────┐  ┌──────────────┐                 │  │
│  │  │OpenGLBuffer  │  │OpenGLShader  │                 │  │
│  │  └──────────────┘  └──────────────┘                 │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │            Vulkan Backend (Future)                   │  │
│  │  ┌──────────────┐  ┌──────────────┐                 │  │
│  │  │VulkanBuffer  │  │VulkanShader  │                 │  │
│  │  └──────────────┘  └──────────────┘                 │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │          DirectX 12 Backend (Future)                 │  │
│  │  ┌──────────────┐  ┌──────────────┐                 │  │
│  │  │  D3D12Buffer │  │  D3D12Shader │                 │  │
│  │  └──────────────┘  └──────────────┘                 │  │
│  └──────────────────────────────────────────────────────┘  │
└────────────────────────┬────────────────────────────────────┘
                         │
                         │ Calls
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   Graphics API Layer                         │
│       OpenGL        Vulkan        DirectX        Metal       │
└─────────────────────────────────────────────────────────────┘
```

## RHI Device Creation Flow

```
Application
    │
    │ createRHIDevice(GraphicsAPI::OpenGL)
    ▼
Factory Function
    │
    │ switch(api)
    ├─► OpenGL    → return new OpenGLRHIDevice()
    ├─► Vulkan    → return new VulkanRHIDevice() [Future]
    ├─► DirectX12 → return new D3D12RHIDevice()  [Future]
    └─► Metal     → return new MetalRHIDevice()  [Future]
    │
    ▼
std::shared_ptr<IRHIDevice>
    │
    │ device->initialize()
    ▼
Initialized RHI Device
```

## Resource Creation Flow

```
Application
    │
    │ BufferDesc desc = {...};
    │ device->createBuffer(desc)
    ▼
IRHIDevice::createBuffer()
    │
    │ [OpenGL Backend]
    ▼
OpenGLBuffer Constructor
    │
    ├─► glGenBuffers()
    ├─► glBindBuffer()
    ├─► glBufferData()
    └─► glBindBuffer(0)
    │
    ▼
std::shared_ptr<IRHIBuffer>
    │
    │ [Application uses buffer]
    ▼
Buffer Operations
    │
    ├─► updateData() → glBufferSubData()
    ├─► map()        → glMapBuffer()
    └─► unmap()      → glUnmapBuffer()
    │
    │ [Buffer goes out of scope]
    ▼
~OpenGLBuffer()
    │
    └─► glDeleteBuffers()
```

## Rendering Pipeline Flow

```
Application Frame
    │
    ├─► device->setViewport(...)
    ├─► device->setDepthTest(true)
    ├─► device->setCullMode(...)
    │
    ├─► device->clearColor(...)
    ├─► device->clear(...)
    │
    ├─► shaderProgram->bind()
    ├─► shaderProgram->setUniformFloat(...)
    ├─► shaderProgram->setUniformVec3(...)
    │
    ├─► vertexArray->bind()
    ├─► device->drawIndexed(...)
    ├─► vertexArray->unbind()
    │
    └─► shaderProgram->unbind()
```

## Shader Compilation Flow

```
Application
    │
    │ ShaderDesc desc = {...};
    │ device->createShader(desc)
    ▼
OpenGLShader Constructor
    │
    ├─► glCreateShader(type)
    └─► Store source code
    │
    ▼
shader->compile()
    │
    ├─► glShaderSource()
    ├─► glCompileShader()
    ├─► glGetShaderiv(GL_COMPILE_STATUS)
    │
    ├─► Success? → return true
    └─► Failure? → Store error, return false
    │
    ▼
Application checks result
    │
    ├─► if (!compiled) getCompileErrors()
    │
    ▼
Create Shader Program
    │
    ├─► program = device->createShaderProgram()
    ├─► program->attachShader(vertexShader)
    ├─► program->attachShader(fragmentShader)
    │
    ▼
program->link()
    │
    ├─► glAttachShader() for each shader
    ├─► glLinkProgram()
    ├─► glGetProgramiv(GL_LINK_STATUS)
    ├─► glDetachShader() for each shader
    │
    └─► Success/Failure
```

## Class Hierarchy

```
IRHIResource (interface)
    │
    ├─► IRHIBuffer
    │   └─► OpenGLBuffer
    │
    ├─► IRHIShader
    │   └─► OpenGLShader
    │
    ├─► IRHIShaderProgram
    │   └─► OpenGLShaderProgram
    │
    ├─► IRHITexture
    │   └─► OpenGLTexture
    │
    ├─► IRHIFramebuffer
    │   └─► OpenGLFramebuffer
    │
    └─► IRHIVertexArray
        └─► OpenGLVertexArray

IRHIDevice (interface)
    │
    └─► OpenGLRHIDevice
        [Future: VulkanRHIDevice, D3D12RHIDevice, MetalRHIDevice]
```

## Type Conversion Flow (OpenGL)

```
RHI Type                      OpenGL Type
────────────────────────────────────────────
BufferType::Vertex       →   GL_ARRAY_BUFFER
BufferType::Index        →   GL_ELEMENT_ARRAY_BUFFER
BufferType::Uniform      →   GL_UNIFORM_BUFFER

BufferUsage::Static      →   GL_STATIC_DRAW
BufferUsage::Dynamic     →   GL_DYNAMIC_DRAW
BufferUsage::Stream      →   GL_STREAM_DRAW

ShaderType::Vertex       →   GL_VERTEX_SHADER
ShaderType::Fragment     →   GL_FRAGMENT_SHADER
ShaderType::Geometry     →   GL_GEOMETRY_SHADER
ShaderType::Compute      →   GL_COMPUTE_SHADER

TextureFormat::RGBA8     →   GL_RGBA
TextureFormat::RGBA16F   →   GL_RGBA16F
TextureFormat::Depth32F  →   GL_DEPTH_COMPONENT32F

PrimitiveTopology::
  TriangleList           →   GL_TRIANGLES
  TriangleStrip          →   GL_TRIANGLE_STRIP
  LineList               →   GL_LINES

CompareFunc::Less        →   GL_LESS
CompareFunc::LessEqual   →   GL_LEQUAL
CompareFunc::Always      →   GL_ALWAYS
```

## Memory Management

```
Application Creates Resource
    │
    ▼
std::shared_ptr<IRHIBuffer> buffer
    │
    │ ref_count = 1
    │
    ├─► Application stores reference
    │   ref_count = 2
    │
    ├─► Pass to function
    │   ref_count = 3 (temporarily)
    │   Function returns
    │   ref_count = 2
    │
    ├─► Application releases reference
    │   ref_count = 1
    │
    └─► Original shared_ptr goes out of scope
        ref_count = 0
        │
        ▼
    Destructor Called
        │
        ├─► buffer->release()
        │   └─► glDeleteBuffers()
        │
        └─► Memory freed
```

## Extension Point: Adding Vulkan Backend

```
Step 1: Create VulkanRHI.h
    │
    ├─► class VulkanRHIDevice : public IRHIDevice
    ├─► class VulkanBuffer : public IRHIBuffer
    ├─► class VulkanShader : public IRHIShader
    └─► ... (other resources)
    │
    ▼
Step 2: Create VulkanRHI.cpp
    │
    ├─► Implement all virtual methods
    ├─► Add Vulkan-specific code
    │   ├─► vkCreateInstance()
    │   ├─► vkCreateDevice()
    │   └─► vkCreateBuffer(), etc.
    │
    ▼
Step 3: Update Factory
    │
    └─► case GraphicsAPI::Vulkan:
        return std::make_shared<VulkanRHIDevice>();
    │
    ▼
Step 4: Test
    │
    └─► auto device = createRHIDevice(GraphicsAPI::Vulkan);
```

## Benefits Visualization

```
Without RHI:
┌──────────────┐
│ Application  │
└──────┬───────┘
       │ Direct OpenGL calls
       ▼
┌──────────────┐
│   OpenGL     │
└──────────────┘

Problem: Tightly coupled, hard to change API


With RHI:
┌──────────────┐
│ Application  │
└──────┬───────┘
       │ RHI interface calls
       ▼
┌──────────────┐
│  RHI Layer   │ ◄─── Abstraction
└──────┬───────┘
       │
       ├─► OpenGL
       ├─► Vulkan
       ├─► DirectX
       └─► Metal

Benefits:
✓ Loosely coupled
✓ Easy to add new APIs
✓ Testable (can mock)
✓ Single instrumentation point
```
