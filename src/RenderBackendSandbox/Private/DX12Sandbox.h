/**
 * DX12Sandbox - DirectX 12 API Testing Sandbox
 * 
 * Provides an isolated environment for implementing and testing DirectX 12 API features.
 * This sandbox allows experimentation with DX12 before full RHI integration.
 */

#pragma once
#include <string>
#include <vector>

/**
 * DX12 Testing Sandbox
 * 
 * This class provides methods for testing DirectX 12 API functionality including:
 * - Device initialization
 * - Command queue and command list management
 * - Resource creation (buffers, textures)
 * - Pipeline state objects
 * - Descriptor heaps
 * - Swap chain management
 */
class DX12Sandbox
{
public:
    DX12Sandbox();
    ~DX12Sandbox();

    // Initialize DX12 testing environment
    bool Initialize();
    
    // Shutdown and cleanup DX12 resources
    void Shutdown();
    
    // Run all DX12 tests
    void RunTests();
    
    // Individual test methods
    void TestDeviceInitialization();
    void TestCommandQueueCreation();
    void TestResourceCreation();
    void TestPipelineStateObject();
    void TestDescriptorHeaps();
    void TestSwapChainCreation();
    
    // Query test status
    bool IsInitialized() const { return bInitialized; }
    const std::vector<std::string>& GetTestResults() const { return TestResults; }

private:
    void LogTestResult(const std::string& testName, bool passed, const std::string& details = "");
    
    bool bInitialized = false;
    std::vector<std::string> TestResults;
    
    // DX12 resources (stored as void* to avoid including d3d12.h in header)
    void* Device = nullptr;
    void* CommandQueue = nullptr;
    void* SwapChain = nullptr;
};
