# RHI Build and Testing Guide

## Prerequisites

Before building the RHI implementation, ensure you have:

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Xmake build system (or CMake)
- OpenGL 3.3+ support
- Required dependencies (managed by Xmake):
  - GLFW
  - GLAD
  - GLM
  - ImGui
  - STB

## Building with Xmake

```bash
# Clean previous build
xmake clean

# Configure and build
xmake

# Run the application
xmake run CarrotToy
```

## Building with CMake (if available)

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./CarrotToy
```

## Verifying RHI Implementation

The RHI implementation consists of the following files:

### Header Files
- `include/RHI.h` - Main RHI device interface
- `include/RHITypes.h` - Type definitions and enums
- `include/RHIResources.h` - Resource interfaces
- `include/OpenGLRHI.h` - OpenGL implementation

### Source Files
- `src/OpenGLRHI.cpp` - OpenGL implementation
- `src/RHIExample.cpp` - Example usage (demonstration only)

## Testing RHI Example

To test the RHI example code:

1. Uncomment the main function in `src/RHIExample.cpp`:
```cpp
int main() {
    CarrotToy::RHI::Example::demonstrateRHIUsage();
    return 0;
}
```

2. Modify `xmake.lua` to build the example:
```lua
target("RHIExample")
    set_kind("binary")
    add_files("src/RHIExample.cpp", "src/OpenGLRHI.cpp")
    add_headerfiles("include/*.h")
    add_includedirs("include")
    add_packages("glfw", "glad", "glm")
target_end()
```

3. Build and run:
```bash
xmake
xmake run RHIExample
```

## Integration Testing

To integrate RHI into existing code:

1. Include the RHI headers:
```cpp
#include "RHI.h"
#include "OpenGLRHI.h"
```

2. Create RHI device:
```cpp
using namespace CarrotToy::RHI;
auto rhiDevice = createRHIDevice(GraphicsAPI::OpenGL);
if (!rhiDevice || !rhiDevice->initialize()) {
    std::cerr << "Failed to initialize RHI" << std::endl;
    return false;
}
```

3. Replace OpenGL calls with RHI calls:
```cpp
// Before (direct OpenGL):
glViewport(0, 0, width, height);
glEnable(GL_DEPTH_TEST);
glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

// After (using RHI):
rhiDevice->setViewport(0, 0, width, height);
rhiDevice->setDepthTest(true);
rhiDevice->clearColor(0.2f, 0.2f, 0.2f, 1.0f);
```

## Syntax Validation

To validate syntax without full compilation:

```bash
# Check header syntax
g++ -std=c++17 -fsyntax-only include/RHI.h -I./include
g++ -std=c++17 -fsyntax-only include/RHITypes.h -I./include
g++ -std=c++17 -fsyntax-only include/RHIResources.h -I./include
g++ -std=c++17 -fsyntax-only include/OpenGLRHI.h -I./include

# Note: This will fail without GLAD headers, but validates structure
```

## Static Analysis

Run static analysis tools to check for issues:

```bash
# Using clang-tidy (if available)
clang-tidy src/OpenGLRHI.cpp -- -std=c++17 -I./include

# Using cppcheck (if available)
cppcheck --enable=all --std=c++17 -I./include src/OpenGLRHI.cpp
```

## Common Build Issues

### Issue: "glad/glad.h not found"
**Solution**: Ensure Xmake has installed the GLAD package:
```bash
xmake repo --update
xmake f --glad=y
```

### Issue: "OpenGL functions undefined"
**Solution**: Make sure GLAD is initialized before using RHI:
```cpp
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return false;
}
```

### Issue: "Multiple definition of createRHIDevice"
**Solution**: The factory function is defined in `OpenGLRHI.cpp`. Only link this file once.

## Performance Testing

To benchmark RHI overhead:

1. Create a simple test that draws many objects
2. Measure frame time with direct OpenGL
3. Measure frame time with RHI abstraction
4. Compare results (overhead should be < 1%)

Expected results:
- RHI overhead: Minimal (< 1% in most cases)
- Uniform location caching: Reduces overhead significantly
- Smart pointer overhead: Negligible with optimization enabled

## Memory Testing

Use Valgrind or AddressSanitizer to check for memory leaks:

```bash
# With Valgrind
valgrind --leak-check=full ./CarrotToy

# With AddressSanitizer
g++ -fsanitize=address -std=c++17 src/*.cpp -I./include -lGL -lglfw
./a.out
```

Expected results:
- No memory leaks
- All resources properly released
- No use-after-free errors

## Continuous Integration

Add to CI pipeline:

```yaml
- name: Build RHI
  run: |
    xmake
    
- name: Test RHI
  run: |
    xmake run CarrotToy --test-rhi
```

## Documentation

After successful build, verify documentation is up to date:
- [ ] RHI.md is comprehensive
- [ ] ARCHITECTURE.md includes RHI
- [ ] README.md mentions RHI
- [ ] Example code in RHIExample.cpp works

## Next Steps

After verifying the RHI implementation:

1. Consider refactoring existing code to use RHI
2. Implement Vulkan backend for comparison
3. Add unit tests for each RHI interface
4. Benchmark performance vs direct OpenGL
5. Add more example usage patterns

## Support

For issues or questions:
- Check RHI.md for detailed documentation
- Review RHIExample.cpp for usage patterns
- Consult ARCHITECTURE.md for design details
- Open an issue on GitHub
