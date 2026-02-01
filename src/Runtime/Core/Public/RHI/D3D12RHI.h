#pragma once

#include "RHI.h"
#include "RHIResources.h"
#include <vector>
#include <unordered_map>

// Forward declare D3D12 types to avoid including d3d12.h in header
struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Resource;
struct ID3D12RootSignature;
struct ID3D12PipelineState;
struct ID3D12DescriptorHeap;
struct ID3D12Fence;
struct IDXGISwapChain3;
struct IDXGIFactory4;
struct IDXGIAdapter1;

namespace CarrotToy {
namespace RHI {

// Forward declarations
class D3D12Buffer;
class D3D12Shader;
class D3D12ShaderProgram;
class D3D12Texture;
class D3D12Framebuffer;
class D3D12VertexArray;
class D3D12UniformBuffer;

// D3D12 Buffer implementation
class D3D12Buffer : public IRHIBuffer {
public:
    D3D12Buffer(ID3D12Device* device, const BufferDesc& desc);
    ~D3D12Buffer() override;
    
    void updateData(const void* data, size_t size, size_t offset = 0) override;
    void* map() override;
    void unmap() override;
    
    bool isValid() const override { return resource != nullptr; }
    void release() override;
    
    size_t getSize() const override { return size; }
    BufferType getType() const override { return type; }
    
    ID3D12Resource* getResource() const { return resource; }
    
private:
    ID3D12Device* device;
    ID3D12Resource* resource;
    BufferType type;
    BufferUsage usage;
    size_t size;
    void* mappedData;
};

// D3D12 Shader implementation
class D3D12Shader : public IRHIShader {
public:
    D3D12Shader(ID3D12Device* device, const ShaderDesc& desc);
    ~D3D12Shader() override;
    
    bool compile() override;
    std::string getCompileErrors() const override { return errors; }
    
    bool isValid() const override { return !bytecode.empty(); }
    void release() override;
    
    ShaderType getType() const override { return type; }
    const std::vector<uint8_t>& getBytecode() const { return bytecode; }
    
private:
    ID3D12Device* device;
    ShaderType type;
    std::vector<uint8_t> source;
    std::vector<uint8_t> bytecode;
    std::string errors;
};

// D3D12 Shader Program implementation (Pipeline State Object)
class D3D12ShaderProgram : public IRHIShaderProgram {
public:
    D3D12ShaderProgram(ID3D12Device* device);
    ~D3D12ShaderProgram() override;
    
    void attachShader(IRHIShader* shader) override;
    void detachShader(IRHIShader* shader) override;
    
    bool link() override;
    void bind() override;
    void unbind() override;
    
    void setUniformFloat(const std::string& name, float value) override;
    void setUniformVec2(const std::string& name, float x, float y) override;
    void setUniformVec3(const std::string& name, float x, float y, float z) override;
    void setUniformVec4(const std::string& name, float x, float y, float z, float w) override;
    void setUniformInt(const std::string& name, int value) override;
    void setUniformBool(const std::string& name, bool value) override;
    void setUniformMatrix4(const std::string& name, const float* value) override;
    
    std::string getLinkErrors() const override { return errors; }
    
    bool isValid() const override { return pipelineState != nullptr; }
    void release() override;
    
    ID3D12PipelineState* getPipelineState() const { return pipelineState; }
    ID3D12RootSignature* getRootSignature() const { return rootSignature; }
    
private:
    ID3D12Device* device;
    ID3D12PipelineState* pipelineState;
    ID3D12RootSignature* rootSignature;
    ID3D12Resource* constantBuffer;
    std::string errors;
    std::vector<D3D12Shader*> attachedShaders;
    std::unordered_map<std::string, size_t> uniformOffsets;
    std::vector<uint8_t> constantBufferData;
};

// D3D12 Texture implementation
class D3D12Texture : public IRHITexture {
public:
    D3D12Texture(ID3D12Device* device, const TextureDesc& desc);
    ~D3D12Texture() override;
    
    void updateData(const void* data, uint32_t width, uint32_t height) override;
    void bind(uint32_t slot = 0) override;
    void unbind() override;
    
    bool isValid() const override { return resource != nullptr; }
    void release() override;
    
    uint32_t getWidth() const override { return width; }
    uint32_t getHeight() const override { return height; }
    TextureFormat getFormat() const override { return format; }
    
    ID3D12Resource* getResource() const { return resource; }
    
private:
    ID3D12Device* device;
    ID3D12Resource* resource;
    ID3D12DescriptorHeap* srvHeap;
    uint32_t width;
    uint32_t height;
    TextureFormat format;
};

// D3D12 Framebuffer implementation (Render Target)
class D3D12Framebuffer : public IRHIFramebuffer {
public:
    D3D12Framebuffer(ID3D12Device* device, const FramebufferDesc& desc);
    ~D3D12Framebuffer() override;
    
    void bind() override;
    void unbind() override;
    void attachColorTexture(IRHITexture* texture, uint32_t attachment = 0) override;
    void attachDepthTexture(IRHITexture* texture) override;
    bool isComplete() override;
    
    bool isValid() const override { return true; }
    void release() override;
    
    IRHITexture* getColorTexture(uint32_t attachment = 0) override;
    IRHITexture* getDepthTexture() override;
    
    ID3D12DescriptorHeap* getRTVHeap() const { return rtvHeap; }
    ID3D12DescriptorHeap* getDSVHeap() const { return dsvHeap; }
    
private:
    ID3D12Device* device;
    ID3D12DescriptorHeap* rtvHeap;
    ID3D12DescriptorHeap* dsvHeap;
    std::vector<std::shared_ptr<IRHITexture>> colorTextures;
    std::shared_ptr<IRHITexture> depthTexture;
    uint32_t width;
    uint32_t height;
};

// D3D12 Vertex Array implementation (Input Layout)
class D3D12VertexArray : public IRHIVertexArray {
public:
    D3D12VertexArray(ID3D12Device* device);
    ~D3D12VertexArray() override;
    
    void bind() override;
    void unbind() override;
    void setVertexBuffer(IRHIBuffer* buffer, uint32_t binding = 0) override;
    void setIndexBuffer(IRHIBuffer* buffer) override;
    void setVertexAttribute(const VertexAttribute& attribute) override;
    
    bool isValid() const override { return true; }
    void release() override;
    
    const std::vector<IRHIBuffer*>& getVertexBuffers() const { return vertexBuffers; }
    IRHIBuffer* getIndexBuffer() const { return indexBuffer; }
    const std::vector<VertexAttribute>& getAttributes() const { return attributes; }
    
private:
    ID3D12Device* device;
    std::vector<IRHIBuffer*> vertexBuffers;
    IRHIBuffer* indexBuffer;
    std::vector<VertexAttribute> attributes;
};

// D3D12 Uniform Buffer implementation (Constant Buffer)
class D3D12UniformBuffer : public IRHIUniformBuffer {
public:
    D3D12UniformBuffer(ID3D12Device* device, size_t size, uint32_t binding);
    ~D3D12UniformBuffer() override;
    
    void update(const void* data, size_t size, size_t offset = 0) override;
    void bind(uint32_t binding) override;
    
    size_t getSize() const override { return sizeBytes; }
    bool isValid() const override { return resource != nullptr; }
    void release() override;
    
    uintptr_t getNativeHandle() const override { return (uintptr_t)resource; }
    ID3D12Resource* getResource() const { return resource; }
    
private:
    ID3D12Device* device;
    ID3D12Resource* resource;
    size_t sizeBytes;
    uint32_t binding;
    void* mappedData;
};

// D3D12 RHI Device implementation
class D3D12RHIDevice : public IRHIDevice {
public:
    D3D12RHIDevice();
    ~D3D12RHIDevice() override;
    
    bool initialize() override;
    void shutdown() override;
    
    GraphicsAPI getGraphicsAPI() const override { return GraphicsAPI::DirectX12; }
    
    // Resource creation
    std::shared_ptr<IRHIBuffer> createBuffer(const BufferDesc& desc) override;
    std::shared_ptr<IRHIShader> createShader(const ShaderDesc& desc) override;
    std::shared_ptr<IRHIShaderProgram> createShaderProgram() override;
    std::shared_ptr<IRHITexture> createTexture(const TextureDesc& desc) override;
    std::shared_ptr<IRHIFramebuffer> createFramebuffer(const FramebufferDesc& desc) override;
    std::shared_ptr<IRHIVertexArray> createVertexArray() override;
    std::shared_ptr<IRHIUniformBuffer> createUniformBuffer(size_t size, uint32_t binding) override;
    
    // Rendering state
    void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    
    void setDepthTest(bool enabled) override;
    void setDepthWrite(bool enabled) override;
    void setDepthFunc(CompareFunc func) override;
    
    void setBlend(bool enabled) override;
    void setBlendFunc(BlendFactor srcFactor, BlendFactor dstFactor) override;
    void setBlendOp(BlendOp op) override;
    
    void setCullMode(CullMode mode) override;
    
    // Clearing
    void clearColor(float r, float g, float b, float a) override;
    void clearDepth(float depth) override;
    void clear(bool color, bool depth, bool stencil) override;
    
    // Drawing
    void draw(PrimitiveTopology topology, uint32_t vertexCount, uint32_t startVertex = 0) override;
    void drawIndexed(PrimitiveTopology topology, uint32_t indexCount, uint32_t startIndex = 0) override;
    
    // D3D12-specific accessors
    ID3D12Device* getDevice() const { return device; }
    ID3D12CommandQueue* getCommandQueue() const { return commandQueue; }
    ID3D12GraphicsCommandList* getCommandList() const { return commandList; }
    
private:
    bool initialized;
    ID3D12Device* device;
    ID3D12CommandQueue* commandQueue;
    ID3D12CommandAllocator* commandAllocator;
    ID3D12GraphicsCommandList* commandList;
    ID3D12Fence* fence;
    UINT64 fenceValue;
    HANDLE fenceEvent;
    
    // State tracking
    struct RenderState {
        uint32_t viewportX, viewportY, viewportWidth, viewportHeight;
        uint32_t scissorX, scissorY, scissorWidth, scissorHeight;
        bool depthTestEnabled;
        bool depthWriteEnabled;
        CompareFunc depthFunc;
        bool blendEnabled;
        BlendFactor srcBlendFactor, dstBlendFactor;
        BlendOp blendOp;
        CullMode cullMode;
        float clearColorR, clearColorG, clearColorB, clearColorA;
        float clearDepth;
    } state;
};

} // namespace RHI
} // namespace CarrotToy
