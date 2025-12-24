#include "OpenGLRHI.h"
#include <glad/glad.h>
#include <iostream>
#include <cstring>

namespace CarrotToy {
namespace RHI {

// Helper function to convert RHI types to OpenGL types
static unsigned int toGLBufferType(BufferType type) {
    switch (type) {
        case BufferType::Vertex:  return GL_ARRAY_BUFFER;
        case BufferType::Index:   return GL_ELEMENT_ARRAY_BUFFER;
        case BufferType::Uniform: return GL_UNIFORM_BUFFER;
        default: return GL_ARRAY_BUFFER;
    }
}

static unsigned int toGLBufferUsage(BufferUsage usage) {
    switch (usage) {
        case BufferUsage::Static:  return GL_STATIC_DRAW;
        case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        case BufferUsage::Stream:  return GL_STREAM_DRAW;
        default: return GL_STATIC_DRAW;
    }
}

static unsigned int toGLShaderType(ShaderType type) {
    switch (type) {
        case ShaderType::Vertex:   return GL_VERTEX_SHADER;
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
        case ShaderType::Compute:  return GL_COMPUTE_SHADER;
        default: return GL_VERTEX_SHADER;
    }
}

static unsigned int toGLTextureFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::RGB8:           return GL_RGB;
        case TextureFormat::RGBA8:          return GL_RGBA;
        case TextureFormat::RGBA16F:        return GL_RGBA16F;
        case TextureFormat::RGBA32F:        return GL_RGBA32F;
        case TextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
        case TextureFormat::Depth32F:       return GL_DEPTH_COMPONENT32F;
        default: return GL_RGBA;
    }
}

static unsigned int toGLTextureFilter(TextureFilter filter) {
    switch (filter) {
        case TextureFilter::Nearest:              return GL_NEAREST;
        case TextureFilter::Linear:               return GL_LINEAR;
        case TextureFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilter::LinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
        default: return GL_LINEAR;
    }
}

static unsigned int toGLTextureWrap(TextureWrap wrap) {
    switch (wrap) {
        case TextureWrap::Repeat:         return GL_REPEAT;
        case TextureWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
        case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
        default: return GL_REPEAT;
    }
}

static unsigned int toGLCompareFunc(CompareFunc func) {
    switch (func) {
        case CompareFunc::Never:        return GL_NEVER;
        case CompareFunc::Less:         return GL_LESS;
        case CompareFunc::Equal:        return GL_EQUAL;
        case CompareFunc::LessEqual:    return GL_LEQUAL;
        case CompareFunc::Greater:      return GL_GREATER;
        case CompareFunc::NotEqual:     return GL_NOTEQUAL;
        case CompareFunc::GreaterEqual: return GL_GEQUAL;
        case CompareFunc::Always:       return GL_ALWAYS;
        default: return GL_LESS;
    }
}

static unsigned int toGLBlendFactor(BlendFactor factor) {
    switch (factor) {
        case BlendFactor::Zero:              return GL_ZERO;
        case BlendFactor::One:               return GL_ONE;
        case BlendFactor::SrcColor:          return GL_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:  return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:          return GL_DST_COLOR;
        case BlendFactor::OneMinusDstColor:  return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:          return GL_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:  return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:          return GL_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:  return GL_ONE_MINUS_DST_ALPHA;
        default: return GL_ONE;
    }
}

static unsigned int toGLBlendOp(BlendOp op) {
    switch (op) {
        case BlendOp::Add:             return GL_FUNC_ADD;
        case BlendOp::Subtract:        return GL_FUNC_SUBTRACT;
        case BlendOp::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendOp::Min:             return GL_MIN;
        case BlendOp::Max:             return GL_MAX;
        default: return GL_FUNC_ADD;
    }
}

static unsigned int toGLPrimitiveTopology(PrimitiveTopology topology) {
    switch (topology) {
        case PrimitiveTopology::TriangleList:  return GL_TRIANGLES;
        case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
        case PrimitiveTopology::LineList:      return GL_LINES;
        case PrimitiveTopology::LineStrip:     return GL_LINE_STRIP;
        case PrimitiveTopology::PointList:     return GL_POINTS;
        default: return GL_TRIANGLES;
    }
}

// OpenGLBuffer implementation
OpenGLBuffer::OpenGLBuffer(const BufferDesc& desc)
    : bufferID(0), type(desc.type), usage(desc.usage), size(desc.size) {
    glGenBuffers(1, &bufferID);
    glBindBuffer(toGLBufferType(type), bufferID);
    glBufferData(toGLBufferType(type), desc.size, desc.initialData, toGLBufferUsage(usage));
    glBindBuffer(toGLBufferType(type), 0);
}

OpenGLBuffer::~OpenGLBuffer() {
    release();
}

void OpenGLBuffer::updateData(const void* data, size_t updateSize, size_t offset) {
    glBindBuffer(toGLBufferType(type), bufferID);
    glBufferSubData(toGLBufferType(type), offset, updateSize, data);
    glBindBuffer(toGLBufferType(type), 0);
}

void* OpenGLBuffer::map() {
    glBindBuffer(toGLBufferType(type), bufferID);
    void* ptr = glMapBuffer(toGLBufferType(type), GL_READ_WRITE);
    return ptr;
}

void OpenGLBuffer::unmap() {
    glUnmapBuffer(toGLBufferType(type));
    glBindBuffer(toGLBufferType(type), 0);
}

void OpenGLBuffer::release() {
    if (bufferID != 0) {
        glDeleteBuffers(1, &bufferID);
        bufferID = 0;
    }
}

// OpenGLShader implementation
OpenGLShader::OpenGLShader(const ShaderDesc& desc)
    : shaderID(0), type(desc.type), source(desc.source, desc.sourceSize) {
    shaderID = glCreateShader(toGLShaderType(type));
}

OpenGLShader::~OpenGLShader() {
    release();
}

bool OpenGLShader::compile() {
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);
    
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        errors = infoLog;
        return false;
    }
    
    errors.clear();
    return true;
}

void OpenGLShader::release() {
    if (shaderID != 0) {
        glDeleteShader(shaderID);
        shaderID = 0;
    }
}

// OpenGLShaderProgram implementation
OpenGLShaderProgram::OpenGLShaderProgram()
    : programID(0) {
    programID = glCreateProgram();
}

OpenGLShaderProgram::~OpenGLShaderProgram() {
    release();
}

void OpenGLShaderProgram::attachShader(IRHIShader* shader) {
    if (auto* glShader = dynamic_cast<OpenGLShader*>(shader)) {
        glAttachShader(programID, glShader->getShaderID());
        attachedShaders.push_back(glShader->getShaderID());
    }
}

void OpenGLShaderProgram::detachShader(IRHIShader* shader) {
    if (auto* glShader = dynamic_cast<OpenGLShader*>(shader)) {
        glDetachShader(programID, glShader->getShaderID());
    }
}

bool OpenGLShaderProgram::link() {
    glLinkProgram(programID);
    
    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        errors = infoLog;
        return false;
    }
    
    // Detach shaders after linking
    for (unsigned int shaderID : attachedShaders) {
        glDetachShader(programID, shaderID);
    }
    attachedShaders.clear();
    
    errors.clear();
    return true;
}

void OpenGLShaderProgram::bind() {
    glUseProgram(programID);
}

void OpenGLShaderProgram::unbind() {
    glUseProgram(0);
}

int OpenGLShaderProgram::getUniformLocation(const std::string& name) {
    auto it = uniformLocationCache.find(name);
    if (it != uniformLocationCache.end()) {
        return it->second;
    }
    
    int location = glGetUniformLocation(programID, name.c_str());
    uniformLocationCache[name] = location;
    return location;
}

void OpenGLShaderProgram::setUniformFloat(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void OpenGLShaderProgram::setUniformVec2(const std::string& name, float x, float y) {
    glUniform2f(getUniformLocation(name), x, y);
}

void OpenGLShaderProgram::setUniformVec3(const std::string& name, float x, float y, float z) {
    glUniform3f(getUniformLocation(name), x, y, z);
}

void OpenGLShaderProgram::setUniformVec4(const std::string& name, float x, float y, float z, float w) {
    glUniform4f(getUniformLocation(name), x, y, z, w);
}

void OpenGLShaderProgram::setUniformInt(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void OpenGLShaderProgram::setUniformBool(const std::string& name, bool value) {
    glUniform1i(getUniformLocation(name), value ? 1 : 0);
}

void OpenGLShaderProgram::setUniformMatrix4(const std::string& name, const float* value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value);
}

void OpenGLShaderProgram::release() {
    if (programID != 0) {
        glDeleteProgram(programID);
        programID = 0;
    }
}

// OpenGLTexture implementation
OpenGLTexture::OpenGLTexture(const TextureDesc& desc)
    : textureID(0), width(desc.width), height(desc.height), format(desc.format),
      minFilter(desc.minFilter), magFilter(desc.magFilter), wrapS(desc.wrapS), wrapT(desc.wrapT) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    unsigned int glFormat = toGLTextureFormat(format);
    unsigned int internalFormat = glFormat;
    unsigned int dataType = GL_UNSIGNED_BYTE;
    
    // Determine internal format and data type based on format
    if (format == TextureFormat::RGBA16F || format == TextureFormat::RGBA32F) {
        dataType = GL_FLOAT;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, glFormat, dataType, desc.initialData);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLTextureFilter(minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLTextureFilter(magFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLTextureWrap(wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLTextureWrap(wrapT));
    
    if (desc.generateMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture::~OpenGLTexture() {
    release();
}

void OpenGLTexture::updateData(const void* data, uint32_t newWidth, uint32_t newHeight) {
    width = newWidth;
    height = newHeight;
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    unsigned int glFormat = toGLTextureFormat(format);
    unsigned int dataType = GL_UNSIGNED_BYTE;
    
    if (format == TextureFormat::RGBA16F || format == TextureFormat::RGBA32F) {
        dataType = GL_FLOAT;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, dataType, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTexture::bind(uint32_t slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void OpenGLTexture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTexture::release() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}

// OpenGLFramebuffer implementation
OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDesc& desc)
    : framebufferID(0), depthTexture(nullptr), width(desc.width), height(desc.height) {
    glGenFramebuffers(1, &framebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
    
    // Create default color texture
    TextureDesc colorDesc;
    colorDesc.width = width;
    colorDesc.height = height;
    colorDesc.format = TextureFormat::RGBA8;
    auto colorTex = std::make_shared<OpenGLTexture>(colorDesc);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex->getTextureID(), 0);
    colorTextures.push_back(colorTex);
    
    // Create depth texture if requested
    if (desc.hasDepthStencil) {
        TextureDesc depthDesc;
        depthDesc.width = width;
        depthDesc.height = height;
        depthDesc.format = TextureFormat::Depth24Stencil8;
        auto depthTex = std::make_shared<OpenGLTexture>(depthDesc);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTex->getTextureID(), 0);
        depthTexture = depthTex;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLFramebuffer::~OpenGLFramebuffer() {
    release();
}

void OpenGLFramebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
}

void OpenGLFramebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::attachColorTexture(IRHITexture* texture, uint32_t attachment) {
    if (auto* glTexture = dynamic_cast<OpenGLTexture*>(texture)) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, glTexture->getTextureID(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // Note: Caller must keep texture alive - we only store for default textures created in constructor
    }
}

void OpenGLFramebuffer::attachDepthTexture(IRHITexture* texture) {
    if (auto* glTexture = dynamic_cast<OpenGLTexture*>(texture)) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glTexture->getTextureID(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // Note: Caller must keep texture alive - we only store for default textures created in constructor
    }
}

bool OpenGLFramebuffer::isComplete() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
    bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return complete;
}

IRHITexture* OpenGLFramebuffer::getColorTexture(uint32_t attachment) {
    if (attachment < colorTextures.size()) {
        return colorTextures[attachment].get();
    }
    return nullptr;
}

IRHITexture* OpenGLFramebuffer::getDepthTexture() {
    return depthTexture.get();
}

void OpenGLFramebuffer::release() {
    if (framebufferID != 0) {
        glDeleteFramebuffers(1, &framebufferID);
        framebufferID = 0;
    }
}

// OpenGLVertexArray implementation
OpenGLVertexArray::OpenGLVertexArray()
    : vaoID(0), indexBuffer(nullptr) {
    glGenVertexArrays(1, &vaoID);
}

OpenGLVertexArray::~OpenGLVertexArray() {
    release();
}

void OpenGLVertexArray::bind() {
    glBindVertexArray(vaoID);
}

void OpenGLVertexArray::unbind() {
    glBindVertexArray(0);
}

void OpenGLVertexArray::setVertexBuffer(IRHIBuffer* buffer, uint32_t binding) {
    if (auto* glBuffer = dynamic_cast<OpenGLBuffer*>(buffer)) {
        glBindVertexArray(vaoID);
        glBindBuffer(GL_ARRAY_BUFFER, glBuffer->getBufferID());
        glBindVertexArray(0);
        
        if (binding >= vertexBuffers.size()) {
            vertexBuffers.resize(binding + 1, nullptr);
        }
        vertexBuffers[binding] = buffer;
    }
}

void OpenGLVertexArray::setIndexBuffer(IRHIBuffer* buffer) {
    if (auto* glBuffer = dynamic_cast<OpenGLBuffer*>(buffer)) {
        glBindVertexArray(vaoID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer->getBufferID());
        glBindVertexArray(0);
        indexBuffer = buffer;
    }
}

void OpenGLVertexArray::setVertexAttribute(const VertexAttribute& attribute) {
    glBindVertexArray(vaoID);
    glEnableVertexAttribArray(attribute.location);
    glVertexAttribPointer(
        attribute.location,
        attribute.componentCount,
        GL_FLOAT,
        attribute.normalized ? GL_TRUE : GL_FALSE,
        attribute.stride,  // Use stride from attribute
        (void*)(uintptr_t)attribute.offset
    );
    glBindVertexArray(0);
}

void OpenGLVertexArray::release() {
    if (vaoID != 0) {
        glDeleteVertexArrays(1, &vaoID);
        vaoID = 0;
    }
}

// OpenGLRHIDevice implementation
OpenGLRHIDevice::OpenGLRHIDevice()
    : initialized(false) {
}

OpenGLRHIDevice::~OpenGLRHIDevice() {
    shutdown();
}

bool OpenGLRHIDevice::initialize() {
    // OpenGL context should already be initialized by GLFW/GLAD
    // This is just a validation check
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize OpenGL RHI" << std::endl;
        return false;
    }
    
    initialized = true;
    return true;
}

void OpenGLRHIDevice::shutdown() {
    initialized = false;
}

std::shared_ptr<IRHIBuffer> OpenGLRHIDevice::createBuffer(const BufferDesc& desc) {
    return std::make_shared<OpenGLBuffer>(desc);
}

std::shared_ptr<IRHIShader> OpenGLRHIDevice::createShader(const ShaderDesc& desc) {
    return std::make_shared<OpenGLShader>(desc);
}

std::shared_ptr<IRHIShaderProgram> OpenGLRHIDevice::createShaderProgram() {
    return std::make_shared<OpenGLShaderProgram>();
}

std::shared_ptr<IRHITexture> OpenGLRHIDevice::createTexture(const TextureDesc& desc) {
    return std::make_shared<OpenGLTexture>(desc);
}

std::shared_ptr<IRHIFramebuffer> OpenGLRHIDevice::createFramebuffer(const FramebufferDesc& desc) {
    return std::make_shared<OpenGLFramebuffer>(desc);
}

std::shared_ptr<IRHIVertexArray> OpenGLRHIDevice::createVertexArray() {
    return std::make_shared<OpenGLVertexArray>();
}

void OpenGLRHIDevice::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    glViewport(x, y, width, height);
}

void OpenGLRHIDevice::setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    glScissor(x, y, width, height);
}

void OpenGLRHIDevice::setDepthTest(bool enabled) {
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void OpenGLRHIDevice::setDepthWrite(bool enabled) {
    glDepthMask(enabled ? GL_TRUE : GL_FALSE);
}

void OpenGLRHIDevice::setDepthFunc(CompareFunc func) {
    glDepthFunc(toGLCompareFunc(func));
}

void OpenGLRHIDevice::setBlend(bool enabled) {
    if (enabled) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}

void OpenGLRHIDevice::setBlendFunc(BlendFactor srcFactor, BlendFactor dstFactor) {
    glBlendFunc(toGLBlendFactor(srcFactor), toGLBlendFactor(dstFactor));
}

void OpenGLRHIDevice::setBlendOp(BlendOp op) {
    glBlendEquation(toGLBlendOp(op));
}

void OpenGLRHIDevice::setCullMode(CullMode mode) {
    if (mode == CullMode::None) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
        glCullFace(mode == CullMode::Front ? GL_FRONT : GL_BACK);
    }
}

void OpenGLRHIDevice::clearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void OpenGLRHIDevice::clearDepth(float depth) {
    glClearDepth(depth);
}

void OpenGLRHIDevice::clear(bool color, bool depth, bool stencil) {
    GLbitfield mask = 0;
    if (color) mask |= GL_COLOR_BUFFER_BIT;
    if (depth) mask |= GL_DEPTH_BUFFER_BIT;
    if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
    glClear(mask);
}

void OpenGLRHIDevice::draw(PrimitiveTopology topology, uint32_t vertexCount, uint32_t startVertex) {
    glDrawArrays(toGLPrimitiveTopology(topology), startVertex, vertexCount);
}

void OpenGLRHIDevice::drawIndexed(PrimitiveTopology topology, uint32_t indexCount, uint32_t startIndex) {
    glDrawElements(toGLPrimitiveTopology(topology), indexCount, GL_UNSIGNED_INT, (void*)(uintptr_t)(startIndex * sizeof(uint32_t)));
}

// Factory function implementation
std::shared_ptr<IRHIDevice> createRHIDevice(GraphicsAPI api) {
    switch (api) {
        case GraphicsAPI::OpenGL:
            return std::make_shared<OpenGLRHIDevice>();
        case GraphicsAPI::Vulkan:
        case GraphicsAPI::DirectX11:
        case GraphicsAPI::DirectX12:
        case GraphicsAPI::Metal:
            std::cerr << "Requested graphics API is not yet implemented" << std::endl;
            return nullptr;
        default:
            return nullptr;
    }
}

} // namespace RHI
} // namespace CarrotToy
