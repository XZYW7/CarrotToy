/**
 * CustomModule - Application Module
 * 
 * This is an example custom application module that demonstrates
 * how to create an independent module as a separate output target.
 */

#pragma once
#include "Modules/ModuleInterface.h"

/**
 * Custom Application Module
 * Example of a custom application with its own entry point via IMPLEMENT_APPLICATION_MODULE
 */
class FCustomModule : public IModuleInterface
{
public:
    virtual ~FCustomModule() override = default;

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
    virtual bool IsGameModule() const override { return false; }
    
    // Custom functionality
    void DoSomething();
};
