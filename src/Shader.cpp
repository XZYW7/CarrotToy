#include "Shader.h"
#include "CoreUtils.h"
#include <fstream>
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

    if (!GLAD_GL_ARB_gl_spirv && !GLAD_GL_VERSION_4_6) {
        std::cerr << "GL_ARB_gl_spirv not supported!" << std::endl;
        return false;
    }

    shader = glCreateShader(type);

#ifdef GL_SHADER_BINARY_FORMAT_SPIR_V
    GLenum format = GL_SHADER_BINARY_FORMAT_SPIR_V;
#elif defined(GL_SHADER_BINARY_FORMAT_SPIR_V_ARB)
    GLenum format = GL_SHADER_BINARY_FORMAT_SPIR_V_ARB;
#else
    GLenum format = 0x9551; // Fallback constant
#endif

    if (glShaderBinary) {
        glShaderBinary(1, &shader, format, source.data(), (GLsizei)source.size());
        LOG("Loaded SPIR-V shader binary, size: " << source.size());
    } else {
        std::cerr << "glShaderBinary not available at runtime; cannot load SPIR-V binary." << std::endl;
        return false;
    }

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
        if (g_ProgramUBOs.count(programID)) {
            auto& ids = g_ProgramUBOs[programID].uboIDs;
            if (!ids.empty()) {
                glDeleteBuffers((GLsizei)ids.size(), ids.data());
            }
            g_ProgramUBOs.erase(programID);
        }
        glDeleteProgram(programID);
    }
}

void Shader::use() {
    if (programID != 0) {
        glUseProgram(programID);

        // Restore UBO bindings
        if (g_ProgramUBOs.count(programID)) {
            const auto& ids = g_ProgramUBOs[programID].uboIDs;
            for (size_t i = 0; i < ids.size(); ++i) {
                glBindBufferBase(GL_UNIFORM_BUFFER, (GLuint)i, ids[i]);
            }
        }
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

    // --- UBO automatic setup: create one UBO per active uniform block and cache variable offsets ---
    ProgramUBOCache cache;
    GLint numBlocks = 0;
    glGetProgramiv(newProgramID, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);

    // Keep a mapping blockIndex -> ubo handle so we can later assign variable offsets
    std::vector<GLuint> blockUBOs(numBlocks, 0);
    std::set<GLuint> usedBindings;

    for (GLint blockIndex = 0; blockIndex < numBlocks; ++blockIndex) {
        char blockName[256] = {0};
        glGetActiveUniformBlockName(newProgramID, blockIndex, sizeof(blockName), nullptr, blockName);

        // Prefer a declared binding (e.g. HLSL register(b#)) if present and free.
        GLint declaredBinding = -1;
        glGetActiveUniformBlockiv(newProgramID, blockIndex, GL_UNIFORM_BLOCK_BINDING, &declaredBinding);

        GLuint bindingPoint = 0;
        if (declaredBinding >= 0 && usedBindings.count((GLuint)declaredBinding) == 0) {
            bindingPoint = (GLuint)declaredBinding;
        } else {
            // find the lowest unused binding
            while (usedBindings.count(bindingPoint)) ++bindingPoint;
        }
        usedBindings.insert(bindingPoint);

        // Bind the block to the chosen binding point for this program
        glUniformBlockBinding(newProgramID, blockIndex, (GLint)bindingPoint);

        // Allocate a UBO sized for the block and bind it to the binding point
        GLint blockSize = 0;
        glGetActiveUniformBlockiv(newProgramID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

        GLuint ubo = 0;
        if (blockSize > 0) {
            glGenBuffers(1, &ubo);
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);

            if (cache.uboIDs.size() <= bindingPoint) cache.uboIDs.resize(bindingPoint + 1);
            cache.uboIDs[bindingPoint] = ubo;
        }
        blockUBOs[blockIndex] = ubo;

        std::cout << "Initialized UBO: " << blockName << " (BlockIndex: " << blockIndex << " -> Binding: " << bindingPoint << ", Size: " << blockSize << ")" << std::endl;
    }

    // Walk all active uniforms once and cache offsets for those that belong to a UBO
    GLint numUniforms = 0;
    glGetProgramiv(newProgramID, GL_ACTIVE_UNIFORMS, &numUniforms);

    for (GLint uni = 0; uni < numUniforms; ++uni) {
        char nameBuf[256] = {0};
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveUniform(newProgramID, uni, sizeof(nameBuf), &length, &size, &type, nameBuf);
        std::string fullName(nameBuf, length);
        LOG("Active uniform: " << fullName << " (Type: " << type << ", Size: " << size << ")");
        GLint blockIndex = -1;
        glGetActiveUniformsiv(newProgramID, 1, (const GLuint*)&uni, GL_UNIFORM_BLOCK_INDEX, &blockIndex);
        if (blockIndex < 0 || blockIndex >= numBlocks) continue;

        GLint offset = -1;
        glGetActiveUniformsiv(newProgramID, 1, (const GLuint*)&uni, GL_UNIFORM_OFFSET, &offset);
        if (offset < 0) continue;

        GLuint uboID = blockUBOs[blockIndex];
        if (uboID == 0) continue; // no buffer allocated for this block

        auto store = [&](const std::string& key){ cache.vars[key] = {uboID, offset}; };
        store(fullName);
        
        // TODO : Remove unsafe name based cache, use Structured data like UE GlobalShader
        // name without array suffix: "foo[0]" -> "foo"
        size_t arr = fullName.find('[');
        if (arr != std::string::npos) store(fullName.substr(0, arr));

        // short name after last '.' (handles decorated HLSL/SPIRV names)
        size_t lastDot = fullName.find_last_of('.');
        if (lastDot != std::string::npos) {
            std::string shortName = fullName.substr(lastDot + 1);
            size_t sArr = shortName.find('[');
            if (sArr != std::string::npos) shortName = shortName.substr(0, sArr);
            store(shortName);
        }
    }

    if (!cache.uboIDs.empty()) {
        std::cout << "Caching UBO vars for program " << newProgramID << ":\n";
        for (const auto& kv : cache.vars) {
            std::cout << "  - " << kv.first << " (UBO: " << kv.second.uboID << ", offset: " << kv.second.offset << ")\n";
        }
        g_ProgramUBOs[newProgramID] = cache;
    }

    // Delete old program if exists
    if (programID != 0) {
        if (g_ProgramUBOs.count(programID)) {
            auto& ids = g_ProgramUBOs[programID].uboIDs;
            if (!ids.empty()) {
                glDeleteBuffers((GLsizei)ids.size(), ids.data());
            }
            g_ProgramUBOs.erase(programID);
        }
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

// Helper macro for UBO fallback
#define TRY_SET_UBO(name, dataPtr, dataSize) \
    if (g_ProgramUBOs.count(programID)) { \
        auto& cache = g_ProgramUBOs[programID]; \
        if (cache.vars.count(name)) { \
            auto& var = cache.vars[name]; \
            glBindBuffer(GL_UNIFORM_BUFFER, var.uboID); \
            glBufferSubData(GL_UNIFORM_BUFFER, var.offset, dataSize, dataPtr); \
            glBindBuffer(GL_UNIFORM_BUFFER, 0); \
            return; \
        } \
    }

void Shader::setFloat(const std::string& name, float value) {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1) { glUniform1f(loc, value); return; }
    TRY_SET_UBO(name, &value, sizeof(float));
}

void Shader::setVec2(const std::string& name, float x, float y) {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1) { glUniform2f(loc, x, y); return; }
    float data[2] = {x, y};
    TRY_SET_UBO(name, data, sizeof(data));
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1) { glUniform3f(loc, x, y, z); return; }
    float data[3] = {x, y, z};
    TRY_SET_UBO(name, data, sizeof(data));
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1) { glUniform4f(loc, x, y, z, w); return; }
    float data[4] = {x, y, z, w};
    TRY_SET_UBO(name, data, sizeof(data));
}

void Shader::setInt(const std::string& name, int value) {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1) { glUniform1i(loc, value); return; }
    TRY_SET_UBO(name, &value, sizeof(int));
}

void Shader::setBool(const std::string& name, bool value) {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1) { glUniform1i(loc, value ? 1 : 0); return; }
    // HLSL bool in cbuffer is 4 bytes (int)
    int data = value ? 1 : 0;
    TRY_SET_UBO(name, &data, sizeof(int));
}

void Shader::setMatrix4(const std::string& name, const float* value) {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1) { glUniformMatrix4fv(loc, 1, GL_FALSE, value); return; }
    TRY_SET_UBO(name, value, sizeof(float) * 16);
}

} // namespace CarrotToy