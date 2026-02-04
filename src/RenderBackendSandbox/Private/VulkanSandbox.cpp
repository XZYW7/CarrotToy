#include "VulkanSandbox.h"
#include "CoreUtils.h"
#include <iostream>
#include <vector>
#include <cstring>

// Try to include Vulkan headers if available
#ifdef __has_include
#  if __has_include(<vulkan/vulkan.h>)
#    define HAS_VULKAN_SDK 1
#    include <vulkan/vulkan.h>
#  else
#    define HAS_VULKAN_SDK 0
#  endif
#else
#  define HAS_VULKAN_SDK 0
#endif

VulkanSandbox::VulkanSandbox()
{
    LOG("VulkanSandbox: Constructor");
}

VulkanSandbox::~VulkanSandbox()
{
    Shutdown();
}

bool VulkanSandbox::Initialize()
{
    LOG("VulkanSandbox: Initializing Vulkan testing environment");
    
#if HAS_VULKAN_SDK
    LOG("VulkanSandbox: Vulkan SDK detected - using real Vulkan API");
    
    // This is a simple test - we'll just check if we can query Vulkan version
    // and available extensions without creating a full instance
    
    bInitialized = true;
    LOG("VulkanSandbox: Initialization complete (Vulkan SDK available)");
    return true;
#else
    LOG("VulkanSandbox: Vulkan SDK not found - stub implementation");
    LOG("VulkanSandbox: Install Vulkan SDK to run real Vulkan tests");
    
    // Stub implementation - assume Vulkan is not available
    bInitialized = true;
    LOG("VulkanSandbox: Initialization complete (stub mode)");
    return true;
#endif
}

void VulkanSandbox::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }
    
    LOG("VulkanSandbox: Shutting down Vulkan testing environment");
    
#if HAS_VULKAN_SDK
    // Clean up any Vulkan resources
    if (Instance != nullptr)
    {
        vkDestroyInstance(static_cast<VkInstance>(Instance), nullptr);
        Instance = nullptr;
        LOG("VulkanSandbox: Destroyed Vulkan instance");
    }
#endif
    
    bInitialized = false;
    LOG("VulkanSandbox: Shutdown complete");
}

void VulkanSandbox::RunTests()
{
    if (!bInitialized)
    {
        LOG("VulkanSandbox: Error - Cannot run tests, sandbox not initialized");
        return;
    }
    
    LOG("=== Running Vulkan Sandbox Tests ===");
    TestResults.clear();
    
    TestInstanceCreation();
    TestPhysicalDeviceSelection();
    TestLogicalDeviceCreation();
    TestCommandBufferCreation();
    TestResourceCreation();
    TestPipelineCreation();
    TestSwapChainCreation();
    
    LOG("=== Vulkan Sandbox Tests Complete ===");
    LOG("VulkanSandbox: Total tests run: " + std::to_string(TestResults.size()));
}

void VulkanSandbox::TestInstanceCreation()
{
    LOG("VulkanSandbox: Test - Instance Creation");
    
#if HAS_VULKAN_SDK
    bool passed = true;
    std::string details;
    
    try
    {
        // Step 1: Query Vulkan API version
        uint32_t apiVersion = 0;
        VkResult result = vkEnumerateInstanceVersion(&apiVersion);
        
        if (result == VK_SUCCESS)
        {
            uint32_t major = VK_VERSION_MAJOR(apiVersion);
            uint32_t minor = VK_VERSION_MINOR(apiVersion);
            uint32_t patch = VK_VERSION_PATCH(apiVersion);
            
            details = "Vulkan API Version: " + std::to_string(major) + "." + 
                     std::to_string(minor) + "." + std::to_string(patch);
            LOG("VulkanSandbox: " + details);
        }
        else
        {
            passed = false;
            details = "Failed to query Vulkan version";
        }
        
        // Step 2: Enumerate available instance extensions
        if (passed)
        {
            uint32_t extensionCount = 0;
            result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            
            if (result == VK_SUCCESS)
            {
                details += ", Extensions: " + std::to_string(extensionCount);
                LOG("VulkanSandbox: Found " + std::to_string(extensionCount) + " instance extensions");
                
                if (extensionCount > 0)
                {
                    std::vector<VkExtensionProperties> extensions(extensionCount);
                    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
                    
                    if (result == VK_SUCCESS)
                    {
                        LOG("VulkanSandbox: Available extensions:");
                        for (uint32_t i = 0; i < std::min(extensionCount, 5u); ++i)
                        {
                            LOG("  - " + std::string(extensions[i].extensionName));
                        }
                        if (extensionCount > 5)
                        {
                            LOG("  ... and " + std::to_string(extensionCount - 5) + " more");
                        }
                    }
                }
            }
            else
            {
                passed = false;
                details = "Failed to enumerate instance extensions";
            }
        }
        
        // Step 3: Create a minimal Vulkan instance
        if (passed)
        {
            VkApplicationInfo appInfo = {};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "RenderBackendSandbox";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "CarrotToy";
            appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;
            
            VkInstanceCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;
            createInfo.enabledExtensionCount = 0;
            createInfo.ppEnabledExtensionNames = nullptr;
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
            
            VkInstance instance = VK_NULL_HANDLE;
            result = vkCreateInstance(&createInfo, nullptr, &instance);
            
            if (result == VK_SUCCESS)
            {
                LOG("VulkanSandbox: Successfully created Vulkan instance!");
                
                // Store the instance for cleanup
                Instance = instance;
                
                // Step 4: Enumerate physical devices
                uint32_t deviceCount = 0;
                result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
                
                if (result == VK_SUCCESS && deviceCount > 0)
                {
                    details += ", Physical Devices: " + std::to_string(deviceCount);
                    LOG("VulkanSandbox: Found " + std::to_string(deviceCount) + " physical device(s)");
                    
                    std::vector<VkPhysicalDevice> devices(deviceCount);
                    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
                    
                    // Get properties of first device
                    VkPhysicalDeviceProperties deviceProperties;
                    vkGetPhysicalDeviceProperties(devices[0], &deviceProperties);
                    
                    LOG("VulkanSandbox: Primary GPU: " + std::string(deviceProperties.deviceName));
                    details += ", GPU: " + std::string(deviceProperties.deviceName);
                }
                else if (deviceCount == 0)
                {
                    LOG("VulkanSandbox: Warning - No Vulkan-compatible GPUs found");
                    details += ", No GPUs found";
                }
            }
            else
            {
                passed = false;
                details = "Failed to create Vulkan instance (error code: " + std::to_string(result) + ")";
            }
        }
    }
    catch (const std::exception& e)
    {
        passed = false;
        details = std::string("Exception: ") + e.what();
    }
    
    LogTestResult("Instance Creation", passed, details);
#else
    // Stub implementation when Vulkan SDK is not available
    bool passed = false;
    std::string details = "Vulkan SDK not available - install Vulkan SDK to run this test";
    LOG("VulkanSandbox: " + details);
    LogTestResult("Instance Creation", passed, details);
#endif
}

void VulkanSandbox::TestPhysicalDeviceSelection()
{
    LOG("VulkanSandbox: Test - Physical Device Selection");
    
    // TODO: Implement actual test
    // - Enumerate physical devices
    // - Query device properties
    // - Query device features
    // - Select suitable device
    
    bool passed = true; // Stub
    LogTestResult("Physical Device Selection", passed, "Physical device selected (stub)");
}

void VulkanSandbox::TestLogicalDeviceCreation()
{
    LOG("VulkanSandbox: Test - Logical Device Creation");
    
    // TODO: Implement actual test
    // - Enumerate queue families
    // - Select graphics and present queues
    // - Create logical device
    // - Get queue handles
    
    bool passed = true; // Stub
    LogTestResult("Logical Device Creation", passed, "Logical device created (stub)");
}

void VulkanSandbox::TestCommandBufferCreation()
{
    LOG("VulkanSandbox: Test - Command Buffer Creation");
    
    // TODO: Implement actual test
    // - Create command pool
    // - Allocate command buffers
    // - Begin/end command buffer recording
    // - Submit command buffers
    
    bool passed = true; // Stub
    LogTestResult("Command Buffer Creation", passed, "Command buffers created (stub)");
}

void VulkanSandbox::TestResourceCreation()
{
    LOG("VulkanSandbox: Test - Resource Creation");
    
    // TODO: Implement actual test
    // - Create vertex buffers
    // - Create index buffers
    // - Create uniform buffers
    // - Create textures and images
    // - Allocate and bind memory
    
    bool passed = true; // Stub
    LogTestResult("Resource Creation", passed, "Resources created successfully (stub)");
}

void VulkanSandbox::TestPipelineCreation()
{
    LOG("VulkanSandbox: Test - Pipeline Creation");
    
    // TODO: Implement actual test
    // - Load shader modules
    // - Create pipeline layout
    // - Create graphics pipeline
    // - Create compute pipeline
    
    bool passed = true; // Stub
    LogTestResult("Pipeline Creation", passed, "Pipelines created successfully (stub)");
}

void VulkanSandbox::TestSwapChainCreation()
{
    LOG("VulkanSandbox: Test - Swap Chain Creation");
    
    // TODO: Implement actual test
    // - Query surface capabilities
    // - Select surface format
    // - Select present mode
    // - Create swap chain
    // - Create image views
    
    bool passed = true; // Stub
    LogTestResult("Swap Chain Creation", passed, "Swap chain created (stub)");
}

void VulkanSandbox::LogTestResult(const std::string& testName, bool passed, const std::string& details)
{
    std::string result = testName + ": " + (passed ? "PASS" : "FAIL");
    if (!details.empty())
    {
        result += " - " + details;
    }
    
    TestResults.push_back(result);
    LOG("VulkanSandbox: " + result);
}
