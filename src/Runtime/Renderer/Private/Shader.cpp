#include "Shader.h"
#include "CoreUtils.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <cstring>

namespace CarrotToy {

// Helper to check file extension
static bool hasExtension(const std::string& path, const std::string& ext) {
    if (path.length() < ext.length()) return false;
    std::string p = path.substr(path.length() - ext.length());
    std::transform(p.begin(), p.end(), p.begin(), ::tolower);
    return p == ext;
}

// Shader implementation
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : vertexPath(vertexPath), fragmentPath(fragmentPath) {
}

Shader::~Shader() {
    if (shaderProgram && shaderProgram->isValid()) {
        // Remove program cache entry
        uintptr_t programID = shaderProgram->getNativeHandle();
        if (g_ProgramUBOs.count(programID)) {
            g_ProgramUBOs.erase(programID);
        }
    }
}

void Shader::use() {
    if (shaderProgram && shaderProgram->isValid()) {
        shaderProgram->bind();
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
    // Determine shader format based on file extension
    RHI::ShaderSourceFormat vFormat = hasExtension(vertexPath, ".spv") 
        ? RHI::ShaderSourceFormat::SPIRV 
        : RHI::ShaderSourceFormat::GLSL;
    RHI::ShaderSourceFormat fFormat = hasExtension(fragmentPath, ".spv") 
        ? RHI::ShaderSourceFormat::SPIRV 
        : RHI::ShaderSourceFormat::GLSL;
    
    // Compile vertex shader
    if (!compileShader(vertexShader, RHI::ShaderType::Vertex, vertexSource, vFormat)) {
        return false;
    }
    
    // Compile fragment shader
    if (!compileShader(fragmentShader, RHI::ShaderType::Fragment, fragmentSource, fFormat)) {
        return false;
    }
    
    return true;
}

bool Shader::compileShader(std::shared_ptr<RHI::IRHIShader>& shader, 
                           RHI::ShaderType type, 
                           const std::string& source,
                           RHI::ShaderSourceFormat format) {
    auto rhiDev = RHI::getGlobalDevice();
    if (!rhiDev) {
        std::cerr << "No global RHI device available!" << std::endl;
        return false;
    }
    
    // Create shader descriptor
    RHI::ShaderDesc desc;
    desc.type = type;
    desc.source = source.data();
    desc.sourceSize = source.size();
    desc.format = format;
    
    // Set entry point for SPIR-V shaders
    if (format == RHI::ShaderSourceFormat::SPIRV) {
        desc.entryPoint = (type == RHI::ShaderType::Vertex) ? "VSMain" : "PSMain";
    }
    
    // Create and compile shader
    shader = rhiDev->createShader(desc);
    if (!shader || !shader->isValid()) {
        std::cerr << "Failed to create shader!" << std::endl;
        return false;
    }
    
    if (!shader->compile()) {
        std::cerr << "Shader compilation failed: " << shader->getCompileErrors() << std::endl;
        return false;
    }
    
    return true;
}

bool Shader::linkProgram() {
    auto rhiDev = RHI::getGlobalDevice();
    if (!rhiDev) {
        std::cerr << "No global RHI device available!" << std::endl;
        return false;
    }
    
    if (!vertexShader || !fragmentShader) {
        std::cerr << "Shaders not compiled!" << std::endl;
        return false;
    }
    
    // Create shader program
    auto newProgram = rhiDev->createShaderProgram();
    if (!newProgram || !newProgram->isValid()) {
        std::cerr << "Failed to create shader program!" << std::endl;
        return false;
    }
    
    // Attach shaders
    newProgram->attachShader(vertexShader.get());
    newProgram->attachShader(fragmentShader.get());
    
    // Link program
    if (!newProgram->link()) {
        std::cerr << "Shader linking failed: " << newProgram->getLinkErrors() << std::endl;
        return false;
    }
    
    // --- UBO automatic setup using RHI reflection ---
    ProgramUBOCache cache;
    auto uniformBlocks = newProgram->getUniformBlocks();
    std::set<uint32_t> usedBindings;
    
    // Map to store block index -> UBO handle
    std::map<uint32_t, uintptr_t> blockUBOs;
    
    for (const auto& block : uniformBlocks) {
        uint32_t bindingPoint = block.binding;
        
        // If binding is already used, find a free one
        if (usedBindings.count(bindingPoint)) {
            bindingPoint = 0;
            while (usedBindings.count(bindingPoint)) ++bindingPoint;
        }
        usedBindings.insert(bindingPoint);
        
        // Create UBO via RHI
        if (block.size > 0) {
            auto rhiUB = rhiDev->createUniformBuffer(block.size, bindingPoint);
            if (!rhiUB) {
                std::cerr << "RHI failed to create UBO for block '" << block.name 
                          << "' (binding " << bindingPoint << ")" << std::endl;
            } else {
                // Categorize UBOs based on name
                if (block.name.find("PerFrame") != std::string::npos) {
                    perFrameUBO = rhiUB;
                    perFrameUBOSize = block.size;
                } else if (block.name.find("Light") != std::string::npos) {
                    lightUBO = rhiUB;
                    lightUBOSize = block.size;
                } else if (block.name.find("Material") != std::string::npos) {
                    materialUBO = rhiUB;
                    materialUBOSize = block.size;
                }
                
                // Store native handle in cache
                uintptr_t native = rhiUB->getNativeHandle();
                if (cache.uboIDs.size() <= bindingPoint) {
                    cache.uboIDs.resize(bindingPoint + 1);
                }
                cache.uboIDs[bindingPoint] = native;
                blockUBOs[block.blockIndex] = native;
            }
        }
        
        std::cout << "Initialized UBO: " << block.name 
                  << " (BlockIndex: " << block.blockIndex 
                  << " -> Binding: " << bindingPoint 
                  << ", Size: " << block.size << ")" << std::endl;
    }
    
    // Cache uniform variable offsets
    auto uniformVars = newProgram->getUniformVariables();
    for (const auto& var : uniformVars) {
        if (var.offset < 0) continue; // Skip non-block uniforms
        
        uintptr_t uboID = 0;
        if (blockUBOs.count(var.blockIndex)) {
            uboID = blockUBOs[var.blockIndex];
        }
        
        auto store = [&](const std::string& key) {
            cache.vars[key] = {uboID, var.offset};
        };
        
        store(var.name);
        
        // Store short name variants (for compatibility)
        size_t arr = var.name.find('[');
        if (arr != std::string::npos) {
            store(var.name.substr(0, arr));
        }
        
        size_t lastDot = var.name.find_last_of('.');
        if (lastDot != std::string::npos) {
            std::string shortName = var.name.substr(lastDot + 1);
            size_t sArr = shortName.find('[');
            if (sArr != std::string::npos) {
                shortName = shortName.substr(0, sArr);
            }
            store(shortName);
        }
    }
    
    // Store cache for this program
    uintptr_t programID = newProgram->getNativeHandle();
    if (!cache.uboIDs.empty()) {
        std::cout << "Caching UBO vars for program " << programID << ":\n";
        for (const auto& kv : cache.vars) {
            std::cout << "  - " << kv.first 
                      << " (UBO: " << kv.second.uboID 
                      << ", offset: " << kv.second.offset << ")\n";
        }
        g_ProgramUBOs[programID] = cache;
    }
    
    // Delete old program if exists
    if (shaderProgram && shaderProgram->isValid()) {
        uintptr_t oldID = shaderProgram->getNativeHandle();
        if (g_ProgramUBOs.count(oldID)) {
            g_ProgramUBOs.erase(oldID);
        }
    }
    
    shaderProgram = newProgram;
    linked = true;
    return true;
}

uintptr_t Shader::getID() const {
    if (shaderProgram && shaderProgram->isValid()) {
        return shaderProgram->getNativeHandle();
    }
    return 0;
}

// Uniform setters using RHI
void Shader::setFloat(const std::string& name, float value) {
    if (shaderProgram && shaderProgram->isValid()) {
        shaderProgram->setUniformFloat(name, value);
    }
}

void Shader::setVec2(const std::string& name, float x, float y) {
    if (shaderProgram && shaderProgram->isValid()) {
        shaderProgram->setUniformVec2(name, x, y);
    }
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    if (shaderProgram && shaderProgram->isValid()) {
        shaderProgram->setUniformVec3(name, x, y, z);
    }
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
    if (shaderProgram && shaderProgram->isValid()) {
        shaderProgram->setUniformVec4(name, x, y, z, w);
    }
}

void Shader::setInt(const std::string& name, int value) {
    if (shaderProgram && shaderProgram->isValid()) {
        shaderProgram->setUniformInt(name, value);
    }
}

void Shader::setBool(const std::string& name, bool value) {
    if (shaderProgram && shaderProgram->isValid()) {
        shaderProgram->setUniformBool(name, value);
    }
}

void Shader::setMatrix4(const std::string& name, const float* value) {
    if (shaderProgram && shaderProgram->isValid()) {
        shaderProgram->setUniformMatrix4(name, value);
    }
}

void Shader::setPerFrameMatrices(const float* model, const float* view, const float* projection) {
    // If we have a typed RHI-backed PerFrame UBO, assemble and update it
    if (perFrameUBO && perFrameUBO->isValid() && perFrameUBOSize > 0) {
        std::vector<unsigned char> block(perFrameUBOSize);
        uintptr_t programID = getID();
        
        if (g_ProgramUBOs.count(programID)) {
            auto& cache = g_ProgramUBOs[programID];
            auto findOffset = [&](const std::string& field) -> int32_t {
                // exact match
                auto it = cache.vars.find(field);
                if (it != cache.vars.end()) return it->second.offset;
                
                // .suffix match
                std::string dot = std::string(".") + field;
                for (const auto& kv : cache.vars) {
                    if (kv.first.size() > dot.size() && 
                        kv.first.compare(kv.first.size() - dot.size(), dot.size(), dot) == 0) {
                        return kv.second.offset;
                    }
                }
                
                // ends with field
                for (const auto& kv : cache.vars) {
                    if (kv.first.size() >= field.size() && 
                        kv.first.compare(kv.first.size() - field.size(), field.size(), field) == 0) {
                        return kv.second.offset;
                    }
                }
                return -1;
            };

            int32_t offModel = findOffset("model");
            if (offModel >= 0) memcpy(block.data() + offModel, model, sizeof(float) * 16);
            
            int32_t offView = findOffset("view");
            if (offView >= 0) memcpy(block.data() + offView, view, sizeof(float) * 16);
            
            int32_t offProj = findOffset("projection");
            if (offProj >= 0) memcpy(block.data() + offProj, projection, sizeof(float) * 16);
        } else {
            // Best-effort contiguous layout
            memcpy(block.data(), model, sizeof(float) * 16);
            memcpy(block.data() + sizeof(float) * 16, view, sizeof(float) * 16);
            memcpy(block.data() + sizeof(float) * 32, projection, sizeof(float) * 16);
        }

        perFrameUBO->update(block.data(), block.size(), 0);
        return;
    }

    // Fallback to direct uniforms
    setMatrix4("model", model);
    setMatrix4("view", view);
    setMatrix4("projection", projection);
}

void Shader::setLightData(const float* lightPos, const float* lightColor, const float* viewPos) {
    if (lightUBO && lightUBO->isValid() && lightUBOSize > 0) {
        std::vector<unsigned char> block(lightUBOSize);
        uintptr_t programID = getID();
        
        if (g_ProgramUBOs.count(programID)) {
            auto& cache = g_ProgramUBOs[programID];
            auto findOffset = [&](const std::string& field) -> int32_t {
                auto it = cache.vars.find(field);
                if (it != cache.vars.end()) return it->second.offset;
                
                std::string dot = std::string(".") + field;
                for (const auto& kv : cache.vars) {
                    if (kv.first.size() > dot.size() && 
                        kv.first.compare(kv.first.size() - dot.size(), dot.size(), dot) == 0) {
                        return kv.second.offset;
                    }
                }
                
                for (const auto& kv : cache.vars) {
                    if (kv.first.size() >= field.size() && 
                        kv.first.compare(kv.first.size() - field.size(), field.size(), field) == 0) {
                        return kv.second.offset;
                    }
                }
                return -1;
            };

            int32_t offLP = findOffset("lightPos");
            if (offLP >= 0) memcpy(block.data() + offLP, lightPos, sizeof(float) * 3);
            
            int32_t offLC = findOffset("lightColor");
            if (offLC >= 0) memcpy(block.data() + offLC, lightColor, sizeof(float) * 3);
            
            int32_t offVP = findOffset("viewPos");
            if (offVP >= 0) memcpy(block.data() + offVP, viewPos, sizeof(float) * 3);
        } else {
            memcpy(block.data(), lightPos, sizeof(float) * 3);
            memcpy(block.data() + sizeof(float) * 4, lightColor, sizeof(float) * 3);
            memcpy(block.data() + sizeof(float) * 8, viewPos, sizeof(float) * 3);
        }

        lightUBO->update(block.data(), block.size(), 0);
        return;
    }

    // Fallback to direct uniforms
    setVec3("lightPos", lightPos[0], lightPos[1], lightPos[2]);
    setVec3("lightColor", lightColor[0], lightColor[1], lightColor[2]);
    setVec3("viewPos", viewPos[0], viewPos[1], viewPos[2]);
}

void Shader::updateMaterialBlock(const void* data, size_t size) {
    if (materialUBO && materialUBO->isValid() && materialUBOSize >= size) {
        materialUBO->update(data, size, 0);
    } else {
        LOG("updateMaterialBlock: no material UBO available for program " << getID());
    }
}

int32_t Shader::getUBOOffset(const std::string& field) const {
    uintptr_t programID = getID();
    if (!g_ProgramUBOs.count(programID)) return -1;
    
    const auto& cache = g_ProgramUBOs.at(programID);
    
    // exact match
    auto it = cache.vars.find(field);
    if (it != cache.vars.end()) return it->second.offset;
    
    // dot-suffix match
    std::string dot = std::string(".") + field;
    for (const auto& kv : cache.vars) {
        if (kv.first.size() > dot.size() && 
            kv.first.compare(kv.first.size() - dot.size(), dot.size(), dot) == 0) {
            return kv.second.offset;
        }
    }
    
    // ends-with match
    for (const auto& kv : cache.vars) {
        if (kv.first.size() >= field.size() && 
            kv.first.compare(kv.first.size() - field.size(), field.size(), field) == 0) {
            return kv.second.offset;
        }
    }
    
    return -1;
}

} // namespace CarrotToy
