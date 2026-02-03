#pragma once

#include "RHITypes.h"
#include "RHIResources.h"
#include <memory>
#include <string>

namespace CarrotToy {
namespace RHI {

// RHI Device interface - main entry point for graphics API abstraction
class IRHIDevice {
public:
    virtual ~IRHIDevice() = default;
    
    using ProcAddressLoader = void* (*)(const char* name);

    // Device initialization and shutdown
    virtual bool initialize(ProcAddressLoader loader = nullptr) = 0;
    virtual void shutdown() = 0;
    
    // Get graphics API type
    virtual GraphicsAPI getGraphicsAPI() const = 0;
    
    // Resource creation
    virtual std::shared_ptr<IRHIBuffer> createBuffer(const BufferDesc& desc) = 0;
    virtual std::shared_ptr<IRHIShader> createShader(const ShaderDesc& desc) = 0;
    virtual std::shared_ptr<IRHIShaderProgram> createShaderProgram() = 0;
    virtual std::shared_ptr<IRHITexture> createTexture(const TextureDesc& desc) = 0;
    virtual std::shared_ptr<IRHIFramebuffer> createFramebuffer(const FramebufferDesc& desc) = 0;
    virtual std::shared_ptr<IRHIVertexArray> createVertexArray() = 0;
    // Create a uniform buffer object (size in bytes) and bind it to a binding index
    virtual std::shared_ptr<class IRHIUniformBuffer> createUniformBuffer(size_t size, uint32_t binding) = 0;
    
    // Rendering state
    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    
    virtual void setDepthTest(bool enabled) = 0;
    virtual void setDepthWrite(bool enabled) = 0;
    virtual void setDepthFunc(CompareFunc func) = 0;
    
    virtual void setBlend(bool enabled) = 0;
    virtual void setBlendFunc(BlendFactor srcFactor, BlendFactor dstFactor) = 0;
    virtual void setBlendOp(BlendOp op) = 0;
    
    virtual void setCullMode(CullMode mode) = 0;
    
    // Clearing
    virtual void clearColor(float r, float g, float b, float a) = 0;
    virtual void clearDepth(float depth) = 0;
    virtual void clear(bool color, bool depth, bool stencil) = 0;
    
    // Drawing
    virtual void draw(PrimitiveTopology topology, uint32_t vertexCount, uint32_t startVertex = 0) = 0;
    virtual void drawIndexed(PrimitiveTopology topology, uint32_t indexCount, uint32_t startIndex = 0) = 0;
};

// Factory function to create RHI device based on API type
RHI_API std::shared_ptr<IRHIDevice> createRHIDevice(GraphicsAPI api);

// Global device accessors (convenience for high-level systems)
RHI_API void setGlobalDevice(std::shared_ptr<IRHIDevice> device);
RHI_API std::shared_ptr<IRHIDevice> getGlobalDevice();

} // namespace RHI
} // namespace CarrotToy
