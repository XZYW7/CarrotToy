#include "Material.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

namespace CarrotToy {

// Helper to check file extension
static bool hasExtension(const std::string& path, const std::string& ext) {
    if (path.length() < ext.length()) return false;
    std::string p = path.substr(path.length() - ext.length());
    std::transform(p.begin(), p.end(), p.begin(), ::tolower);
    return p == ext;
}

// Helper to compile SPIR-V shader
static bool compileSPIRVShader(unsigned int& shader, int type, const std::string& source) {
    // Check for GL_ARB_gl_spirv extension or OpenGL 4.6+
    // Note: GLAD_GL_ARB_gl_spirv might be undefined if not generated in glad.h
    // We assume the user has updated glad configuration to include it.
#ifdef GL_ARB_gl_spirv
    if (!GLAD_GL_ARB_gl_spirv) {
        std::cerr << "GL_ARB_gl_spirv not supported!" << std::endl;
        return false;
    }
#else
    // Fallback check if GL version is 4.6+ which includes SPIR-V support
    if (!GLAD_GL_VERSION_4_6) {
         std::cerr << "OpenGL 4.6 or GL_ARB_gl_spirv not supported!" << std::endl;
         return false;
    }
#endif

    shader = glCreateShader(type);
    
#ifdef GL_SHADER_BINARY_FORMAT_SPIR_V
    GLenum format = GL_SHADER_BINARY_FORMAT_SPIR_V;
#elif defined(GL_SHADER_BINARY_FORMAT_SPIR_V_ARB)
    GLenum format = GL_SHADER_BINARY_FORMAT_SPIR_V_ARB;
#else
    GLenum format = 0x9551; // Fallback constant
#endif

    glShaderBinary(1, &shader, format, source.data(), (GLsizei)source.size());

    // Entry point - assuming "VSMain" for vertex and "PSMain" for fragment for HLSL
    // You should compile your HLSL with:
    // dxc -T vs_6_0 -E VSMain ...
    // dxc -T ps_6_0 -E PSMain ...
    const char* entryPoint = (type == GL_VERTEX_SHADER) ? "VSMain" : "PSMain";
    
    // Specialization constants can be passed here if needed
#ifdef GL_ARB_gl_spirv
    glSpecializeShaderARB(shader, entryPoint, 0, nullptr, nullptr);
#else
    glSpecializeShader(shader, entryPoint, 0, nullptr, nullptr);
#endif

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "SPIR-V specialization failed: " << infoLog << std::endl;
        return false;
    }
    return true;
}

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
    auto readFile = [](const std::string& path) -> std::string {
        bool isBinary = hasExtension(path, ".spv");
        std::ifstream file(path, isBinary ? (std::ios::binary | std::ios::ate) : std::ios::ate);
        if (!file.is_open()) return "";
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::string buffer(size, '\0');
        if (file.read(&buffer[0], size)) return buffer;
        return "";
    };

    std::string vCode = readFile(vertexPath);
    std::string fCode = readFile(fragmentPath);
    
    if (vCode.empty() || fCode.empty()) {
        std::cerr << "Failed to open shader files: " << vertexPath << ", " << fragmentPath << std::endl;
        return;
    }
    
    compile(vCode, fCode);
}

bool Shader::compile(const std::string& vertexSource, const std::string& fragmentSource) {
    unsigned int vertex, fragment;
    bool vSuccess, fSuccess;
    
    if (hasExtension(vertexPath, ".spv")) {
        vSuccess = compileSPIRVShader(vertex, GL_VERTEX_SHADER, vertexSource);
    } else {
        vSuccess = compileShader(vertex, GL_VERTEX_SHADER, vertexSource);
    }
    
    if (!vSuccess) return false;
    
    if (hasExtension(fragmentPath, ".spv")) {
        fSuccess = compileSPIRVShader(fragment, GL_FRAGMENT_SHADER, fragmentSource);
    } else {
        fSuccess = compileShader(fragment, GL_FRAGMENT_SHADER, fragmentSource);
    }
    
    if (!fSuccess) {
        glDeleteShader(vertex);
        return false;
    }
    
    // Create and link program
    unsigned int newProgramID = glCreateProgram();
    glAttachShader(newProgramID, vertex);
    glAttachShader(newProgramID, fragment);
    glLinkProgram(newProgramID);
    
    int success;
    glGetProgramiv(newProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(newProgramID, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed: " << infoLog << std::endl;
        glDeleteProgram(newProgramID);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }
    
    // Delete old program if exists
    if (programID != 0) {
        glDeleteProgram(programID);
    }
    
    programID = newProgramID;
    
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
    // Linking is now done directly in compile() function
    // This function is kept for compatibility but is no longer used
    return true;
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
    // Clean up parameter data
    for (auto& [name, param] : parameters) {
        switch (param.type) {
            case ShaderParamType::Float:
                delete static_cast<float*>(param.data);
                break;
            case ShaderParamType::Vec3:
            case ShaderParamType::Vec4:
                delete[] static_cast<float*>(param.data);
                break;
            case ShaderParamType::Int:
                delete static_cast<int*>(param.data);
                break;
            case ShaderParamType::Bool:
                delete static_cast<bool*>(param.data);
                break;
            case ShaderParamType::Texture2D:
                delete static_cast<unsigned int*>(param.data);
                break;
            default:
                break;
        }
    }
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
