#pragma once

#include "RHI.h"
#include "RHIResources.h"
#include <vector>
#include <unordered_map>

// Forward declare Vulkan types to avoid including vulkan.h in header
struct VkInstance_T;
struct VkPhysicalDevice_T;
struct VkDevice_T;
struct VkQueue_T;
struct VkCommandPool_T;
struct VkCommandBuffer_T;
struct VkBuffer_T;
struct VkDeviceMemory_T;
struct VkShaderModule_T;
struct VkPipeline_T;
struct VkPipelineLayout_T;
struct VkDescriptorSetLayout_T;
struct VkDescriptorPool_T;
struct VkDescriptorSet_T;
struct VkImage_T;
struct VkImageView_T;
struct VkSampler_T;
struct VkFramebuffer_T;
struct VkRenderPass_T;

typedef VkInstance_T* VkInstance;
typedef VkPhysicalDevice_T* VkPhysicalDevice;
typedef VkDevice_T* VkDevice;
typedef VkQueue_T* VkQueue;
typedef VkCommandPool_T* VkCommandPool;
typedef VkCommandBuffer_T* VkCommandBuffer;
typedef VkBuffer_T* VkBuffer;
typedef VkDeviceMemory_T* VkDeviceMemory;
typedef VkShaderModule_T* VkShaderModule;
typedef VkPipeline_T* VkPipeline;
typedef VkPipelineLayout_T* VkPipelineLayout;
typedef VkDescriptorSetLayout_T* VkDescriptorSetLayout;
typedef VkDescriptorPool_T* VkDescriptorPool;
typedef VkDescriptorSet_T* VkDescriptorSet;
typedef VkImage_T* VkImage;
typedef VkImageView_T* VkImageView;
typedef VkSampler_T* VkSampler;
typedef VkFramebuffer_T* VkFramebuffer;
typedef VkRenderPass_T* VkRenderPass;

namespace CarrotToy {
namespace RHI {

// Forward declarations
class VulkanBuffer;
class VulkanShader;
class VulkanShaderProgram;
class VulkanTexture;
class VulkanFramebuffer;
class VulkanVertexArray;
class VulkanUniformBuffer;

// Vulkan Buffer implementation
class VulkanBuffer : public IRHIBuffer {
public:
    VulkanBuffer(VkDevice device, const BufferDesc& desc);
    ~VulkanBuffer() override;
    
    void updateData(const void* data, size_t size, size_t offset = 0) override;
    void* map() override;
    void unmap() override;
    
    bool isValid() const override { return buffer != nullptr; }
    void release() override;
    
    size_t getSize() const override { return size; }
    BufferType getType() const override { return type; }
    
    VkBuffer getBuffer() const { return buffer; }
    
private:
    VkDevice device;
    VkBuffer buffer;
    VkDeviceMemory memory;
    BufferType type;
    BufferUsage usage;
    size_t size;
};

// Vulkan Shader implementation
class VulkanShader : public IRHIShader {
public:
    VulkanShader(VkDevice device, const ShaderDesc& desc);
    ~VulkanShader() override;
    
    bool compile() override;
    std::string getCompileErrors() const override { return errors; }
    
    bool isValid() const override { return shaderModule != nullptr; }
    void release() override;
    
    ShaderType getType() const override { return type; }
    VkShaderModule getShaderModule() const { return shaderModule; }
    
private:
    VkDevice device;
    VkShaderModule shaderModule;
    ShaderType type;
    std::vector<uint8_t> source;
    std::string errors;
};

// Vulkan Shader Program implementation
class VulkanShaderProgram : public IRHIShaderProgram {
public:
    VulkanShaderProgram(VkDevice device);
    ~VulkanShaderProgram() override;
    
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
    
    bool isValid() const override { return pipeline != nullptr; }
    void release() override;
    
    VkPipeline getPipeline() const { return pipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
    
private:
    VkDevice device;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    std::string errors;
    std::vector<VulkanShader*> attachedShaders;
};

// Vulkan Texture implementation
class VulkanTexture : public IRHITexture {
public:
    VulkanTexture(VkDevice device, const TextureDesc& desc);
    ~VulkanTexture() override;
    
    void updateData(const void* data, uint32_t width, uint32_t height) override;
    void bind(uint32_t slot = 0) override;
    void unbind() override;
    
    bool isValid() const override { return image != nullptr; }
    void release() override;
    
    uint32_t getWidth() const override { return width; }
    uint32_t getHeight() const override { return height; }
    TextureFormat getFormat() const override { return format; }
    
    VkImage getImage() const { return image; }
    VkImageView getImageView() const { return imageView; }
    VkSampler getSampler() const { return sampler; }
    
private:
    VkDevice device;
    VkImage image;
    VkImageView imageView;
    VkSampler sampler;
    VkDeviceMemory memory;
    uint32_t width;
    uint32_t height;
    TextureFormat format;
};

// Vulkan Framebuffer implementation
class VulkanFramebuffer : public IRHIFramebuffer {
public:
    VulkanFramebuffer(VkDevice device, const FramebufferDesc& desc);
    ~VulkanFramebuffer() override;
    
    void bind() override;
    void unbind() override;
    void attachColorTexture(IRHITexture* texture, uint32_t attachment = 0) override;
    void attachDepthTexture(IRHITexture* texture) override;
    bool isComplete() override;
    
    bool isValid() const override { return framebuffer != nullptr; }
    void release() override;
    
    IRHITexture* getColorTexture(uint32_t attachment = 0) override;
    IRHITexture* getDepthTexture() override;
    
    VkFramebuffer getFramebuffer() const { return framebuffer; }
    VkRenderPass getRenderPass() const { return renderPass; }
    
private:
    VkDevice device;
    VkFramebuffer framebuffer;
    VkRenderPass renderPass;
    std::vector<std::shared_ptr<IRHITexture>> colorTextures;
    std::shared_ptr<IRHITexture> depthTexture;
    uint32_t width;
    uint32_t height;
};

// Vulkan Vertex Array implementation (Input Assembly)
class VulkanVertexArray : public IRHIVertexArray {
public:
    VulkanVertexArray(VkDevice device);
    ~VulkanVertexArray() override;
    
    void bind() override;
    void unbind() override;
    void setVertexBuffer(IRHIBuffer* buffer, uint32_t binding = 0) override;
    void setIndexBuffer(IRHIBuffer* buffer) override;
    void setVertexAttribute(const VertexAttribute& attribute) override;
    
    bool isValid() const override { return true; }
    void release() override;
    
private:
    VkDevice device;
    std::vector<IRHIBuffer*> vertexBuffers;
    IRHIBuffer* indexBuffer;
    std::vector<VertexAttribute> attributes;
};

// Vulkan Uniform Buffer implementation
class VulkanUniformBuffer : public IRHIUniformBuffer {
public:
    VulkanUniformBuffer(VkDevice device, size_t size, uint32_t binding);
    ~VulkanUniformBuffer() override;
    
    void update(const void* data, size_t size, size_t offset = 0) override;
    void bind(uint32_t binding) override;
    
    size_t getSize() const override { return sizeBytes; }
    bool isValid() const override { return buffer != nullptr; }
    void release() override;
    
    uintptr_t getNativeHandle() const override { return (uintptr_t)buffer; }
    VkBuffer getBuffer() const { return buffer; }
    
private:
    VkDevice device;
    VkBuffer buffer;
    VkDeviceMemory memory;
    size_t sizeBytes;
    uint32_t binding;
};

// Vulkan RHI Device implementation
class VulkanRHIDevice : public IRHIDevice {
public:
    VulkanRHIDevice();
    ~VulkanRHIDevice() override;
    
    bool initialize() override;
    void shutdown() override;
    
    GraphicsAPI getGraphicsAPI() const override { return GraphicsAPI::Vulkan; }
    
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
    
    // Vulkan-specific accessors
    VkInstance getInstance() const { return instance; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkDevice getDevice() const { return device; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkCommandPool getCommandPool() const { return commandPool; }
    
private:
    bool initialized;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkCommandPool commandPool;
    VkCommandBuffer currentCommandBuffer;
    
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
