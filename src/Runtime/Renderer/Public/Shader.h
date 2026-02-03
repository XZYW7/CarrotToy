#pragma once

#include <string>
#include <glad/glad.h>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include "RHI/RHI.h"
#include "RendererAPI.h"
namespace CarrotToy {
// --- 通用 UBO 缓存结构 ---
// TODO：Support RHI abstraction layer
struct UBOVarLocation {
    GLuint uboID;
    GLint offset;
};

struct ProgramUBOCache {
    std::vector<GLuint> uboIDs; // 所有的 UBO Handle，用于析构时删除
    std::map<std::string, UBOVarLocation> vars; // 变量名 -> {UBO ID, Offset}
};

static std::map<GLuint, ProgramUBOCache> g_ProgramUBOs;

// Shader class - manages shader compilation and hot-reloading
class RENDERER_API Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    
    void use();
    void reload();
    bool compile(const std::string& vertexSource, const std::string& fragmentSource);
    
    unsigned int getID() const { return programID; }
    
    // Uniform setters
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, float x, float y);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setInt(const std::string& name, int value);
    void setBool(const std::string& name, bool value);
    void setMatrix4(const std::string& name, const float* value);
    // High-level convenience uploads (typed, avoid relying on string names everywhere)
    void setPerFrameMatrices(const float* model, const float* view, const float* projection);
    void setLightData(const float* lightPos, const float* lightColor, const float* viewPos);
    // Material block helpers
    size_t getMaterialUBOSize() const { return materialUBOSize; }
    void updateMaterialBlock(const void* data, size_t size);
    // Query existing cached UBO offset (from reflection) for a given field name
    GLint getUBOOffset(const std::string& field) const;

    std::string getVertexPath() const { return vertexPath; }
    std::string getFragmentPath() const { return fragmentPath; }
    int getVertexShaderID() const { return vertexShaderID; }
    int getFragmentShaderID() const { return fragmentShaderID; }
    bool linkProgram();
private:
    bool linked = false;
    unsigned int programID;
    std::string vertexPath;
    std::string fragmentPath;
    // Optional RHI-backed uniform buffers for faster/typed uploads
    std::shared_ptr<CarrotToy::RHI::IRHIUniformBuffer> perFrameUBO;
    std::shared_ptr<CarrotToy::RHI::IRHIUniformBuffer> lightUBO;
    size_t perFrameUBOSize = 0;
    size_t lightUBOSize = 0;
    std::shared_ptr<CarrotToy::RHI::IRHIUniformBuffer> materialUBO;
    size_t materialUBOSize = 0;
    
    bool compileShader(unsigned int& shader, int type, const std::string& source);
    
    bool linkProgram(unsigned int vertex, unsigned int fragment);
    int vertexShaderID = 0;
    int fragmentShaderID = 0;
};
}
