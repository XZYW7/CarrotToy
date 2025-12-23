# CarrotToy Examples and Usage Guide

## Basic Usage Examples

### Example 1: Creating a Simple Material

```cpp
#include "Material.h"
#include "Renderer.h"

// Create a shader
auto shader = std::make_shared<CarrotToy::Shader>(
    "shaders/default.vert",
    "shaders/default.frag"
);

// Create a material with the shader
auto material = CarrotToy::MaterialManager::getInstance()
    .createMaterial("MyMaterial", shader);

// Set material properties
material->setVec3("albedo", 0.8f, 0.1f, 0.1f);  // Red color
material->setFloat("metallic", 0.0f);           // Non-metallic
material->setFloat("roughness", 0.7f);          // Fairly rough
```

### Example 2: Runtime Shader Editing

```cpp
// Load and modify shader at runtime
auto shader = material->getShader();

// Edit shader code
std::string newFragmentCode = R"(
    #version 330 core
    out vec4 FragColor;
    in vec3 Normal;
    
    uniform vec3 color;
    
    void main() {
        vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
        float diff = max(dot(normalize(Normal), lightDir), 0.0);
        FragColor = vec4(color * diff, 1.0);
    }
)";

// Compile the new shader
shader->compile(vertexCode, newFragmentCode);
```

### Example 3: Creating Multiple Materials

```cpp
// Create a metallic material
auto metalShader = std::make_shared<CarrotToy::Shader>(
    "shaders/default.vert", "shaders/default.frag"
);
auto metal = CarrotToy::MaterialManager::getInstance()
    .createMaterial("Metal", metalShader);
metal->setVec3("albedo", 0.7f, 0.7f, 0.7f);
metal->setFloat("metallic", 1.0f);
metal->setFloat("roughness", 0.3f);

// Create a plastic material
auto plasticShader = std::make_shared<CarrotToy::Shader>(
    "shaders/default.vert", "shaders/default.frag"
);
auto plastic = CarrotToy::MaterialManager::getInstance()
    .createMaterial("Plastic", plasticShader);
plastic->setVec3("albedo", 0.2f, 0.6f, 0.9f);
plastic->setFloat("metallic", 0.0f);
plastic->setFloat("roughness", 0.5f);
```

### Example 4: Using the Unlit Shader

```cpp
auto unlitShader = std::make_shared<CarrotToy::Shader>(
    "shaders/unlit.vert",
    "shaders/unlit.frag"
);

auto unlitMaterial = CarrotToy::MaterialManager::getInstance()
    .createMaterial("Emissive", unlitShader);
unlitMaterial->setVec3("color", 1.0f, 0.5f, 0.0f);  // Orange glow
```

### Example 5: Ray Tracing Scene

```cpp
#include "RayTracer.h"

// Create and configure ray tracer
CarrotToy::RayTracer rayTracer;

// Export current scene
renderer.exportSceneForRayTracing("scene.txt");

// Load and render with ray tracing
rayTracer.loadScene("scene.txt");
rayTracer.render(1920, 1080, "output.png");
```

## Advanced Examples

### Custom PBR Parameters

```cpp
// Create a material with custom PBR values
auto material = CarrotToy::MaterialManager::getInstance()
    .createMaterial("CustomPBR", pbrShader);

// Dielectric materials (non-metals)
material->setVec3("albedo", 0.5f, 0.0f, 0.0f);  // Base color
material->setFloat("metallic", 0.0f);            // 0 = dielectric
material->setFloat("roughness", 0.2f);           // Smooth surface

// Metallic materials
material->setVec3("albedo", 1.0f, 0.85f, 0.57f); // Gold color
material->setFloat("metallic", 1.0f);             // Full metal
material->setFloat("roughness", 0.1f);            // Very smooth
```

### Custom Shader with Additional Uniforms

Create `shaders/custom.frag`:
```glsl
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 baseColor;
uniform float emissiveStrength;
uniform float time;

void main() {
    vec3 N = normalize(Normal);
    float pulse = sin(time * 2.0) * 0.5 + 0.5;
    vec3 emissive = baseColor * emissiveStrength * pulse;
    FragColor = vec4(baseColor + emissive, 1.0);
}
```

Use in code:
```cpp
auto shader = std::make_shared<CarrotToy::Shader>(
    "shaders/default.vert",
    "shaders/custom.frag"
);

auto material = CarrotToy::MaterialManager::getInstance()
    .createMaterial("Pulsing", shader);
material->setVec3("baseColor", 0.0f, 1.0f, 0.5f);
material->setFloat("emissiveStrength", 2.0f);
```

### Material Presets

```cpp
class MaterialPresets {
public:
    static std::shared_ptr<CarrotToy::Material> createGold() {
        auto mat = createPBRMaterial("Gold");
        mat->setVec3("albedo", 1.0f, 0.85f, 0.57f);
        mat->setFloat("metallic", 1.0f);
        mat->setFloat("roughness", 0.2f);
        return mat;
    }
    
    static std::shared_ptr<CarrotToy::Material> createRubber() {
        auto mat = createPBRMaterial("Rubber");
        mat->setVec3("albedo", 0.1f, 0.1f, 0.1f);
        mat->setFloat("metallic", 0.0f);
        mat->setFloat("roughness", 0.9f);
        return mat;
    }
    
    static std::shared_ptr<CarrotToy::Material> createGlass() {
        auto mat = createPBRMaterial("Glass");
        mat->setVec3("albedo", 0.95f, 0.95f, 0.95f);
        mat->setFloat("metallic", 0.0f);
        mat->setFloat("roughness", 0.05f);
        return mat;
    }
    
private:
    static std::shared_ptr<CarrotToy::Material> createPBRMaterial(
        const std::string& name) {
        auto shader = std::make_shared<CarrotToy::Shader>(
            "shaders/default.vert",
            "shaders/default.frag"
        );
        return CarrotToy::MaterialManager::getInstance()
            .createMaterial(name, shader);
    }
};
```

## UI Interaction Examples

### Programmatically Controlling the Material Editor

```cpp
#include "MaterialEditor.h"

CarrotToy::MaterialEditor editor;
editor.initialize(&renderer);

// Set callback for shader recompilation
editor.setOnShaderRecompile([]() {
    std::cout << "Shader recompiled!" << std::endl;
    // Perform any post-recompile actions
});

// Main loop
while (!renderer.shouldClose()) {
    renderer.beginFrame();
    renderer.renderMaterialPreview(activeMaterial);
    editor.render();
    renderer.endFrame();
}
```

### Custom Render Loop

```cpp
while (!renderer.shouldClose()) {
    renderer.beginFrame();
    
    // Update time uniform for animated shaders
    float time = glfwGetTime();
    activeMaterial->getShader()->use();
    activeMaterial->getShader()->setFloat("time", time);
    
    // Render with current material
    renderer.renderMaterialPreview(activeMaterial);
    
    // Render UI
    editor.render();
    
    renderer.endFrame();
}
```

## Tips and Best Practices

### Performance Optimization

1. **Minimize shader recompilation**: Only recompile when necessary
2. **Batch material changes**: Update multiple parameters before rendering
3. **Use material instancing**: Share shaders between materials when possible

### Shader Development Workflow

1. Start with a basic shader template
2. Test in rasterization mode first
3. Iterate with the shader editor
4. Save successful shaders to files
5. Test edge cases (extreme parameter values)

### Material Organization

```cpp
// Organize materials by category
auto& manager = CarrotToy::MaterialManager::getInstance();

// Metals
manager.createMaterial("Gold", metalShader);
manager.createMaterial("Silver", metalShader);
manager.createMaterial("Copper", metalShader);

// Plastics
manager.createMaterial("RedPlastic", plasticShader);
manager.createMaterial("BluePlastic", plasticShader);

// Special effects
manager.createMaterial("Emissive", unlitShader);
manager.createMaterial("Hologram", holoShader);
```

## Troubleshooting Common Issues

### Shader Compilation Errors

Check the console output for error messages. Common issues:
- Missing uniform declarations
- Incorrect GLSL version
- Type mismatches in operations

### Black Materials

If materials appear black:
1. Check if shader compiled successfully
2. Verify uniform values are being set
3. Ensure normals are correct
4. Check lighting calculations

### UI Not Responding

Ensure ImGui is initialized:
```cpp
if (!editor.initialize(&renderer)) {
    std::cerr << "Failed to initialize editor" << std::endl;
}
```

## Next Steps

- Explore the shader files in `shaders/` directory
- Modify the PBR shader to add new features
- Create custom material presets for your use case
- Experiment with the ray tracing functionality
- Implement texture support for materials

For more information, see the [README](README.md) and [BUILD](BUILD.md) documentation.
