#include "Platform/ImGuiContext.h"
#include "Platform/Platform.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace CarrotToy {

/**
 * OpenGL3 + GLFW implementation of ImGuiContext
 */
class ImGuiContextOpenGL : public ImGuiContext {
public:
    ImGuiContextOpenGL() 
        : initialized_(false), window_(nullptr) {
    }
    
    ~ImGuiContextOpenGL() override {
        shutdown();
    }
    
    bool initialize(Platform::IPlatformWindow* window) override {
        if (initialized_) {
            std::cerr << "ImGuiContext: Already initialized" << std::endl;
            return true;
        }
        
        if (!window) {
            std::cerr << "ImGuiContext: Cannot initialize with null window" << std::endl;
            return false;
        }
        
        window_ = window;
        
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        // Setup style
        ImGui::StyleColorsDark();
        
        // Setup Platform/Renderer backends
        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->getNativeHandle());
        if (!glfwWindow) {
            std::cerr << "ImGuiContext: Failed to get GLFW window handle" << std::endl;
            ImGui::DestroyContext();
            return false;
        }
        
        ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
        ImGui_ImplOpenGL3_Init("#version 460");
        
        std::cout << "ImGuiContext: Initialized successfully (OpenGL3 + GLFW backend)" << std::endl;
        initialized_ = true;
        return true;
    }
    
    void shutdown() override {
        if (!initialized_) {
            return;
        }
        
        if (ImGui::GetCurrentContext() == nullptr) {
            std::cerr << "ImGuiContext: Shutdown called but ImGui context is null" << std::endl;
            initialized_ = false;
            return;
        }
        
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        window_ = nullptr;
        initialized_ = false;
        std::cout << "ImGuiContext: Shutdown complete" << std::endl;
    }
    
    void beginFrame() override {
        if (!initialized_) {
            std::cerr << "ImGuiContext: Cannot begin frame - not initialized" << std::endl;
            return;
        }
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        
        // Fix: Force update DisplaySize if needed (for DLL boundary issues)
        ImGuiIO& io = ImGui::GetIO();
        if (window_ && (io.DisplaySize.x <= 0 || io.DisplaySize.y <= 0)) {
            uint32_t w, h;
            window_->getSize(w, h);
            uint32_t fw, fh;
            window_->getFramebufferSize(fw, fh);
            
            io.DisplaySize = ImVec2((float)w, (float)h);
            if (io.DisplaySize.x > 0 && io.DisplaySize.y > 0) {
                io.DisplayFramebufferScale = ImVec2((float)fw / w, (float)fh / h);
            }
            
            // Manually inject input events for DLL boundary compatibility
            double mx, my;
            window_->getCursorPos(mx, my);
            io.MousePos = ImVec2((float)mx, (float)my);
            
            io.MouseDown[0] = window_->getMouseButton(0); // Left
            io.MouseDown[1] = window_->getMouseButton(1); // Right
            io.MouseDown[2] = window_->getMouseButton(2); // Middle
        }
        
        ImGui::NewFrame();
    }
    
    void endFrame() override {
        if (!initialized_) {
            std::cerr << "ImGuiContext: Cannot end frame - not initialized" << std::endl;
            return;
        }
        
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    bool isInitialized() const override {
        return initialized_;
    }
    
private:
    bool initialized_;
    Platform::IPlatformWindow* window_;
};

// Factory function implementation
std::shared_ptr<ImGuiContext> createImGuiContext() {
    return std::make_shared<ImGuiContextOpenGL>();
}

} // namespace CarrotToy
