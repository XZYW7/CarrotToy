#pragma once

#include "RHITypes.h"
#include <string>

namespace CarrotToy {
namespace RHI {

// Base class for RHI resources
class IRHIResource {
public:
    virtual ~IRHIResource() = default;
    
    // Resource management
    virtual bool isValid() const = 0;
    virtual void release() = 0;
};

// Buffer interface
class IRHIBuffer : public IRHIResource {
public:
    virtual ~IRHIBuffer() = default;
    
    // Buffer operations
    virtual void updateData(const void* data, size_t size, size_t offset = 0) = 0;
    virtual void* map() = 0;
    virtual void unmap() = 0;
    
    virtual size_t getSize() const = 0;
    virtual BufferType getType() const = 0;
};

// Shader interface
class IRHIShader : public IRHIResource {
public:
    virtual ~IRHIShader() = default;
    
    // Shader operations
    virtual bool compile() = 0;
    virtual std::string getCompileErrors() const = 0;
    
    virtual ShaderType getType() const = 0;
};

// Shader program interface (collection of shaders)
class IRHIShaderProgram : public IRHIResource {
public:
    virtual ~IRHIShaderProgram() = default;
    
    // Program operations
    virtual bool link() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    
    // Uniform setters
    virtual void setUniformFloat(const std::string& name, float value) = 0;
    virtual void setUniformVec2(const std::string& name, float x, float y) = 0;
    virtual void setUniformVec3(const std::string& name, float x, float y, float z) = 0;
    virtual void setUniformVec4(const std::string& name, float x, float y, float z, float w) = 0;
    virtual void setUniformInt(const std::string& name, int value) = 0;
    virtual void setUniformBool(const std::string& name, bool value) = 0;
    virtual void setUniformMatrix4(const std::string& name, const float* value) = 0;
    
    virtual std::string getLinkErrors() const = 0;
};

// Texture interface
class IRHITexture : public IRHIResource {
public:
    virtual ~IRHITexture() = default;
    
    // Texture operations
    virtual void updateData(const void* data, uint32_t width, uint32_t height) = 0;
    virtual void bind(uint32_t slot = 0) = 0;
    virtual void unbind() = 0;
    
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual TextureFormat getFormat() const = 0;
};

// Framebuffer interface
class IRHIFramebuffer : public IRHIResource {
public:
    virtual ~IRHIFramebuffer() = default;
    
    // Framebuffer operations
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void attachColorTexture(IRHITexture* texture, uint32_t attachment = 0) = 0;
    virtual void attachDepthTexture(IRHITexture* texture) = 0;
    virtual bool isComplete() = 0;
    
    virtual IRHITexture* getColorTexture(uint32_t attachment = 0) = 0;
    virtual IRHITexture* getDepthTexture() = 0;
};

// Vertex array/input layout interface
class IRHIVertexArray : public IRHIResource {
public:
    virtual ~IRHIVertexArray() = default;
    
    // Vertex array operations
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void setVertexBuffer(IRHIBuffer* buffer, uint32_t binding = 0) = 0;
    virtual void setIndexBuffer(IRHIBuffer* buffer) = 0;
    virtual void setVertexAttribute(const VertexAttribute& attribute) = 0;
};

} // namespace RHI
} // namespace CarrotToy
