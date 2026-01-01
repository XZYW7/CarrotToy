#include "Renderer.h"
#include "Material.h"
#include "MaterialEditor.h"
#include "RayTracer.h"
#include <iostream>
#include <memory>

using namespace CarrotToy;

int maint() {
    std::cout << "CarrotToy - Material Editor Lab" << std::endl;
    std::cout << "================================" << std::endl;
    
    // Initialize renderer
    auto renderer = std::make_unique<Renderer>();
    if (!renderer->initialize(1280, 720, "CarrotToy - Material Editor")) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    std::cout << "Renderer initialized successfully" << std::endl;
    
    // Create default shader
    auto defaultShader = std::make_shared<Shader>(
        "shaders/default.vert",
        "shaders/default.frag"
    );
    
    // Create default material
    auto defaultMaterial = MaterialManager::getInstance().createMaterial(
        "DefaultPBR", 
        defaultShader
    );
    defaultMaterial->setVec3("albedo", 0.8f, 0.2f, 0.2f);
    defaultMaterial->setFloat("metallic", 0.5f);
    defaultMaterial->setFloat("roughness", 0.5f);
    
    std::cout << "Default material created" << std::endl;
    
    // Initialize material editor
    auto editor = std::make_unique<MaterialEditor>();
    if (!editor->initialize(renderer.get())) {
        std::cerr << "Failed to initialize material editor" << std::endl;
        return -1;
    }
    
    std::cout << "Material editor initialized successfully" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "- Use the Materials panel to select and create materials" << std::endl;
    std::cout << "- Use Material Properties to edit shader parameters" << std::endl;
    std::cout << "- Use Shader Editor to edit and recompile shaders in runtime" << std::endl;
    std::cout << "- Toggle between Rasterization and Ray Tracing modes" << std::endl;
    
    // Main loop
    while (!renderer->shouldClose()) {
        renderer->beginFrame();
        auto selected = editor->getSelectedMaterial() ? editor->getSelectedMaterial() : defaultMaterial;
        // Render scene with current material
        renderer->renderMaterialPreview(selected);
        
        // Render UI
        editor->render();
        
        renderer->endFrame();
    }
    
    // Cleanup
    editor->shutdown();
    renderer->shutdown();
    
    std::cout << "\nCarrotToy shutdown complete" << std::endl;
    
    return 0;
}
