/**
 * CustomModule - Application Module
 * 
 * This is an example custom application module that demonstrates
 * how to create an independent module as a separate output target.
 */

#ifndef CUSTOM_MODULE_H
#define CUSTOM_MODULE_H

#include "Modules/Module.h"
#include "Modules/EngineModules.h"
#include "CoreUtils.h"

// Custom Module implementation
class FCustomModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        LOG("CustomModule: Starting up");
        // Initialize your custom systems here
        // 在此初始化你的自定义系统
    }

    virtual void ShutdownModule() override
    {
        LOG("CustomModule: Shutting down");
        // Cleanup your custom systems here
        // 在此清理你的自定义系统
    }

    virtual bool IsGameModule() const override
    {
        return true;  // This is a game module
    }

    // Custom functionality
    void DoSomething()
    {
        LOG("CustomModule: Doing something awesome!");
    }
};

#endif // CUSTOM_MODULE_H
