#pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>
#include "Shader.h"

namespace CarrotToy {

// Shader parameter types
enum class ShaderParamType {
    Float,
    Vec2,
    Vec3,
    Vec4,
    Int,
    Bool,
    Texture2D,
    Matrix4
};

// Shader parameter structure
struct ShaderParameter {
    std::string name;
    ShaderParamType type;
    void* data;
    
    ShaderParameter(const std::string& name, ShaderParamType type, void* data)
        : name(name), type(type), data(data) {}
};


// Material class - represents a material with shader and parameters
class RENDERER_API Material {
public:
    Material(const std::string& name, std::shared_ptr<Shader> shader);
    ~Material();
    
    void bind();
    void unbind();
    
    // Parameter management
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setTexture(const std::string& name, unsigned int textureID);
    
    std::shared_ptr<Shader> getShader() { return shader; }
    std::string getName() const { return name; }
    
    std::map<std::string, ShaderParameter>& getParameters() { return parameters; }
    
private:
    std::string name;
    std::shared_ptr<Shader> shader;
    std::map<std::string, ShaderParameter> parameters;
};

// Material Manager - manages all materials in the scene
class RENDERER_API MaterialManager {
public:
    static MaterialManager& getInstance();
    
    std::shared_ptr<Material> createMaterial(const std::string& name, std::shared_ptr<Shader> shader);
    std::shared_ptr<Material> getMaterial(const std::string& name);
    void removeMaterial(const std::string& name);
    
    std::map<std::string, std::shared_ptr<Material>>& getAllMaterials() { return materials; }
    
private:
    MaterialManager() = default;
    std::map<std::string, std::shared_ptr<Material>> materials;
};

} // namespace CarrotToy
