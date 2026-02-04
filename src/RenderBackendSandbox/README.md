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
│   ├── BasicTests.h                      # Basic concrete tests (NEW)
│   ├── BasicTests.cpp                    # Basic test implementation (NEW)
│   ├── DX12Sandbox.h                     # DX12 testing interface
│   ├── DX12Sandbox.cpp                   # DX12 test implementation
│   ├── VulkanSandbox.h                   # Vulkan testing interface
│   └── VulkanSandbox.cpp                 # Vulkan test implementation
└── xmake.lua                             # Build configuration
```

## Features

### Basic Tests (Concrete Implementations) ✨ NEW
- **Memory Allocation**: Real buffer allocation and validation (1KB, 1MB buffers)
- **String Operations**: String concatenation, length, comparison, substring
- **Math Operations**: Arithmetic, floating point, math functions, min/max
- **Platform Detection**: OS detection, architecture (32/64-bit), endianness
- **Buffer Operations**: memset, memcpy with pattern validation

All BasicTests include actual validation logic (not stubs) and run on all platforms.

### DX12 Sandbox ✨ WITH REAL API IMPLEMENTATION
- **Device Initialization**: ✅ **IMPLEMENTED** - Real DX12 API test
  - Creates DXGI factory
  - Enumerates graphics adapters
  - Gets adapter information (GPU name, VRAM)
  - Creates D3D12 device with feature level detection
  - Properly releases device on shutdown
- Command queue and command list creation (stub implementation)
- Resource creation (buffers, textures) (stub implementation)
- Pipeline state object (PSO) testing (stub implementation)
- Descriptor heap management (stub implementation)
- Swap chain creation and management (stub implementation)

**Note:** DX12 device initialization test includes real DX12 API calls (Windows only). Other tests are stub implementations. Install Windows SDK to run the real test.

### Vulkan Sandbox ✨ WITH REAL API IMPLEMENTATION
- **Instance Creation**: ✅ **IMPLEMENTED** - Real Vulkan API test
  - Queries Vulkan API version
  - Enumerates available instance extensions
  - Creates minimal Vulkan instance
  - Enumerates physical devices and GPU information
  - Properly destroys instance on shutdown
- Physical device selection (stub implementation)
- Logical device initialization (stub implementation)
- Queue family management (stub implementation)
- Command buffer creation (stub implementation)
- Resource creation (buffers, images) (stub implementation)
- Pipeline creation (stub implementation)
- Swap chain management (stub implementation)

**Note:** Vulkan instance creation test includes real Vulkan API calls. Other tests are stub implementations. Install Vulkan SDK to run the real test.

## Building

Build the RenderBackendSandbox module:

```bash
# Build the module
xmake build RenderBackendSandbox

# Run the sandbox
xmake run RenderBackendSandbox
```

## Usage

The sandbox automatically runs all tests on startup:

### Test Execution Order
1. **Basic Tests** - Concrete implementations with real validation
2. **DX12 Tests** - DirectX 12 API tests (**Device Initialization test has real implementation**)
3. **Vulkan Tests** - Vulkan API tests (**Instance Creation test has real implementation**)

### Expected Output

**With Windows SDK on Windows (DX12):**
```
========================================
=== Running DX12 Sandbox Tests ===
========================================
DX12Sandbox: Test - Device Initialization
DX12Sandbox: Successfully created DXGI factory
DX12Sandbox: Found 1 adapter(s)
DX12Sandbox: Primary Adapter: NVIDIA GeForce RTX 3080
DX12Sandbox: Dedicated Video Memory: 10240 MB
DX12Sandbox: Successfully created D3D12 device!
DX12Sandbox: Feature Level: 12.1
DX12Sandbox: Device Initialization: PASS - DXGI Factory created, Adapters: 1, GPU: NVIDIA GeForce RTX 3080, VRAM: 10240MB, Feature Level: 12.1
```

**Without Windows SDK or on non-Windows:**
```
DX12Sandbox: DX12 SDK not available - install Windows SDK to run this test
DX12Sandbox: Device Initialization: FAIL - DX12 SDK not available
```

**With Vulkan SDK installed:**
```
========================================
=== Running Vulkan Sandbox Tests ===
========================================
VulkanSandbox: Test - Instance Creation
VulkanSandbox: Vulkan API Version: 1.3.X
VulkanSandbox: Found 15 instance extensions
VulkanSandbox: Available extensions:
  - VK_KHR_surface
  - VK_KHR_xcb_surface
  - VK_KHR_xlib_surface
  ... and more
VulkanSandbox: Successfully created Vulkan instance!
VulkanSandbox: Found 1 physical device(s)
VulkanSandbox: Primary GPU: NVIDIA GeForce RTX 3080
VulkanSandbox: Instance Creation: PASS - Vulkan API Version: 1.3.X, Extensions: 15, Physical Devices: 1, GPU: NVIDIA GeForce RTX 3080
```

**Without Vulkan SDK:**
```
VulkanSandbox: Vulkan SDK not found - stub implementation
VulkanSandbox: Instance Creation: FAIL - Vulkan SDK not available - install Vulkan SDK to run this test
```

**Basic Tests output:**
```
========================================
=== Running Basic Concrete Tests ===
========================================
BasicTests: Memory Allocation: PASS - Successfully allocated buffers: 1KB and 1MB
BasicTests: String Operations: PASS - All string operations validated successfully
BasicTests: Math Operations: PASS - All math operations validated successfully
BasicTests: Platform Detection: PASS - Detected platform: Linux (64-bit), Little Endian
BasicTests: Buffer Operations: PASS - All buffer operations validated successfully

Summary: 5 passed, 0 failed
========================================
```

### Adding Your Own Tests

You can modify the source files to add your own tests:

1. Edit `BasicTests.cpp` to add more platform-independent tests
2. Edit `DX12Sandbox.cpp` to add DX12-specific tests
3. Edit `VulkanSandbox.cpp` to add Vulkan-specific tests
4. Rebuild and run to see your test results

## Platform Support

- **Windows**: Full DX12 support + Vulkan support (with Vulkan SDK)
- **Linux**: Vulkan support (DX12 N/A)
- **macOS**: Metal support recommended (Vulkan via MoltenVK, DX12 N/A)

## Integration Roadmap

1. **Phase 1 (Current)**: Isolated testing environment with BasicTests concrete implementation ✅
2. **Phase 2**: Implement actual DX12 and Vulkan API calls
3. **Phase 3**: Create abstraction interfaces for common patterns
4. **Phase 4**: Integrate tested backends into RHI module
5. **Phase 5**: Add backend selection at runtime

## Adding New Tests

To add a new test:

### For Basic Tests:

1. Add a test method declaration in `BasicTests.h`:
   ```cpp
   void TestMyNewFeature();
   ```

2. Implement the test in `BasicTests.cpp`:
   ```cpp
   void BasicTests::TestMyNewFeature()
   {
       LOG("BasicTests: Test - My New Feature");
       
       bool passed = true;
       std::string details;
       
       try
       {
           // Your actual test logic here
           // Perform real operations and validation
           int result = 2 + 2;
           if (result != 4)
           {
               passed = false;
               details = "Math is broken!";
           }
           else
           {
               details = "Validation successful";
           }
       }
       catch (const std::exception& e)
       {
           passed = false;
           details = std::string("Exception: ") + e.what();
       }
       
       LogTestResult("My New Feature", passed, details);
   }
   ```

3. Call the test in `RunTests()`:
   ```cpp
   void BasicTests::RunTests()
   {
       // ... existing tests ...
       TestMyNewFeature();
   }
   ```

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

## BasicTests Implementation Details

The BasicTests class demonstrates real testing with actual validation:

### Test Coverage

| Test Name | What It Tests | Validations |
|-----------|---------------|-------------|
| **Memory Allocation** | Buffer allocation and memory operations | Allocates 1KB and 1MB buffers, writes/reads patterns, validates data integrity |
| **String Operations** | C++ string functionality | Concatenation, length, comparison, substring extraction |
| **Math Operations** | Arithmetic and math functions | Basic arithmetic, floating point (with tolerance), sqrt, pow, min/max |
| **Platform Detection** | System information | OS detection (Win/Linux/macOS), architecture (32/64-bit), endianness |
| **Buffer Operations** | Low-level memory operations | memset (multiple patterns: 0xAA, 0x55, 0x00), memcpy with validation |

### Key Features

- ✅ **Real Validation**: Not stub implementations - actual pass/fail logic
- ✅ **Exception Handling**: Try/catch blocks for robust error handling
- ✅ **Cross-Platform**: Works on Windows, Linux, and macOS
- ✅ **No External Dependencies**: Uses only standard C++ library
- ✅ **Detailed Logging**: Reports test results with descriptive messages
- ✅ **Pass/Fail Tracking**: Counts and reports passed/failed tests

### Example: How BasicTests Differ from Stubs

**Stub Implementation (DX12/Vulkan):**
```cpp
void DX12Sandbox::TestDeviceInitialization()
{
    LOG("DX12Sandbox: Test - Device Initialization");
    bool passed = true; // Always passes
    LogTestResult("Device Initialization", passed, "Device created successfully (stub)");
}
```

**Concrete Implementation (BasicTests):**
```cpp
void BasicTests::TestMemoryAllocation()
{
    bool passed = true;
    try
    {
        // Actually allocate memory
        if (!AllocateTestBuffer(1024)) {
            passed = false; // Real failure condition
        }
        // Actually write to memory
        for (size_t i = 0; i < TestBufferSize; ++i) {
            TestBuffer[i] = static_cast<uint8_t>(i % 256);
        }
        // Actually validate memory contents
        for (size_t i = 0; i < TestBufferSize; ++i) {
            if (TestBuffer[i] != static_cast<uint8_t>(i % 256)) {
                passed = false; // Real validation failure
                break;
            }
        }
    }
    catch (const std::exception& e) {
        passed = false; // Real exception handling
    }
    LogTestResult("Memory Allocation", passed, details);
}
```

The BasicTests demonstrate the pattern that DX12 and Vulkan tests should follow when implementing real API calls.

## Vulkan Instance Creation Test Implementation

The Vulkan sandbox now includes a **real implementation** of instance creation test, demonstrating actual Vulkan API usage.

### What the Vulkan Test Does

The `TestInstanceCreation()` method performs the following operations with real Vulkan API calls:

1. **Query Vulkan Version**
   ```cpp
   vkEnumerateInstanceVersion(&apiVersion);
   // Reports: "Vulkan API Version: 1.3.X"
   ```

2. **Enumerate Instance Extensions**
   ```cpp
   vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
   // Reports available extensions like VK_KHR_surface, VK_KHR_xcb_surface, etc.
   ```

3. **Create Minimal Vulkan Instance**
   ```cpp
   VkInstanceCreateInfo createInfo = {};
   createInfo.pApplicationInfo = &appInfo;
   vkCreateInstance(&createInfo, nullptr, &instance);
   // Creates a real Vulkan instance without any extensions
   ```

4. **Enumerate Physical Devices**
   ```cpp
   vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
   vkGetPhysicalDeviceProperties(devices[0], &deviceProperties);
   // Reports GPU name (e.g., "NVIDIA GeForce RTX 3080")
   ```

5. **Clean Up**
   ```cpp
   vkDestroyInstance(instance, nullptr);
   // Properly destroys the instance in Shutdown()
   ```

### Conditional Compilation

The implementation uses `__has_include` to detect Vulkan SDK availability:

- **With Vulkan SDK**: Runs real Vulkan API calls
- **Without Vulkan SDK**: Reports that SDK is not available, test shows as FAIL with informative message

### Example Code Pattern

This implementation shows the pattern for adding more Vulkan tests:

```cpp
void VulkanSandbox::TestInstanceCreation()
{
#if HAS_VULKAN_SDK
    bool passed = true;
    std::string details;
    
    try {
        // Step 1: Perform Vulkan API operation
        VkResult result = vkSomeVulkanFunction(...);
        
        if (result == VK_SUCCESS) {
            details = "Operation successful";
        } else {
            passed = false;
            details = "Operation failed";
        }
    }
    catch (const std::exception& e) {
        passed = false;
        details = std::string("Exception: ") + e.what();
    }
    
    LogTestResult("Test Name", passed, details);
#else
    LogTestResult("Test Name", false, "Vulkan SDK not available");
#endif
}
```

### Benefits of This Implementation

- ✅ **Real Vulkan API Usage**: Not a stub - actual API calls
- ✅ **Proper Error Handling**: Checks VkResult and handles exceptions
- ✅ **Informative Output**: Reports version, extensions, GPU information
- ✅ **Graceful Degradation**: Works without Vulkan SDK (reports unavailable)
- ✅ **Resource Management**: Properly cleans up Vulkan instance
- ✅ **Cross-Platform**: Works on Linux, Windows, macOS (with Vulkan SDK)

## DX12 Device Initialization Test Implementation

The DX12 sandbox now includes a **real implementation** of device initialization test, demonstrating actual DirectX 12 API usage.

### What the DX12 Test Does

The `TestDeviceInitialization()` method performs the following operations with real DX12 API calls:

1. **Create DXGI Factory**
   ```cpp
   ComPtr<IDXGIFactory4> factory;
   CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
   // Creates DXGI factory for enumerating adapters
   ```

2. **Enumerate Graphics Adapters**
   ```cpp
   factory->EnumAdapters1(adapterIndex, &adapter);
   // Enumerates all graphics adapters in the system
   ```

3. **Get Adapter Information**
   ```cpp
   adapter->GetDesc1(&desc);
   // Reports GPU name, VRAM, etc.
   // Example: "NVIDIA GeForce RTX 3080, VRAM: 10240MB"
   ```

4. **Create D3D12 Device with Feature Level Detection**
   ```cpp
   D3D12CreateDevice(adapter.Get(), featureLevel, IID_PPV_ARGS(&device));
   // Tries feature levels: 12.1, 12.0, 11.1, 11.0
   // Reports highest supported level
   ```

5. **Clean Up**
   ```cpp
   device.Reset(); // ComPtr automatically releases
   // Properly releases device in Shutdown()
   ```

### Conditional Compilation

The implementation uses `__has_include` to detect Windows SDK availability:

- **With Windows SDK on Windows**: Runs real DX12 API calls
- **Without Windows SDK on Windows**: Reports that SDK is not available
- **On non-Windows platforms**: Reports that DX12 is Windows-only

### Example Output

**With Windows SDK:**
```
DX12Sandbox: Successfully created DXGI factory
DX12Sandbox: Found 1 adapter(s)
DX12Sandbox: Primary Adapter: NVIDIA GeForce RTX 3080
DX12Sandbox: Dedicated Video Memory: 10240 MB
DX12Sandbox: Successfully created D3D12 device!
DX12Sandbox: Feature Level: 12.1
DX12Sandbox: Device Initialization: PASS - DXGI Factory created, Adapters: 1, GPU: NVIDIA GeForce RTX 3080, VRAM: 10240MB, Feature Level: 12.1
```

**Without Windows SDK:**
```
DX12Sandbox: DX12 SDK not available - install Windows SDK to run this test
DX12Sandbox: Device Initialization: FAIL - DX12 SDK not available
```

### Benefits of This Implementation

- ✅ **Real DX12 API Usage**: Not a stub - actual API calls
- ✅ **Proper Error Handling**: Checks HRESULT values and handles exceptions
- ✅ **Informative Output**: Reports GPU name, VRAM, feature level
- ✅ **Graceful Degradation**: Works without Windows SDK (reports unavailable)
- ✅ **Resource Management**: Uses ComPtr for automatic cleanup
- ✅ **Feature Level Detection**: Automatically detects highest supported D3D feature level

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

- **DX12**: Device initialization test now uses real DX12 API calls (Windows with SDK only)
- **Vulkan**: Instance creation test now uses real Vulkan API calls (all platforms with SDK)
- **BasicTests**: All tests use real validation logic (platform-independent)
- Other DX12 and Vulkan tests remain as stubs for future implementation
- Each sandbox is independent - failures in one don't affect the other
- Tests can be run individually or all at once

## Future Enhancements

- [ ] Add more DX12 API implementations (command queues, resources, etc.)
- [ ] Add more Vulkan API implementations (devices, pipelines, etc.)
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
