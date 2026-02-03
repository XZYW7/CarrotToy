#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include "RHI/RHI.h"
#include "RendererAPI.h"

namespace CarrotToy {

// --- UBO 缓存结构 (now using RHI types) ---
struct UBOVarLocation {
    uintptr_t uboID;  // RHI native handle
    int32_t offset;
};

struct ProgramUBOCache {
    std::vector<uintptr_t> uboIDs; // All UBO Handles for cleanup
    std::map<std::string, UBOVarLocation> vars; // Variable name -> {UBO ID, Offset}
};

static std::map<uintptr_t, ProgramUBOCache> g_ProgramUBOs;

// Shader class - manages shader compilation and hot-reloading via RHI
class RENDERER_API Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    
    void use();
    void reload();
    bool compile(const std::string& vertexSource, const std::string& fragmentSource);
    
    uintptr_t getID() const;
    
    // Uniform setters
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, float x, float y);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setInt(const std::string& name, int value);
    void setBool(const std::string& name, bool value);
    void setMatrix4(const std::string& name, const float* value);
    
    // High-level convenience uploads
    void setPerFrameMatrices(const float* model, const float* view, const float* projection);
    void setLightData(const float* lightPos, const float* lightColor, const float* viewPos);
    
    // Material block helpers
    size_t getMaterialUBOSize() const { return materialUBOSize; }
    void updateMaterialBlock(const void* data, size_t size);
    
    // Query cached UBO offset (from reflection) for a given field name
    int32_t getUBOOffset(const std::string& field) const;

    std::string getVertexPath() const { return vertexPath; }
    std::string getFragmentPath() const { return fragmentPath; }
    
    bool linkProgram();
    
private:
    bool linked = false;
    std::string vertexPath;
    std::string fragmentPath;
    
    // RHI shader objects
    std::shared_ptr<CarrotToy::RHI::IRHIShader> vertexShader;
    std::shared_ptr<CarrotToy::RHI::IRHIShader> fragmentShader;
    std::shared_ptr<CarrotToy::RHI::IRHIShaderProgram> shaderProgram;
    
    // RHI-backed uniform buffers
    std::shared_ptr<CarrotToy::RHI::IRHIUniformBuffer> perFrameUBO;
    std::shared_ptr<CarrotToy::RHI::IRHIUniformBuffer> lightUBO;
    std::shared_ptr<CarrotToy::RHI::IRHIUniformBuffer> materialUBO;
    size_t perFrameUBOSize = 0;
    size_t lightUBOSize = 0;
    size_t materialUBOSize = 0;
    
    bool compileShader(std::shared_ptr<CarrotToy::RHI::IRHIShader>& shader, 
                       CarrotToy::RHI::ShaderType type, 
                       const std::string& source,
                       CarrotToy::RHI::ShaderSourceFormat format);
};

}

