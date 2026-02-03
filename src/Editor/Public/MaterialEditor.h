#pragma once

#include <string>
#include <functional>
#include <memory>
#include "EditorAPI.h"
namespace CarrotToy {

class Material;
class Renderer;

// Material Editor UI class - provides ImGui interface for material editing
class EDITOR_API MaterialEditor{
public:

    MaterialEditor();
    ~MaterialEditor();
    
    bool initialize(Renderer* renderer);
    void shutdown();
    
    void render();

    std::shared_ptr<Material> getSelectedMaterial() const;

    void showMaterialList();
    void showMaterialProperties();
    void showShaderEditor();
    void showPreviewWindow();
    
    void setOnShaderRecompile(std::function<void()> callback) { 
        onShaderRecompile = callback; 
    }


    
private:
    bool initialized = false;

    Renderer* renderer;
    std::string selectedMaterialName;
    
    // Shader editor state
    char vertexShaderBuffer[8192];
    char fragmentShaderBuffer[8192];
    bool shaderEditorOpen;
    
    std::function<void()> onShaderRecompile;
    
    void renderMaterialParameter(const std::string& name, void* data, int type);
    void loadCurrentShaderSources();
};

} // namespace CarrotToy
