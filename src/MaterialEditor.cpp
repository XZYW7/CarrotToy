#include "MaterialEditor.h"
#include "Material.h"
#include "Renderer.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace CarrotToy {

MaterialEditor::MaterialEditor() 
    : renderer(nullptr), shaderEditorOpen(false) {
    vertexShaderBuffer[0] = '\0';
    fragmentShaderBuffer[0] = '\0';
}

MaterialEditor::~MaterialEditor() {
    if (initialized) {
        shutdown();
    }
}

bool MaterialEditor::initialize(Renderer* r) {
    initialized = true;
    renderer = r;
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(renderer->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return initialized;
}

void MaterialEditor::shutdown() {
    if (ImGui::GetCurrentContext() == nullptr) 
    {
        std::cerr << "MaterialEditor::shutdown called but ImGui context is null!" << std::endl;
        return;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    renderer = nullptr;
    initialized = false;
}

void MaterialEditor::render() {
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    showMaterialList();
    showMaterialProperties();
    showShaderEditor();
    showPreviewWindow();
    
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MaterialEditor::showMaterialList() {
    ImGui::Begin("Materials");
    
    auto& materials = MaterialManager::getInstance().getAllMaterials();
    
    if (ImGui::Button("Create New Material")) {
        // Create a default material
        ImGui::OpenPopup("Create Material");
    }
    
    if (ImGui::BeginPopup("Create Material")) {
        static char materialName[128] = "NewMaterial";
        ImGui::InputText("Name", materialName, 128);
        
        if (ImGui::Button("Create")) {
            // Would need a default shader
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    ImGui::Separator();
    
    for (auto& [name, material] : materials) {
        if (ImGui::Selectable(name.c_str(), selectedMaterialName == name)) {
            selectedMaterialName = name;
        }
    }
    
    ImGui::End();
}

void MaterialEditor::showMaterialProperties() {
    ImGui::Begin("Material Properties");
    
    if (!selectedMaterialName.empty()) {
        auto material = MaterialManager::getInstance().getMaterial(selectedMaterialName);
        if (material) {
            ImGui::Text("Material: %s", selectedMaterialName.c_str());
            ImGui::Separator();
            
            auto& parameters = material->getParameters();
            for (auto& [name, param] : parameters) {
                renderMaterialParameter(name, param.data, static_cast<int>(param.type));
            }
            
            if (ImGui::Button("Edit Shader")) {
                shaderEditorOpen = true;
            }
        }
    } else {
        ImGui::Text("No material selected");
    }
    
    ImGui::End();
}

void MaterialEditor::showShaderEditor() {
    if (!shaderEditorOpen) return;
    
    ImGui::Begin("Shader Editor", &shaderEditorOpen, ImGuiWindowFlags_MenuBar);
    
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save")) {
                // TODO : Save shader
            }
            if (ImGui::MenuItem("Recompile")) {
                if (onShaderRecompile) {
                    onShaderRecompile();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    ImGui::Text("Vertex Shader:");
    ImGui::InputTextMultiline("##vertex", vertexShaderBuffer, sizeof(vertexShaderBuffer), 
                              ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16));
    
    ImGui::Separator();
    
    ImGui::Text("Fragment Shader:");
    ImGui::InputTextMultiline("##fragment", fragmentShaderBuffer, sizeof(fragmentShaderBuffer), 
                              ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16));
    
    if (ImGui::Button("Compile and Apply")) {
        if (!selectedMaterialName.empty()) {
            auto material = MaterialManager::getInstance().getMaterial(selectedMaterialName);
            if (material && material->getShader()) {
                if (material->getShader()->compile(vertexShaderBuffer, fragmentShaderBuffer)) {
                    std::cout << "Shader recompiled successfully!" << std::endl;
                } else {
                    std::cout << "Shader compilation failed!" << std::endl;
                }
            }
        }
    }
    
    ImGui::End();
}

void MaterialEditor::showPreviewWindow() {
    ImGui::Begin("Material Preview");
    
    ImGui::Text("Preview render mode:");
    static int renderMode = 0;
    ImGui::RadioButton("Rasterization", &renderMode, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Ray Tracing", &renderMode, 1);
    
    if (renderer) {
        renderer->setRenderMode(renderMode == 0 ? 
            Renderer::RenderMode::Rasterization : 
            Renderer::RenderMode::RayTracing);
    }
    
    if (ImGui::Button("Export Scene for Ray Tracing")) {
        if (renderer) {
            renderer->exportSceneForRayTracing("scene_export.txt");
        }
    }
    
    ImGui::End();
}

void MaterialEditor::renderMaterialParameter(const std::string& name, void* data, int type) {
    using ParamType = CarrotToy::ShaderParamType;
    switch (static_cast<ParamType>(type)) {
        case ParamType::Float:
            ImGui::DragFloat(name.c_str(), (float*)data, 0.01f);
            break;
        case ParamType::Vec3:
            ImGui::ColorEdit3(name.c_str(), (float*)data);
            break;
        case ParamType::Vec4:
            ImGui::ColorEdit4(name.c_str(), (float*)data);
            break;
        case ParamType::Int:
            ImGui::DragInt(name.c_str(), (int*)data);
            break;
        case ParamType::Bool:
            ImGui::Checkbox(name.c_str(), (bool*)data);
            break;
        default:
            break;
    }
}

} // namespace CarrotToy
