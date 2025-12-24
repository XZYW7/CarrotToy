#pragma once

#include "RHI.h"
#include "RHIResources.h"
#include <vector>
#include <unordered_map>

namespace CarrotToy {
namespace RHI {

// Forward declarations
class OpenGLBuffer;
class OpenGLShader;
class OpenGLShaderProgram;
class OpenGLTexture;
class OpenGLFramebuffer;
class OpenGLVertexArray;

// OpenGL Buffer implementation
class OpenGLBuffer : public IRHIBuffer {
public:
    OpenGLBuffer(const BufferDesc& desc);
    ~OpenGLBuffer() override;
    
    void updateData(const void* data, size_t size, size_t offset = 0) override;
    void* map() override;
    void unmap() override;
    
    bool isValid() const override { return bufferID != 0; }
    void release() override;
    
    size_t getSize() const override { return size; }
    BufferType getType() const override { return type; }
    
    unsigned int getBufferID() const { return bufferID; }
    
private:
    unsigned int bufferID;
    BufferType type;
    BufferUsage usage;
    size_t size;
};

// OpenGL Shader implementation
class OpenGLShader : public IRHIShader {
public:
    OpenGLShader(const ShaderDesc& desc);
    ~OpenGLShader() override;
    
    bool compile() override;
    std::string getCompileErrors() const override { return errors; }
    
    bool isValid() const override { return shaderID != 0; }
    void release() override;
    
    ShaderType getType() const override { return type; }
    unsigned int getShaderID() const { return shaderID; }
    
private:
    unsigned int shaderID;
    ShaderType type;
    std::string source;
    std::string errors;
};

// OpenGL Shader Program implementation
class OpenGLShaderProgram : public IRHIShaderProgram {
public:
    OpenGLShaderProgram();
    ~OpenGLShaderProgram() override;
    
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
    
    bool isValid() const override { return programID != 0; }
    void release() override;
    
    void attachShader(IRHIShader* shader);
    void detachShader(IRHIShader* shader);
    
private:
    unsigned int programID;
    std::string errors;
    std::vector<unsigned int> attachedShaders;
    
    int getUniformLocation(const std::string& name);
    std::unordered_map<std::string, int> uniformLocationCache;
};

// OpenGL Texture implementation
class OpenGLTexture : public IRHITexture {
public:
    OpenGLTexture(const TextureDesc& desc);
    ~OpenGLTexture() override;
    
    void updateData(const void* data, uint32_t width, uint32_t height) override;
    void bind(uint32_t slot = 0) override;
    void unbind() override;
    
    bool isValid() const override { return textureID != 0; }
    void release() override;
    
    uint32_t getWidth() const override { return width; }
    uint32_t getHeight() const override { return height; }
    TextureFormat getFormat() const override { return format; }
    
    unsigned int getTextureID() const { return textureID; }
    
private:
    unsigned int textureID;
    uint32_t width;
    uint32_t height;
    TextureFormat format;
    TextureFilter minFilter;
    TextureFilter magFilter;
    TextureWrap wrapS;
    TextureWrap wrapT;
};

// OpenGL Framebuffer implementation
class OpenGLFramebuffer : public IRHIFramebuffer {
public:
    OpenGLFramebuffer(const FramebufferDesc& desc);
    ~OpenGLFramebuffer() override;
    
    void bind() override;
    void unbind() override;
    void attachColorTexture(IRHITexture* texture, uint32_t attachment = 0) override;
    void attachDepthTexture(IRHITexture* texture) override;
    bool isComplete() override;
    
    bool isValid() const override { return framebufferID != 0; }
    void release() override;
    
    IRHITexture* getColorTexture(uint32_t attachment = 0) override;
    IRHITexture* getDepthTexture() override;
    
    unsigned int getFramebufferID() const { return framebufferID; }
    
private:
    unsigned int framebufferID;
    std::vector<IRHITexture*> colorTextures;
    IRHITexture* depthTexture;
    uint32_t width;
    uint32_t height;
};

// OpenGL Vertex Array implementation
class OpenGLVertexArray : public IRHIVertexArray {
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray() override;
    
    void bind() override;
    void unbind() override;
    void setVertexBuffer(IRHIBuffer* buffer, uint32_t binding = 0) override;
    void setIndexBuffer(IRHIBuffer* buffer) override;
    void setVertexAttribute(const VertexAttribute& attribute) override;
    
    bool isValid() const override { return vaoID != 0; }
    void release() override;
    
    unsigned int getVAOID() const { return vaoID; }
    
private:
    unsigned int vaoID;
    std::vector<IRHIBuffer*> vertexBuffers;
    IRHIBuffer* indexBuffer;
};

// OpenGL RHI Device implementation
class OpenGLRHIDevice : public IRHIDevice {
public:
    OpenGLRHIDevice();
    ~OpenGLRHIDevice() override;
    
    bool initialize() override;
    void shutdown() override;
    
    GraphicsAPI getGraphicsAPI() const override { return GraphicsAPI::OpenGL; }
    
    // Resource creation
    std::shared_ptr<IRHIBuffer> createBuffer(const BufferDesc& desc) override;
    std::shared_ptr<IRHIShader> createShader(const ShaderDesc& desc) override;
    std::shared_ptr<IRHIShaderProgram> createShaderProgram() override;
    std::shared_ptr<IRHITexture> createTexture(const TextureDesc& desc) override;
    std::shared_ptr<IRHIFramebuffer> createFramebuffer(const FramebufferDesc& desc) override;
    std::shared_ptr<IRHIVertexArray> createVertexArray() override;
    
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
    
private:
    bool initialized;
};

} // namespace RHI
} // namespace CarrotToy
