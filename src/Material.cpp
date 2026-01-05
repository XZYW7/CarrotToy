#include "Material.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>


#include "CoreUtils.h"



namespace CarrotToy {


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

        // If the shader exposes a Material UBO, pack all parameters into the UBO block and upload in one call
        size_t mSize = shader->getMaterialUBOSize();
        if (mSize > 0) {
            std::vector<unsigned char> block(mSize);
            for (auto& kv : parameters) {
                const std::string& pname = kv.first;
                auto& param = kv.second;
                GLint off = shader->getUBOOffset(pname);
                if (off < 0) continue; // not part of material block

                switch (param.type) {
                    case ShaderParamType::Float:
                        memcpy(block.data() + off, param.data, sizeof(float));
                        break;
                    case ShaderParamType::Vec3:
                        memcpy(block.data() + off, param.data, sizeof(float) * 3);
                        break;
                    case ShaderParamType::Vec4:
                        memcpy(block.data() + off, param.data, sizeof(float) * 4);
                        break;
                    case ShaderParamType::Int:
                        memcpy(block.data() + off, param.data, sizeof(int));
                        break;
                    case ShaderParamType::Bool: {
                        int iv = (*(bool*)param.data) ? 1 : 0;
                        memcpy(block.data() + off, &iv, sizeof(int));
                        break;
                    }
                    case ShaderParamType::Matrix4:
                        memcpy(block.data() + off, param.data, sizeof(float) * 16);
                        break;
                    default:
                        // textures and unsupported types are ignored for UBO packing
                        break;
                }
            }
            shader->updateMaterialBlock(block.data(), block.size());
        } else {
            // No material UBO: fall back to setting uniforms directly (uniforms only, no UBO-by-name writes)
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
