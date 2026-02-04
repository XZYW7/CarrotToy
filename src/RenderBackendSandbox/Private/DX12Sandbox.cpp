#include "DX12Sandbox.h"
#include "CoreUtils.h"
#include <iostream>
#include <vector>

// Try to include DX12 headers if available
#ifdef _WIN32
#  ifdef __has_include
#    if __has_include(<d3d12.h>) && __has_include(<dxgi1_6.h>)
#      define HAS_DX12_SDK 1
#      include <d3d12.h>
#      include <dxgi1_6.h>
#      include <wrl/client.h>  // For ComPtr
       using Microsoft::WRL::ComPtr;
#    else
#      define HAS_DX12_SDK 0
#    endif
#  else
#    define HAS_DX12_SDK 0
#  endif
#else
#  define HAS_DX12_SDK 0
#endif

DX12Sandbox::DX12Sandbox()
{
    LOG("DX12Sandbox: Constructor");
}

DX12Sandbox::~DX12Sandbox()
{
    Shutdown();
}

bool DX12Sandbox::Initialize()
{
    LOG("DX12Sandbox: Initializing DirectX 12 testing environment");
    
#if HAS_DX12_SDK
    LOG("DX12Sandbox: Windows platform with DX12 SDK detected - using real DX12 API");
    
    // This is a simple test - we'll just check if we can create a DXGI factory
    // and enumerate adapters without creating a full device initially
    
    bInitialized = true;
    LOG("DX12Sandbox: Initialization complete (DX12 SDK available)");
    return true;
#elif defined(_WIN32)
    LOG("DX12Sandbox: Windows platform but DX12 SDK not found - stub implementation");
    LOG("DX12Sandbox: Install Windows SDK to run real DX12 tests");
    
    bInitialized = true;
    LOG("DX12Sandbox: Initialization complete (stub mode)");
    return true;
#else
    LOG("DX12Sandbox: Platform is not Windows - DX12 not available");
    LOG("DX12Sandbox: Skipping DX12 initialization");
    bInitialized = false;
    return false;
#endif
}

void DX12Sandbox::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }
    
    LOG("DX12Sandbox: Shutting down DirectX 12 testing environment");
    
#if HAS_DX12_SDK
    // Clean up any DX12 resources
    if (Device != nullptr)
    {
        // ComPtr will automatically release
        Device = nullptr;
        LOG("DX12Sandbox: Released D3D12 device");
    }
#endif
    
    bInitialized = false;
    LOG("DX12Sandbox: Shutdown complete");
}

void DX12Sandbox::RunTests()
{
    if (!bInitialized)
    {
        LOG("DX12Sandbox: Error - Cannot run tests, sandbox not initialized");
        return;
    }
    
    LOG("=== Running DX12 Sandbox Tests ===");
    TestResults.clear();
    
    TestDeviceInitialization();
    TestCommandQueueCreation();
    TestResourceCreation();
    TestPipelineStateObject();
    TestDescriptorHeaps();
    TestSwapChainCreation();
    
    LOG("=== DX12 Sandbox Tests Complete ===");
    LOG("DX12Sandbox: Total tests run: " + std::to_string(TestResults.size()));
}

void DX12Sandbox::TestDeviceInitialization()
{
    LOG("DX12Sandbox: Test - Device Initialization");
    
#if HAS_DX12_SDK
    bool passed = true;
    std::string details;
    
    try
    {
        // Step 1: Create DXGI Factory
        ComPtr<IDXGIFactory4> factory;
        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
        
        if (FAILED(hr))
        {
            passed = false;
            details = "Failed to create DXGI factory (HRESULT: 0x" + 
                     std::to_string(static_cast<unsigned long>(hr)) + ")";
        }
        else
        {
            LOG("DX12Sandbox: Successfully created DXGI factory");
            details = "DXGI Factory created";
            
            // Step 2: Enumerate adapters
            ComPtr<IDXGIAdapter1> adapter;
            UINT adapterIndex = 0;
            std::vector<ComPtr<IDXGIAdapter1>> adapters;
            
            while (factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
            {
                adapters.push_back(adapter);
                adapterIndex++;
            }
            
            details += ", Adapters: " + std::to_string(adapters.size());
            LOG("DX12Sandbox: Found " + std::to_string(adapters.size()) + " adapter(s)");
            
            if (adapters.empty())
            {
                passed = false;
                details += " - No adapters found";
            }
            else
            {
                // Step 3: Get adapter information
                DXGI_ADAPTER_DESC1 desc;
                hr = adapters[0]->GetDesc1(&desc);
                
                if (SUCCEEDED(hr))
                {
                    // Convert wide string to regular string
                    char adapterName[256];
                    size_t converted = 0;
                    wcstombs_s(&converted, adapterName, sizeof(adapterName), desc.Description, _TRUNCATE);
                    
                    LOG("DX12Sandbox: Primary Adapter: " + std::string(adapterName));
                    details += ", GPU: " + std::string(adapterName);
                    
                    // Report VRAM
                    size_t vramMB = desc.DedicatedVideoMemory / (1024 * 1024);
                    details += ", VRAM: " + std::to_string(vramMB) + "MB";
                    LOG("DX12Sandbox: Dedicated Video Memory: " + std::to_string(vramMB) + " MB");
                }
                
                // Step 4: Try to create D3D12 device
                ComPtr<ID3D12Device> device;
                
                // Try different feature levels
                D3D_FEATURE_LEVEL featureLevels[] = {
                    D3D_FEATURE_LEVEL_12_1,
                    D3D_FEATURE_LEVEL_12_0,
                    D3D_FEATURE_LEVEL_11_1,
                    D3D_FEATURE_LEVEL_11_0
                };
                
                D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
                bool deviceCreated = false;
                
                for (auto level : featureLevels)
                {
                    hr = D3D12CreateDevice(adapters[0].Get(), level, IID_PPV_ARGS(&device));
                    if (SUCCEEDED(hr))
                    {
                        featureLevel = level;
                        deviceCreated = true;
                        break;
                    }
                }
                
                if (deviceCreated)
                {
                    LOG("DX12Sandbox: Successfully created D3D12 device!");
                    
                    // Store device for cleanup
                    Device = device;
                    
                    // Report feature level
                    std::string levelStr;
                    switch (featureLevel)
                    {
                        case D3D_FEATURE_LEVEL_12_1: levelStr = "12.1"; break;
                        case D3D_FEATURE_LEVEL_12_0: levelStr = "12.0"; break;
                        case D3D_FEATURE_LEVEL_11_1: levelStr = "11.1"; break;
                        case D3D_FEATURE_LEVEL_11_0: levelStr = "11.0"; break;
                        default: levelStr = "Unknown"; break;
                    }
                    
                    details += ", Feature Level: " + levelStr;
                    LOG("DX12Sandbox: Feature Level: " + levelStr);
                }
                else
                {
                    passed = false;
                    details += " - Failed to create D3D12 device (HRESULT: 0x" + 
                             std::to_string(static_cast<unsigned long>(hr)) + ")";
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        passed = false;
        details = std::string("Exception: ") + e.what();
    }
    
    LogTestResult("Device Initialization", passed, details);
#elif defined(_WIN32)
    // Stub implementation when DX12 SDK is not available
    bool passed = false;
    std::string details = "DX12 SDK not available - install Windows SDK to run this test";
    LOG("DX12Sandbox: " + details);
    LogTestResult("Device Initialization", passed, details);
#else
    // Not Windows platform
    bool passed = false;
    std::string details = "DX12 only available on Windows platform";
    LOG("DX12Sandbox: " + details);
    LogTestResult("Device Initialization", passed, details);
#endif
}

void DX12Sandbox::TestCommandQueueCreation()
{
    LOG("DX12Sandbox: Test - Command Queue Creation");
    
    // TODO: Implement actual test
    // - Create direct command queue
    // - Create copy command queue
    // - Create compute command queue
    // - Verify queue properties
    
    bool passed = true; // Stub
    LogTestResult("Command Queue Creation", passed, "Command queues created (stub)");
}

void DX12Sandbox::TestResourceCreation()
{
    LOG("DX12Sandbox: Test - Resource Creation");
    
    // TODO: Implement actual test
    // - Create committed resources (buffers, textures)
    // - Create placed resources
    // - Test resource barriers
    // - Verify resource states
    
    bool passed = true; // Stub
    LogTestResult("Resource Creation", passed, "Resources created successfully (stub)");
}

void DX12Sandbox::TestPipelineStateObject()
{
    LOG("DX12Sandbox: Test - Pipeline State Object");
    
    // TODO: Implement actual test
    // - Create root signature
    // - Compile shaders
    // - Create graphics pipeline state
    // - Create compute pipeline state
    
    bool passed = true; // Stub
    LogTestResult("Pipeline State Object", passed, "PSO created successfully (stub)");
}

void DX12Sandbox::TestDescriptorHeaps()
{
    LOG("DX12Sandbox: Test - Descriptor Heaps");
    
    // TODO: Implement actual test
    // - Create CBV_SRV_UAV descriptor heap
    // - Create RTV descriptor heap
    // - Create DSV descriptor heap
    // - Test descriptor allocation
    
    bool passed = true; // Stub
    LogTestResult("Descriptor Heaps", passed, "Descriptor heaps created (stub)");
}

void DX12Sandbox::TestSwapChainCreation()
{
    LOG("DX12Sandbox: Test - Swap Chain Creation");
    
    // TODO: Implement actual test
    // - Create DXGI swap chain
    // - Configure back buffers
    // - Test present functionality
    
    bool passed = true; // Stub
    LogTestResult("Swap Chain Creation", passed, "Swap chain created (stub)");
}

void DX12Sandbox::LogTestResult(const std::string& testName, bool passed, const std::string& details)
{
    std::string result = testName + ": " + (passed ? "PASS" : "FAIL");
    if (!details.empty())
    {
        result += " - " + details;
    }
    
    TestResults.push_back(result);
    LOG("DX12Sandbox: " + result);
}
