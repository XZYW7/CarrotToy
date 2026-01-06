#include "Renderer.h"
#include "Material.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

namespace CarrotToy {

Renderer::Renderer() 
    : platform(nullptr), window(nullptr), width(800), height(600), 
      renderMode(RenderMode::Rasterization),
      sphereVAO(0), sphereVBO(0), sphereEBO(0),
      previewFBO(0), previewTexture(0) {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize(int w, int h, const std::string& title) {
    width = w;
    height = h;
    
    // Create platform
    platform = Platform::createPlatform();
    if (!platform) {
        std::cerr << "Failed to create platform" << std::endl;
        return false;
    }
    
    // Initialize platform
    if (!platform->initialize()) {
        std::cerr << "Failed to initialize platform" << std::endl;
        return false;
    }
    
    // Create window
    Platform::WindowDesc windowDesc;
    windowDesc.width = w;
    windowDesc.height = h;
    windowDesc.title = title.c_str();
    windowDesc.resizable = true;
    windowDesc.vsync = true;
    
    window = platform->createWindow(windowDesc);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }
    
    // Make context current
    window->makeContextCurrent();
    
    // Set resize callback
    window->setResizeCallback([](uint32_t width, uint32_t height) {
        glViewport(0, 0, width, height);
    });
    
    // Initialize GLAD using GLFW's proc address loader
    // Since we're using GLFW backend, we can use glfwGetProcAddress directly
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // Create and register a global RHI device (OpenGL backend)
    auto rhiDevice = CarrotToy::RHI::createRHIDevice(CarrotToy::RHI::GraphicsAPI::OpenGL);
    if (rhiDevice) {
        if (!rhiDevice->initialize()) {
            std::cerr << "Failed to initialize RHI device" << std::endl;
            return false;
        }
        CarrotToy::RHI::setGlobalDevice(rhiDevice);
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    
    setupPreviewGeometry();
    setupFramebuffer();
    
    return true;
}

void Renderer::shutdown() {
    if (sphereVAO) glDeleteVertexArrays(1, &sphereVAO);
    if (sphereVBO) glDeleteBuffers(1, &sphereVBO);
    if (sphereEBO) glDeleteBuffers(1, &sphereEBO);
    if (previewFBO) glDeleteFramebuffers(1, &previewFBO);
    if (previewTexture) glDeleteTextures(1, &previewTexture);
    
    // Shutdown window and platform
    window.reset();
    if (platform) {
        platform->shutdown();
        platform.reset();
    }
}

void Renderer::beginFrame() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    if (window) {
        window->swapBuffers();
        platform->pollEvents();
    }
}

void Renderer::renderMaterialPreview(std::shared_ptr<Material> material) {
    setPreviewMaterial(material);
    if (!material) return;
    
    material->bind();
    
    // Set up view and projection matrices
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
                                 glm::vec3(0.0f, 0.0f, 0.0f), 
                                 glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                           (float)width / (float)height, 
                                           0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)platform->getTime(), glm::vec3(0.0f, 1.0f, 0.0f));
    
    auto shader = material->getShader();
    // Use typed helpers to upload per-frame matrices and light/camera data
    shader->setPerFrameMatrices(glm::value_ptr(model), glm::value_ptr(view), glm::value_ptr(projection));
    float lightPos[3] = {10.0f, 10.0f, 0.0f};
    float lightColor[3] = {100.0f, 100.0f, 100.0f};
    float viewPos[3] = {0.0f, 0.0f, 3.0f};
    shader->setLightData(lightPos, lightColor, viewPos);
    
    // Render sphere
    if (sphereVAO) {
        glBindVertexArray(sphereVAO);
        // Calculate actual element count: latitudes * longitudes * 6 (2 triangles per quad)
        glDrawElements(GL_TRIANGLES, 50 * 50 * 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    material->unbind();
}

void Renderer::renderScene() {
    // Simple scene rendering - can be expanded
    beginFrame();
    
    // Render with active materials
    if (!previewMaterial) {
        renderMaterialPreview(previewMaterial);
    }
    
    endFrame();
}

bool Renderer::shouldClose() {
    if (!window) {
        std::cerr << "Warning: shouldClose() called with null window" << std::endl;
        return true;  // Return true to prevent infinite loops when window is missing
    }
    return window->shouldClose();
}

Platform::WindowHandle Renderer::getWindowHandle() const {
    return window ? window->getNativeHandle() : nullptr;
}
void Renderer::setPreviewMaterial(std::shared_ptr<Material> m) {
    previewMaterial = m;
}

std::shared_ptr<Material> Renderer::getPreviewMaterial() const {
    return previewMaterial;
}
void Renderer::setupPreviewGeometry() {
    // Create a simple sphere for material preview
    const int latitudes = 50;
    const int longitudes = 50;
    const float PI = 3.14159265358979323846f;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    for (int lat = 0; lat <= latitudes; ++lat) {
        float theta = lat * PI / latitudes;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        
        for (int lon = 0; lon <= longitudes; ++lon) {
            float phi = lon * 2 * PI / longitudes;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            
            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(x); // normal
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    
    for (int lat = 0; lat < latitudes; ++lat) {
        for (int lon = 0; lon < longitudes; ++lon) {
            int first = lat * (longitudes + 1) + lon;
            int second = first + longitudes + 1;
            
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    
    glBindVertexArray(sphereVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Renderer::setupFramebuffer() {
    // Setup framebuffer for preview rendering
    glGenFramebuffers(1, &previewFBO);
    glGenTextures(1, &previewTexture);
    
    glBindTexture(GL_TEXTURE_2D, previewTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindFramebuffer(GL_FRAMEBUFFER, previewFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previewTexture, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::exportSceneForRayTracing(const std::string& outputPath) {
    // Export scene data for offline ray tracing
    std::cout << "Exporting scene to: " << outputPath << std::endl;
    // TODO : Implementation would save scene geometry and materials to file
}

void Renderer::performOfflineRayTrace(const std::string& scenePath, const std::string& outputPath) {
    // Perform offline ray tracing
    std::cout << "Performing offline ray trace from: " << scenePath << " to: " << outputPath << std::endl;
    // Implementation would load scene and ray trace
    // TODO : Actual ray tracing implementation
}

} // namespace CarrotToy
