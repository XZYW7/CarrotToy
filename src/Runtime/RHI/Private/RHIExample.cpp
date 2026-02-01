#include "RHI/RHI.h"
#include "RHI/OpenGLRHI.h"
#include "Platform/Platform.h"
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

// Example demonstrating how to use the RHI (Render Hardware Interface)
// This file is for demonstration purposes and shows the API usage patterns

namespace CarrotToy {
namespace RHI {
namespace Example {

void demonstrateRHIUsage() {
    // 1. Create an RHI device (OpenGL in this case)
    auto rhiDevice = createRHIDevice(GraphicsAPI::OpenGL);
    if (!rhiDevice) {
        std::cerr << "Failed to create RHI device" << std::endl;
        return;
    }
    
    // 2. Initialize the device
    if (!rhiDevice->initialize()) {
        std::cerr << "Failed to initialize RHI device" << std::endl;
        return;
    }
    
    std::cout << "RHI Device initialized successfully" << std::endl;
    
    // 3. Create a vertex buffer
    float vertices[] = {
        // positions         // colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom right
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top
    };
    
    BufferDesc vertexBufferDesc;
    vertexBufferDesc.type = BufferType::Vertex;
    vertexBufferDesc.usage = BufferUsage::Static;
    vertexBufferDesc.size = sizeof(vertices);
    vertexBufferDesc.initialData = vertices;
    
    auto vertexBuffer = rhiDevice->createBuffer(vertexBufferDesc);
    std::cout << "Vertex buffer created" << std::endl;
    
    // 4. Create an index buffer
    uint32_t indices[] = { 0, 1, 2 };
    
    BufferDesc indexBufferDesc;
    indexBufferDesc.type = BufferType::Index;
    indexBufferDesc.usage = BufferUsage::Static;
    indexBufferDesc.size = sizeof(indices);
    indexBufferDesc.initialData = indices;
    
    auto indexBuffer = rhiDevice->createBuffer(indexBufferDesc);
    std::cout << "Index buffer created" << std::endl;
    
    // 5. Create shaders
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        
        out vec3 vertexColor;
        
        void main() {
            gl_Position = vec4(aPos, 1.0);
            vertexColor = aColor;
        }
    )";
    
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";
    
    ShaderDesc vertexShaderDesc;
    vertexShaderDesc.type = ShaderType::Vertex;
    vertexShaderDesc.source = vertexShaderSource;
    vertexShaderDesc.sourceSize = strlen(vertexShaderSource);
    
    ShaderDesc fragmentShaderDesc;
    fragmentShaderDesc.type = ShaderType::Fragment;
    fragmentShaderDesc.source = fragmentShaderSource;
    fragmentShaderDesc.sourceSize = strlen(fragmentShaderSource);
    
    auto vertexShader = rhiDevice->createShader(vertexShaderDesc);
    auto fragmentShader = rhiDevice->createShader(fragmentShaderDesc);
    
    if (!vertexShader->compile()) {
        std::cerr << "Vertex shader compilation failed: " << vertexShader->getCompileErrors() << std::endl;
        return;
    }
    
    if (!fragmentShader->compile()) {
        std::cerr << "Fragment shader compilation failed: " << fragmentShader->getCompileErrors() << std::endl;
        return;
    }
    
    std::cout << "Shaders compiled successfully" << std::endl;
    
    // 6. Create shader program
    auto shaderProgram = rhiDevice->createShaderProgram();
    
    // Attach shaders using the RHI interface (no backend-specific cast needed)
    shaderProgram->attachShader(vertexShader.get());
    shaderProgram->attachShader(fragmentShader.get());
    
    if (!shaderProgram->link()) {
        std::cerr << "Shader program linking failed: " << shaderProgram->getLinkErrors() << std::endl;
        return;
    }
    
    std::cout << "Shader program linked successfully" << std::endl;
    
    // 7. Create vertex array and set up vertex attributes
    auto vertexArray = rhiDevice->createVertexArray();
    vertexArray->bind();
    vertexArray->setVertexBuffer(vertexBuffer.get(), 0);
    vertexArray->setIndexBuffer(indexBuffer.get());
    
    // Position attribute (interleaved with color, stride = 6 floats)
    VertexAttribute positionAttr;
    positionAttr.location = 0;
    positionAttr.binding = 0;
    positionAttr.offset = 0;
    positionAttr.componentCount = 3;
    positionAttr.stride = 6 * sizeof(float);  // position + color per vertex
    positionAttr.normalized = false;
    vertexArray->setVertexAttribute(positionAttr);
    
    // Color attribute (interleaved with position, stride = 6 floats)
    VertexAttribute colorAttr;
    colorAttr.location = 1;
    colorAttr.binding = 0;
    colorAttr.offset = 3 * sizeof(float);
    colorAttr.componentCount = 3;
    colorAttr.stride = 6 * sizeof(float);  // position + color per vertex
    colorAttr.normalized = false;
    vertexArray->setVertexAttribute(colorAttr);
    
    vertexArray->unbind();
    std::cout << "Vertex array configured" << std::endl;
    
    // 8. Create a texture
    TextureDesc textureDesc;
    textureDesc.width = 256;
    textureDesc.height = 256;
    textureDesc.format = TextureFormat::RGBA8;
    textureDesc.minFilter = TextureFilter::Linear;
    textureDesc.magFilter = TextureFilter::Linear;
    textureDesc.wrapS = TextureWrap::Repeat;
    textureDesc.wrapT = TextureWrap::Repeat;
    
    auto texture = rhiDevice->createTexture(textureDesc);
    std::cout << "Texture created" << std::endl;
    
    // 9. Create a framebuffer
    FramebufferDesc framebufferDesc;
    framebufferDesc.width = 1280;
    framebufferDesc.height = 720;
    framebufferDesc.hasDepthStencil = true;
    
    auto framebuffer = rhiDevice->createFramebuffer(framebufferDesc);
    if (framebuffer->isComplete()) {
        std::cout << "Framebuffer created and complete" << std::endl;
    }
    
    // 10. Demonstrate rendering state setup
    rhiDevice->setViewport(0, 0, 1280, 720);
    rhiDevice->setDepthTest(true);
    rhiDevice->setDepthFunc(CompareFunc::Less);
    rhiDevice->setCullMode(CullMode::Back);
    rhiDevice->setBlend(false);
    
    std::cout << "Rendering state configured" << std::endl;
    
    // 11. Demonstrate a render pass
    rhiDevice->clearColor(0.2f, 0.2f, 0.2f, 1.0f);
    rhiDevice->clear(true, true, false);
    
    shaderProgram->bind();
    shaderProgram->setUniformFloat("time", 0.0f);
    shaderProgram->setUniformVec3("lightPos", 0.0f, 10.0f, 0.0f);
    
    vertexArray->bind();
    rhiDevice->drawIndexed(PrimitiveTopology::TriangleList, 3, 0);
    vertexArray->unbind();
    
    shaderProgram->unbind();
    
    std::cout << "Render pass demonstrated" << std::endl;
    std::cout << "Note: This is a demonstration of RHI API usage patterns." << std::endl;
    std::cout << "In a real application, integrate with the Platform layer for window management." << std::endl;
    
    // 12. Cleanup is automatic via smart pointers
    rhiDevice->shutdown();
    std::cout << "RHI demonstration complete" << std::endl;
}

} // namespace Example
} // namespace RHI
} // namespace CarrotToy


// int main() {
//     CarrotToy::RHI::Example::demonstrateRHIUsage();
//     return 0;
// }
