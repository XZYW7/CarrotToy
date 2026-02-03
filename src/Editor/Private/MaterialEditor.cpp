#include "MaterialEditor.h"
#include "Material.h"
#include "Renderer.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include "CoreUtils.h"
#include "Misc/Path.h"
namespace CarrotToy {
// 新增：从文件加载到 buffer 的小工具
static void loadFileToBuffer(const std::string& path, char* buf, size_t bufSize) {
    std::string loadFilePath = path;
    if (bufSize == 0) 
    {
        LOG("buffer = 0");
        return;
    }
    if (Path::endsWith(path, ".spv", false)) {
        // Map SPIR-V binary name back to the original HLSL source in the project's shader folder.
        // Example: "default.vs.spv" -> "<Project>/shaders/default.vs.hlsl"
        std::string noSpv = Path::removeExtension(path); // removes .spv -> may be "shaders/default.vs" or "default.vs"
        std::string fname = Path::getFilename(noSpv); // e.g. "default.vs"
        std::string shaderDir = Path::ShaderWorkingDir();
        if (shaderDir.empty()) {
            LOG("ShaderWorkingDir not set, cannot map .spv to .hlsl");
            buf[0] = '\0';
            return;
        }
        if (shaderDir.back() != '/' && shaderDir.back() != '\\') shaderDir.push_back('/');
        loadFilePath = shaderDir + fname + ".hlsl";
        LOG("Mapped .spv to HLSL source: " << loadFilePath);
    }
    // Find file
    auto absPath = std::filesystem::absolute(loadFilePath);
    if (!std::filesystem::exists(absPath)) {
        LOG("File NOT found at: " << absPath.string());
        LOG("Current working dir: " << std::filesystem::current_path().string());
        return;
    } else {
        LOG("Opening file: " << absPath.string());
    }

    std::ifstream ifs(absPath);
    if (!ifs) { buf[0] = '\0'; LOG("failed to open file: " << absPath); return; }
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    size_t n = std::min(content.size(), bufSize - 1);
    memcpy(buf, content.data(), n);
    buf[n] = '\0';
}

// 新增：尝试从 shader 对象或约定路径加载当前 shader 源
void MaterialEditor::loadCurrentShaderSources() {
    vertexShaderBuffer[0] = '\0';
    fragmentShaderBuffer[0] = '\0';
    if (selectedMaterialName.empty()) return;

    auto material = MaterialManager::getInstance().getMaterial(selectedMaterialName);
    LOG(material->getName());

    if (!material) return;
    auto shader = material->getShader();
    if (!shader) return;

    // 尝试通过 Shader 提供的路径接口加载（假定 getVertexPath/getFragmentPath 存在）
    // 如果你的 Shader 类使用不同命名，请替换为对应的 getter。
    std::string vpath, fpath;

    // Fallback: try conventional shader file names under shaders/
    vpath = shader->getVertexPath();
    fpath = shader->getFragmentPath();

    LOG(vpath);
    LOG(fpath);
    // 如果文件存在则读取到 buffer
    loadFileToBuffer(vpath, vertexShaderBuffer, sizeof(vertexShaderBuffer));
    loadFileToBuffer(fpath, fragmentShaderBuffer, sizeof(fragmentShaderBuffer));

}

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

    // TODO : Distangle the Imgui Logic from editor
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    // Get the native GLFW window handle from the platform-abstracted window
    GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(renderer->getWindowHandle());
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    
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
    
    // Fix: Force update DisplaySize and Scale BEFORE NewFrame if GLFW fails to provide it
    ImGuiIO& io = ImGui::GetIO();
    if (io.DisplaySize.x <= 0 || io.DisplaySize.y <= 0) {
        if (renderer && renderer->getWindow()) {
            uint32_t w, h;
            renderer->getWindow()->getSize(w, h);
            uint32_t fw, fh;
            renderer->getWindow()->getFramebufferSize(fw, fh);

            io.DisplaySize = ImVec2((float)w, (float)h);
            if (io.DisplaySize.x > 0 && io.DisplaySize.y > 0) {
                 io.DisplayFramebufferScale = ImVec2((float)fw / w, (float)fh / h);
            }
            
            // Fix: Manually inject input events since Callbacks via ImGui_ImplGlfw won't fire across DLL boundary
            // if strict static linking of GLFW is in play.
            double mx, my;
            renderer->getCursorPos(mx, my);
            io.MousePos = ImVec2((float)mx, (float)my);
            
            io.MouseDown[0] = renderer->getMouseButton(0); // Left
            io.MouseDown[1] = renderer->getMouseButton(1); // Right
            io.MouseDown[2] = renderer->getMouseButton(2); // Middle
        }
    }

    ImGui::NewFrame();
 
    showMaterialList();
    showMaterialProperties();
    showShaderEditor();
    showPreviewWindow();
    
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

std::shared_ptr<Material> MaterialEditor::getSelectedMaterial() const {
    if (selectedMaterialName.empty()) return nullptr;
    return MaterialManager::getInstance().getMaterial(selectedMaterialName);
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
            auto defaultShader = std::make_shared<Shader>(
                "shaders/default.vs.spv",
                "shaders/default.ps.spv"
            );
            
            // Create default material
            auto defaultMaterial = MaterialManager::getInstance().createMaterial(
                materialName, 
                defaultShader
            );
            defaultShader->reload();
            defaultMaterial->setVec3("albedo", 0.8f, 0.2f, 0.2f);
            defaultMaterial->setFloat("metallic", 0.5f);
            defaultMaterial->setFloat("roughness", 0.5f);
            defaultMaterial->setVec3("color", 0.1f, 0.1f, 0.1f);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    ImGui::Separator();
    
    for (auto& [name, material] : materials) {
        if (ImGui::Selectable(name.c_str(), selectedMaterialName == name)) {
            selectedMaterialName = name;
            material->getShader()->linkProgram();
            printf("Getting selected material: %s\n", selectedMaterialName.c_str());
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
                loadCurrentShaderSources();
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
                // TODO : Save shader to source file
                // Currently saves to the build directory.

                if (!selectedMaterialName.empty()) {
                    auto material = MaterialManager::getInstance().getMaterial(selectedMaterialName);
                    if (material && material->getShader()) {
                        // 1. 获取路径
                        std::string vPath = material->getShader()->getVertexPath();
                        std::string fPath = material->getShader()->getFragmentPath();

                        // 2. 定义保存辅助 lambda
                        auto saveFile = [](const std::string& path, const char* content) -> bool {
                            try {
                                // 确保父目录存在 (例如 shaders/ 文件夹)
                                auto p = std::filesystem::path(path);
                                if (p.has_parent_path()) {
                                    std::filesystem::create_directories(p.parent_path());
                                }
                                
                                std::ofstream out(path, std::ios::trunc | std::ios::binary);
                                if (out.is_open()) {
                                    out << content;
                                    LOG("Saved to: " << std::filesystem::absolute(p).string());
                                    return true;
                                }
                            } catch (const std::exception& e) {
                                LOG("Exception saving file: " << e.what());
                            }
                            LOG("Failed to save file: " << path);
                            return false;
                        };

                        // 3. 执行保存
                        bool vSaved = saveFile(vPath, vertexShaderBuffer);
                        bool fSaved = saveFile(fPath, fragmentShaderBuffer);

                        // 4. 保存成功后自动重新编译
                        if (vSaved && fSaved) {
                            if (material->getShader()->compile(vertexShaderBuffer, fragmentShaderBuffer)) {
                                LOG("Shader recompiled successfully after save.");
                            } else {
                                LOG("Shader compilation failed after save!");
                            }
                        }
                    }
                }
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
