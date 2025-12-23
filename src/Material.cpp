#include "Material.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace CarrotToy {

// Shader implementation
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : vertexPath(vertexPath), fragmentPath(fragmentPath), programID(0) {
    reload();
}

Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

void Shader::use() {
    if (programID != 0) {
        glUseProgram(programID);
    }
}

void Shader::reload() {
    // Read shader files
    std::ifstream vFile(vertexPath);
    std::ifstream fFile(fragmentPath);
    
    if (!vFile.is_open() || !fFile.is_open()) {
        std::cerr << "Failed to open shader files: " << vertexPath << ", " << fragmentPath << std::endl;
        return;
    }
    
    std::stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();
    
    compile(vStream.str(), fStream.str());
}

bool Shader::compile(const std::string& vertexSource, const std::string& fragmentSource) {
    unsigned int vertex, fragment;
    
    if (!compileShader(vertex, GL_VERTEX_SHADER, vertexSource)) {
        return false;
    }
    
    if (!compileShader(fragment, GL_FRAGMENT_SHADER, fragmentSource)) {
        glDeleteShader(vertex);
        return false;
    }
    
    if (!linkProgram(vertex, fragment)) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }
    
    // Delete old program if exists
    if (programID != 0) {
        glDeleteProgram(programID);
    }
    
    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);
    
    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed: " << infoLog << std::endl;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return true;
}

bool Shader::compileShader(unsigned int& shader, int type, const std::string& source) {
    shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed (" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
                  << "): " << infoLog << std::endl;
        return false;
    }
    
    return true;
}

bool Shader::linkProgram(unsigned int vertex, unsigned int fragment) {
    return true; // Linking is done in compile()
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, float x, float y) {
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setBool(const std::string& name, bool value) {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value ? 1 : 0);
}

void Shader::setMatrix4(const std::string& name, const float* value) {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, value);
}

// Material implementation
Material::Material(const std::string& name, std::shared_ptr<Shader> shader)
    : name(name), shader(shader) {
}

Material::~Material() {
}

void Material::bind() {
    if (shader) {
        shader->use();
        
        // Apply all parameters
        for (auto& [name, param] : parameters) {
            switch (param.type) {
                case ShaderParamType::Float:
                    shader->setFloat(name, *(float*)param.data);
                    break;
                case ShaderParamType::Vec3: {
                    float* vec = (float*)param.data;
                    shader->setVec3(name, vec[0], vec[1], vec[2]);
                    break;
                }
                case ShaderParamType::Vec4: {
                    float* vec = (float*)param.data;
                    shader->setVec4(name, vec[0], vec[1], vec[2], vec[3]);
                    break;
                }
                case ShaderParamType::Int:
                    shader->setInt(name, *(int*)param.data);
                    break;
                case ShaderParamType::Bool:
                    shader->setBool(name, *(bool*)param.data);
                    break;
                default:
                    break;
            }
        }
    }
}

void Material::unbind() {
    glUseProgram(0);
}

void Material::setFloat(const std::string& name, float value) {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        *(float*)it->second.data = value;
    } else {
        float* data = new float(value);
        parameters.emplace(name, ShaderParameter(name, ShaderParamType::Float, data));
    }
}

void Material::setVec3(const std::string& name, float x, float y, float z) {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        float* vec = (float*)it->second.data;
        vec[0] = x; vec[1] = y; vec[2] = z;
    } else {
        float* data = new float[3]{x, y, z};
        parameters.emplace(name, ShaderParameter(name, ShaderParamType::Vec3, data));
    }
}

void Material::setVec4(const std::string& name, float x, float y, float z, float w) {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        float* vec = (float*)it->second.data;
        vec[0] = x; vec[1] = y; vec[2] = z; vec[3] = w;
    } else {
        float* data = new float[4]{x, y, z, w};
        parameters.emplace(name, ShaderParameter(name, ShaderParamType::Vec4, data));
    }
}

void Material::setTexture(const std::string& name, unsigned int textureID) {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        *(unsigned int*)it->second.data = textureID;
    } else {
        unsigned int* data = new unsigned int(textureID);
        parameters.emplace(name, ShaderParameter(name, ShaderParamType::Texture2D, data));
    }
}

// MaterialManager implementation
MaterialManager& MaterialManager::getInstance() {
    static MaterialManager instance;
    return instance;
}

std::shared_ptr<Material> MaterialManager::createMaterial(const std::string& name, std::shared_ptr<Shader> shader) {
    auto material = std::make_shared<Material>(name, shader);
    materials[name] = material;
    return material;
}

std::shared_ptr<Material> MaterialManager::getMaterial(const std::string& name) {
    auto it = materials.find(name);
    if (it != materials.end()) {
        return it->second;
    }
    return nullptr;
}

void MaterialManager::removeMaterial(const std::string& name) {
    materials.erase(name);
}

} // namespace CarrotToy
