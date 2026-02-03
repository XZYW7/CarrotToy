# Shader-RHI Decoupling Refactoring

## Overview
This document describes the refactoring that decoupled the Shader class from direct GLAD/OpenGL calls and moved all graphics operations to the RHI (Render Hardware Interface) abstraction layer.

## Problem Statement
The original issue (in Chinese) stated:
> "Shader好像还没和glad解耦开，相关内容是不是应该用RHI管理"
> Translation: "Shader seems not yet decoupled from glad, shouldn't the related content be managed by RHI"

**Before Refactoring:**
- `Shader.cpp` contained ~51 direct OpenGL/GLAD calls
- Shader class bypassed the RHI abstraction layer
- Code was tightly coupled to OpenGL, making it difficult to support other graphics APIs
- Inconsistent architecture: some parts used RHI (UBOs), others used GLAD directly

## Changes Made

### 1. Extended RHI Interfaces

#### New Types in `RHITypes.h`:
```cpp
enum class ShaderSourceFormat {
    GLSL,       // GLSL source code
    SPIRV,      // SPIR-V binary
    HLSL        // HLSL source code (for future support)
};

struct UniformBlockInfo {
    std::string name;
    uint32_t binding;
    uint32_t size;
    uint32_t blockIndex;
};

struct UniformVariableInfo {
    std::string name;
    uint32_t blockIndex;
    int32_t offset;
    uint32_t size;
};
```

#### Extended `ShaderDesc`:
- Added `ShaderSourceFormat format` field
- Added `const char* entryPoint` field for SPIR-V shaders

#### Extended `IRHIShaderProgram`:
- Added `std::vector<UniformBlockInfo> getUniformBlocks() const`
- Added `std::vector<UniformVariableInfo> getUniformVariables() const`
- Added `uintptr_t getNativeHandle() const` for compatibility

### 2. Updated OpenGL RHI Implementation

#### `OpenGLShader`:
- Added support for SPIR-V compilation in `compile()` method
- Stores shader format and entry point
- Handles both GLSL and SPIR-V shader sources

#### `OpenGLShaderProgram`:
- Implemented `getUniformBlocks()` - queries OpenGL for uniform block information
- Implemented `getUniformVariables()` - queries OpenGL for uniform variable information
- Implemented `getNativeHandle()` - returns OpenGL program ID

### 3. Refactored Shader Class

#### Changes in `Shader.h`:
- **Removed**: `#include <glad/glad.h>`
- **Removed**: Direct GL types (`GLuint`, `GLint`)
- **Replaced**: Raw shader/program IDs with RHI object references
  - `unsigned int programID` → `std::shared_ptr<RHI::IRHIShaderProgram> shaderProgram`
  - `int vertexShaderID` → `std::shared_ptr<RHI::IRHIShader> vertexShader`
  - `int fragmentShaderID` → `std::shared_ptr<RHI::IRHIShader> fragmentShader`
- **Updated**: Return types to be API-agnostic (`uintptr_t` instead of `unsigned int`)

#### Changes in `Shader.cpp`:
- **Removed**: All 51 direct OpenGL/GLAD calls
- **Replaced**: Shader compilation with `rhiDev->createShader()` and `shader->compile()`
- **Replaced**: Program linking with `rhiDev->createShaderProgram()` and `program->link()`
- **Replaced**: Uniform setting with RHI program methods (`setUniformFloat()`, etc.)
- **Replaced**: Shader reflection with `program->getUniformBlocks()` and `getUniformVariables()`
- **Maintained**: All public API signatures for backward compatibility
- **Preserved**: SPIR-V support through RHI

## Architecture Improvements

### Before:
```
┌─────────────────┐
│  Shader Class   │ ──────┐
└─────────────────┘       │
                          ├─> Direct GLAD/OpenGL calls
┌─────────────────┐       │
│   RHI Layer     │ ──────┘
└─────────────────┘
```

### After:
```
┌─────────────────┐
│  Shader Class   │
└─────────────────┘
        │
        ▼
┌─────────────────┐
│   RHI Layer     │
└─────────────────┘
        │
        ▼
┌─────────────────┐
│  GLAD/OpenGL    │
└─────────────────┘
```

## Benefits

1. **Complete Decoupling**: Zero direct OpenGL calls in Shader class
2. **API Agnostic**: Easier to add Vulkan, DirectX, or Metal backends
3. **Consistent Architecture**: All graphics operations now go through RHI
4. **Better Maintainability**: Clear separation of concerns
5. **Testability**: Easier to mock RHI for testing
6. **Backward Compatibility**: Existing code using Shader class works unchanged

## Backward Compatibility

All public APIs were maintained:
- `use()`, `reload()`, `compile()` - same signatures
- Uniform setters - same signatures
- `setPerFrameMatrices()`, `setLightData()` - same signatures
- `getID()` - now returns `uintptr_t` instead of `unsigned int` (compatible)

## Migration Notes for Future Developers

### Adding Support for New Graphics APIs:

1. Implement `IRHIShader` for your API (e.g., `VulkanShader`)
2. Implement `IRHIShaderProgram` for your API (e.g., `VulkanShaderProgram`)
3. Implement reflection methods (`getUniformBlocks()`, `getUniformVariables()`)
4. No changes needed in `Shader.cpp` - it will automatically work!

### Using SPIR-V Shaders:

```cpp
// The Shader class automatically detects .spv files
auto shader = std::make_shared<Shader>(
    "shaders/vertex.spv",   // SPIR-V binary
    "shaders/fragment.spv"  // SPIR-V binary
);
shader->reload();
shader->linkProgram();
```

## Testing Recommendations

1. Test with GLSL shaders (.vs, .ps, .glsl)
2. Test with SPIR-V shaders (.spv)
3. Test shader hot-reloading
4. Test uniform buffer object (UBO) management
5. Test reflection with different uniform block layouts
6. Verify all existing applications still work (DefaultGame, TestRHIApp, etc.)

## Files Modified

1. `src/Runtime/RHI/Public/RHI/RHITypes.h` - Extended types
2. `src/Runtime/RHI/Public/RHI/RHIResources.h` - Extended interfaces
3. `src/Runtime/RHI/Public/RHI/OpenGLRHI.h` - Updated declarations
4. `src/Runtime/RHI/Private/OpenGLRHI.cpp` - Implemented new features
5. `src/Runtime/Renderer/Public/Shader.h` - Removed GLAD dependency
6. `src/Runtime/Renderer/Private/Shader.cpp` - Complete refactoring

## Code Review Feedback Addressed

1. Fixed variable shadowing in `OpenGLRHI.cpp` (renamed `format` to `binaryFormat`)
2. Improved uniform block validation to prevent invalid map lookups
3. Added better comments explaining the uniform variable filtering logic

## Conclusion

This refactoring successfully achieved the goal of decoupling the Shader class from GLAD and managing all shader-related operations through the RHI abstraction layer. The codebase is now more maintainable, testable, and prepared for multi-API support.
