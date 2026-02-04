# RenderBackendSandbox Module

## Overview

RenderBackendSandbox is a testing module that provides an isolated environment for implementing and testing DirectX 12 (DX12) and Vulkan rendering backend APIs. This sandbox allows developers to experiment with these graphics APIs before integrating them into the RHI (Render Hardware Interface) layer.

## Purpose

- **API Experimentation**: Test and learn DX12 and Vulkan APIs in isolation
- **Backend Validation**: Validate backend implementations before RHI integration
- **Rapid Prototyping**: Quickly prototype and test rendering features
- **Learning Environment**: Study modern graphics API concepts and patterns

## Architecture

```
RenderBackendSandbox/
├── Private/
│   ├── RenderBackendSandbox.cpp          # Application entry point
│   ├── RenderBackendSandboxModule.h      # Module interface
│   ├── RenderBackendSandboxModule.cpp    # Module implementation
│   ├── DX12Sandbox.h                     # DX12 testing interface
│   ├── DX12Sandbox.cpp                   # DX12 test implementation
│   ├── VulkanSandbox.h                   # Vulkan testing interface
│   └── VulkanSandbox.cpp                 # Vulkan test implementation
└── xmake.lua                             # Build configuration
```

## Features

### DX12 Sandbox
- Device initialization testing
- Command queue and command list creation
- Resource creation (buffers, textures)
- Pipeline state object (PSO) testing
- Descriptor heap management
- Swap chain creation and management

### Vulkan Sandbox
- Instance and device creation
- Physical device selection
- Logical device initialization
- Queue family management
- Command buffer creation
- Resource creation (buffers, images)
- Pipeline creation
- Swap chain management

## Building

Build the RenderBackendSandbox module:

```bash
# Build the module
xmake build RenderBackendSandbox

# Run the sandbox
xmake run RenderBackendSandbox
```

## Usage

The sandbox automatically runs all tests on startup. You can modify the source files to add your own tests:

1. Edit `DX12Sandbox.cpp` to add DX12-specific tests
2. Edit `VulkanSandbox.cpp` to add Vulkan-specific tests
3. Rebuild and run to see your test results

## Platform Support

- **Windows**: Full DX12 support + Vulkan support (with Vulkan SDK)
- **Linux**: Vulkan support (DX12 N/A)
- **macOS**: Metal support recommended (Vulkan via MoltenVK, DX12 N/A)

## Integration Roadmap

1. **Phase 1 (Current)**: Isolated testing environment with stub implementations
2. **Phase 2**: Implement actual DX12 and Vulkan API calls
3. **Phase 3**: Create abstraction interfaces for common patterns
4. **Phase 4**: Integrate tested backends into RHI module
5. **Phase 5**: Add backend selection at runtime

## Adding New Tests

To add a new test:

### For DX12:

1. Add a test method declaration in `DX12Sandbox.h`:
   ```cpp
   void TestMyNewFeature();
   ```

2. Implement the test in `DX12Sandbox.cpp`:
   ```cpp
   void DX12Sandbox::TestMyNewFeature()
   {
       LOG("DX12Sandbox: Test - My New Feature");
       
       // Your test implementation
       bool passed = true;
       
       LogTestResult("My New Feature", passed, "Description");
   }
   ```

3. Call the test in `RunTests()`:
   ```cpp
   void DX12Sandbox::RunTests()
   {
       // ... existing tests ...
       TestMyNewFeature();
   }
   ```

### For Vulkan:

Follow the same pattern in `VulkanSandbox.h` and `VulkanSandbox.cpp`.

## Dependencies

- **Core**: Core engine utilities and logging
- **Platform**: Platform abstraction layer
- **RHI**: RHI interfaces (for reference)
- **Launch**: Application launch framework

### External Dependencies

- **DirectX 12**: Windows SDK (Windows only)
- **Vulkan SDK**: Required for Vulkan tests
- **GLM**: Math library
- **GLFW**: Window management (if needed)

## Notes

- The current implementation uses stub tests to demonstrate the module structure
- Actual DX12 and Vulkan implementations should be added incrementally
- Each sandbox is independent - failures in one don't affect the other
- Tests can be run individually or all at once

## Future Enhancements

- [ ] Add actual DX12 API implementations
- [ ] Add actual Vulkan API implementations
- [ ] Add performance benchmarking
- [ ] Add API usage pattern examples
- [ ] Create visual test cases with rendering output
- [ ] Add headless rendering tests
- [ ] Integration with existing RHI interfaces
- [ ] Add automated test validation
- [ ] Support for Metal backend (macOS)

## Contributing

When adding new tests or features:

1. Follow the existing code structure and naming conventions
2. Add proper logging for test results
3. Document the purpose of each test
4. Keep tests focused and independent
5. Update this README with new features

## See Also

- [RHI Module Documentation](../Runtime/RHI/README.md)
- [CarrotToy Architecture](../../docs/MODULE_ARCHITECTURE.md)
- [Build Instructions](../../BUILD.md)
