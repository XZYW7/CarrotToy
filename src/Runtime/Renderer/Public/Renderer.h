#pragma once

#include <memory>
#include <string>
#include "Platform/Platform.h"
#include "Input/InputDevice.h"
#include "RendererAPI.h"

namespace CarrotToy {

class Shader;
class Material;

// Renderer class - manages the rendering pipeline
class RENDERER_API Renderer {
public:
    enum class RenderMode {
        Rasterization,  // Real-time rasterization
        RayTracing      // Offline ray tracing
    };
    
    Renderer();
    ~Renderer();
    
    bool initialize(int width, int height, const std::string& title);
    void shutdown();
    
    void beginFrame();
    void endFrame();
    
    void renderMaterialPreview(std::shared_ptr<Material> material);
    void renderScene();
    
    void setPreviewMaterial(std::shared_ptr<Material> m);
    std::shared_ptr<Material> getPreviewMaterial() const;

    bool shouldClose();
    Platform::WindowHandle getWindowHandle() const;
    std::shared_ptr<Platform::IPlatformWindow> getWindow() const { return window; }
    std::shared_ptr<Input::IInputDevice> getInputDevice() const { return inputDevice; }
    
    void setRenderMode(RenderMode mode) { renderMode = mode; }
    RenderMode getRenderMode() const { return renderMode; }

    // Input proxies (deprecated - use getInputDevice() instead)
    void getCursorPos(double& x, double& y) const;
    bool getMouseButton(int button) const;
    
    // Offline ray tracing
    void exportSceneForRayTracing(const std::string& outputPath);
    void performOfflineRayTrace(const std::string& scenePath, const std::string& outputPath);
    
private:
    std::shared_ptr<Platform::IPlatformWindow> window;
    std::shared_ptr<Platform::IPlatform> cachedPlatform;  // Cached for getTime() calls
    std::shared_ptr<Input::IInputDevice> inputDevice;
    int width, height;
    RenderMode renderMode;
    
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    unsigned int previewFBO, previewTexture;
    
    void setupPreviewGeometry();
    void setupFramebuffer();

    std::shared_ptr<Material> previewMaterial;
};

} // namespace CarrotToy
