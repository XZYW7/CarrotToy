#include "RenderBackendSandboxModule.h"
#include "BasicTests.h"
#include "DX12Sandbox.h"
#include "VulkanSandbox.h"
#include "CoreUtils.h"
#include <iostream>

FRenderBackendSandboxModule::FRenderBackendSandboxModule() = default;
FRenderBackendSandboxModule::~FRenderBackendSandboxModule() = default;

void FRenderBackendSandboxModule::StartupModule()
{
    LOG("========================================");
    LOG("=== RenderBackendSandbox Starting ===");
    LOG("========================================");
    LOG("");
    LOG("This sandbox module provides an isolated testing environment for:");
    LOG("  - Basic functionality testing (concrete implementations)");
    LOG("  - DirectX 12 (DX12) API implementation and testing");
    LOG("  - Vulkan API implementation and testing");
    LOG("");
    LOG("Purpose:");
    LOG("  - Validate basic testing framework with real tests");
    LOG("  - Experiment with DX12 and Vulkan APIs");
    LOG("  - Validate backend implementations before RHI integration");
    LOG("  - Test rendering features in isolation");
    LOG("");
    
    InitializeSandbox();
    
    // Run all sandbox tests
    LOG("========================================");
    RunAllTests();
    LOG("========================================");
}

void FRenderBackendSandboxModule::ShutdownModule()
{
    LOG("");
    LOG("========================================");
    LOG("=== RenderBackendSandbox Shutdown ===");
    LOG("========================================");
    
    ShutdownSandbox();
}

void FRenderBackendSandboxModule::InitializeSandbox()
{
    LOG("RenderBackendSandbox: Initializing sandbox environment");
    
    // Initialize basic tests
    LOG("");
    LOG("--- Initializing Basic Tests ---");
    BasicTestEnvironment = std::make_unique<BasicTests>();
    if (BasicTestEnvironment->Initialize())
    {
        LOG("Basic tests initialized successfully");
    }
    else
    {
        LOG("Basic tests initialization failed");
    }
    
    // Initialize DX12 sandbox
    LOG("");
    LOG("--- Initializing DX12 Sandbox ---");
    DX12TestEnvironment = std::make_unique<DX12Sandbox>();
    if (DX12TestEnvironment->Initialize())
    {
        LOG("DX12 sandbox initialized successfully");
    }
    else
    {
        LOG("DX12 sandbox initialization skipped or failed");
    }
    
    // Initialize Vulkan sandbox
    LOG("");
    LOG("--- Initializing Vulkan Sandbox ---");
    VulkanTestEnvironment = std::make_unique<VulkanSandbox>();
    if (VulkanTestEnvironment->Initialize())
    {
        LOG("Vulkan sandbox initialized successfully");
    }
    else
    {
        LOG("Vulkan sandbox initialization skipped or failed");
    }
    
    bSandboxInitialized = true;
    LOG("");
    LOG("RenderBackendSandbox: Sandbox environment initialized");
}

void FRenderBackendSandboxModule::ShutdownSandbox()
{
    if (!bSandboxInitialized)
    {
        return;
    }
    
    LOG("RenderBackendSandbox: Shutting down sandbox environment");
    
    // Shutdown Vulkan sandbox
    if (VulkanTestEnvironment)
    {
        VulkanTestEnvironment->Shutdown();
        VulkanTestEnvironment.reset();
    }
    
    // Shutdown DX12 sandbox
    if (DX12TestEnvironment)
    {
        DX12TestEnvironment->Shutdown();
        DX12TestEnvironment.reset();
    }
    
    // Shutdown basic tests
    if (BasicTestEnvironment)
    {
        BasicTestEnvironment->Shutdown();
        BasicTestEnvironment.reset();
    }
    
    bSandboxInitialized = false;
    LOG("RenderBackendSandbox: Sandbox environment shutdown complete");
}

void FRenderBackendSandboxModule::RunBasicTests()
{
    LOG("");
    LOG("========================================");
    LOG("=== Running Basic Concrete Tests ===");
    LOG("========================================");
    
    if (BasicTestEnvironment && BasicTestEnvironment->IsInitialized())
    {
        BasicTestEnvironment->RunTests();
        
        // Print test results summary
        const auto& results = BasicTestEnvironment->GetTestResults();
        LOG("");
        LOG("Basic Test Results Summary:");
        for (const auto& result : results)
        {
            LOG("  " + result);
        }
        LOG("");
        LOG("Summary: " + std::to_string(BasicTestEnvironment->GetPassedTests()) + 
            " passed, " + std::to_string(BasicTestEnvironment->GetFailedTests()) + " failed");
    }
    else
    {
        LOG("Basic tests not initialized - tests skipped");
    }
}

void FRenderBackendSandboxModule::RunDX12Tests()
{
    LOG("");
    LOG("========================================");
    LOG("=== Running DX12 Tests ===");
    LOG("========================================");
    
    if (DX12TestEnvironment && DX12TestEnvironment->IsInitialized())
    {
        DX12TestEnvironment->RunTests();
        
        // Print test results summary
        const auto& results = DX12TestEnvironment->GetTestResults();
        LOG("");
        LOG("DX12 Test Results Summary:");
        for (const auto& result : results)
        {
            LOG("  " + result);
        }
    }
    else
    {
        LOG("DX12 sandbox not initialized - tests skipped");
    }
}

void FRenderBackendSandboxModule::RunVulkanTests()
{
    LOG("");
    LOG("========================================");
    LOG("=== Running Vulkan Tests ===");
    LOG("========================================");
    
    if (VulkanTestEnvironment && VulkanTestEnvironment->IsInitialized())
    {
        VulkanTestEnvironment->RunTests();
        
        // Print test results summary
        const auto& results = VulkanTestEnvironment->GetTestResults();
        LOG("");
        LOG("Vulkan Test Results Summary:");
        for (const auto& result : results)
        {
            LOG("  " + result);
        }
    }
    else
    {
        LOG("Vulkan sandbox not initialized - tests skipped");
    }
}

void FRenderBackendSandboxModule::RunAllTests()
{
    if (!bSandboxInitialized)
    {
        LOG("RenderBackendSandbox: Error - Sandbox not initialized");
        return;
    }
    
    LOG("RenderBackendSandbox: Running all sandbox tests");
    
    // Run basic tests first
    RunBasicTests();
    
    // Run DX12 tests
    RunDX12Tests();
    
    // Run Vulkan tests
    RunVulkanTests();
    
    LOG("");
    LOG("========================================");
    LOG("RenderBackendSandbox: All tests complete");
    LOG("========================================");
}
