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

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Renderer::Renderer() 
    : window(nullptr), width(800), height(600), 
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
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
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
    
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Renderer::beginFrame() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
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
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
    
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
    return window ? glfwWindowShouldClose(window) : true;
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
