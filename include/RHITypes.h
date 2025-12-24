#pragma once

#include <cstdint>

namespace CarrotToy {
namespace RHI {

// Forward declarations
class IRHIDevice;
class IRHIBuffer;
class IRHIShader;
class IRHITexture;
class IRHIFramebuffer;

// Buffer types
enum class BufferType {
    Vertex,
    Index,
    Uniform
};

// Buffer usage hints
enum class BufferUsage {
    Static,    // Data rarely changes
    Dynamic,   // Data changes occasionally
    Stream     // Data changes every frame
};

// Shader types
enum class ShaderType {
    Vertex,
    Fragment,
    Geometry,
    Compute
};

// Texture formats
enum class TextureFormat {
    RGB8,
    RGBA8,
    RGBA16F,
    RGBA32F,
    Depth24Stencil8,
    Depth32F
};

// Texture filtering
enum class TextureFilter {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapLinear
};

// Texture wrapping
enum class TextureWrap {
    Repeat,
    ClampToEdge,
    MirroredRepeat
};

// Primitive topology
enum class PrimitiveTopology {
    TriangleList,
    TriangleStrip,
    LineList,
    LineStrip,
    PointList
};

// Comparison functions
enum class CompareFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

// Blend factors
enum class BlendFactor {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha
};

// Blend operations
enum class BlendOp {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max
};

// Cull mode
enum class CullMode {
    None,
    Front,
    Back
};

// Graphics API types
enum class GraphicsAPI {
    OpenGL,
    Vulkan,
    DirectX11,
    DirectX12,
    Metal
};

// Vertex attribute data
struct VertexAttribute {
    uint32_t location;
    uint32_t binding;
    uint32_t offset;
    uint32_t componentCount;  // 1, 2, 3, or 4
    uint32_t stride;          // Byte offset between consecutive vertices (0 = automatically calculated by OpenGL)
    bool normalized;
};

// Buffer descriptor
struct BufferDesc {
    BufferType type;
    BufferUsage usage;
    size_t size;
    const void* initialData;
    
    BufferDesc() 
        : type(BufferType::Vertex)
        , usage(BufferUsage::Static)
        , size(0)
        , initialData(nullptr) {}
};

// Texture descriptor
struct TextureDesc {
    uint32_t width;
    uint32_t height;
    TextureFormat format;
    TextureFilter minFilter;
    TextureFilter magFilter;
    TextureWrap wrapS;
    TextureWrap wrapT;
    bool generateMipmaps;
    const void* initialData;
    
    TextureDesc()
        : width(0)
        , height(0)
        , format(TextureFormat::RGBA8)
        , minFilter(TextureFilter::Linear)
        , magFilter(TextureFilter::Linear)
        , wrapS(TextureWrap::Repeat)
        , wrapT(TextureWrap::Repeat)
        , generateMipmaps(false)
        , initialData(nullptr) {}
};

// Shader descriptor
struct ShaderDesc {
    ShaderType type;
    const char* source;
    size_t sourceSize;
    
    ShaderDesc()
        : type(ShaderType::Vertex)
        , source(nullptr)
        , sourceSize(0) {}
};

// Framebuffer descriptor
struct FramebufferDesc {
    uint32_t width;
    uint32_t height;
    bool hasDepthStencil;
    
    FramebufferDesc()
        : width(0)
        , height(0)
        , hasDepthStencil(true) {}
};

} // namespace RHI
} // namespace CarrotToy
