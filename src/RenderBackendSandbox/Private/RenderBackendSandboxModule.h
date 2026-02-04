/**
 * RenderBackendSandbox - Application Module Header
 * 
 * This sandbox module provides a testing environment for implementing and testing
 * DirectX 12 (DX12) and Vulkan rendering backend APIs before integrating them into RHI.
 * 
 * Purpose:
 * - Provide an isolated sandbox for DX12 and Vulkan API experimentation
 * - Test and validate backend implementations before RHI integration
 * - Support rapid prototyping and testing of graphics API features
 */

#pragma once
#include "Modules/ModuleInterface.h"
#include <memory>

// Forward declarations
class DX12Sandbox;
class VulkanSandbox;
class BasicTests;

/**
 * RenderBackendSandbox Application Module
 * Provides a testing environment for DX12 and Vulkan backend implementations
 */
class FRenderBackendSandboxModule : public IModuleInterface
{
public:
    FRenderBackendSandboxModule();
    virtual ~FRenderBackendSandboxModule() override;

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
    virtual bool IsGameModule() const override { return false; }
    
    // Sandbox test runners
    void RunBasicTests();
    void RunDX12Tests();
    void RunVulkanTests();
    void RunAllTests();
    
private:
    void InitializeSandbox();
    void ShutdownSandbox();
    
    std::unique_ptr<BasicTests> BasicTestEnvironment;
    std::unique_ptr<DX12Sandbox> DX12TestEnvironment;
    std::unique_ptr<VulkanSandbox> VulkanTestEnvironment;
    bool bSandboxInitialized = false;
};
