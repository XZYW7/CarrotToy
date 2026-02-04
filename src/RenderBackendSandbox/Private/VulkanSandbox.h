/**
 * VulkanSandbox - Vulkan API Testing Sandbox
 * 
 * Provides an isolated environment for implementing and testing Vulkan API features.
 * This sandbox allows experimentation with Vulkan before full RHI integration.
 */

#pragma once
#include <string>
#include <vector>

/**
 * Vulkan Testing Sandbox
 * 
 * This class provides methods for testing Vulkan API functionality including:
 * - Instance and device creation
 * - Physical device selection
 * - Logical device initialization
 * - Queue family management
 * - Command buffer creation
 * - Resource creation (buffers, images)
 * - Pipeline creation
 * - Swap chain management
 */
class VulkanSandbox
{
public:
    VulkanSandbox();
    ~VulkanSandbox();

    // Initialize Vulkan testing environment
    bool Initialize();
    
    // Shutdown and cleanup Vulkan resources
    void Shutdown();
    
    // Run all Vulkan tests
    void RunTests();
    
    // Individual test methods
    void TestInstanceCreation();
    void TestPhysicalDeviceSelection();
    void TestLogicalDeviceCreation();
    void TestCommandBufferCreation();
    void TestResourceCreation();
    void TestPipelineCreation();
    void TestSwapChainCreation();
    
    // Query test status
    bool IsInitialized() const { return bInitialized; }
    const std::vector<std::string>& GetTestResults() const { return TestResults; }

private:
    void LogTestResult(const std::string& testName, bool passed, const std::string& details = "");
    
    bool bInitialized = false;
    std::vector<std::string> TestResults;
    
    // Vulkan resources (stored as void* to avoid including vulkan.h in header)
    void* Instance = nullptr;
    void* PhysicalDevice = nullptr;
    void* Device = nullptr;
    void* SwapChain = nullptr;
};
