/**
 * Module System Usage Examples
 * 模块系统使用示例
 * 
 * This file demonstrates practical usage of the CarrotToy Module System
 */

#include "Modules/Module.h"
#include "Modules/EngineModules.h"
#include "CoreUtils.h"

// Example 1: Creating a simple custom module
// 示例1: 创建一个简单的自定义模块
class FMyCustomModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        LOG("MyCustomModule: Starting up");
        // Initialize your custom systems here
        // 在此初始化你的自定义系统
    }

    virtual void ShutdownModule() override
    {
        LOG("MyCustomModule: Shutting down");
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
        LOG("MyCustomModule: Doing something awesome!");
    }
};

// Register the custom module
// 注册自定义模块
IMPLEMENT_GAME_MODULE(FMyCustomModule, MyCustomModule)

// Example 2: Creating a plugin module
// 示例2: 创建一个插件模块
class FMyPluginModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        LOG("MyPluginModule: Plugin starting");
    }

    virtual void ShutdownModule() override
    {
        LOG("MyPluginModule: Plugin shutting down");
    }

    virtual bool SupportsDynamicReloading() override
    {
        return true;  // This plugin supports hot-reloading
    }

    virtual bool IsGameModule() const override
    {
        return false;  // Plugins are not game modules
    }
};

// Example 3: Using the module system at runtime
// 示例3: 在运行时使用模块系统
void ExampleModuleUsage()
{
    LOG("=== Module System Usage Examples ===");
    LOG("=== 模块系统使用示例 ===");

    // 1. Load a module
    // 1. 加载模块
    LOG("\n[Example 1] Loading modules:");
    FModuleManager::Get().LoadModule("CoreEngine");
    FModuleManager::Get().LoadModule("RHI");
    FModuleManager::Get().LoadModule("MyCustomModule");

    // 2. Check if a module is loaded
    // 2. 检查模块是否已加载
    LOG("\n[Example 2] Checking module status:");
    if (FModuleManager::Get().IsModuleLoaded("CoreEngine"))
    {
        LOG("CoreEngine is loaded!");
    }

    // 3. Get and use a module
    // 3. 获取并使用模块
    LOG("\n[Example 3] Using a module:");
    auto* customModule = static_cast<FMyCustomModule*>(
        FModuleManager::Get().GetModule("MyCustomModule")
    );
    if (customModule)
    {
        customModule->DoSomething();
    }

    // 4. Query modules by type
    // 4. 按类型查询模块
    LOG("\n[Example 4] Querying modules by type:");
    const auto& engineModules = FModuleManager::Get().GetModulesByType(EModuleType::Engine);
    LOG("Engine modules:");
    for (const auto& modName : engineModules)
    {
        LOG("  - " << modName);
    }

    const auto& gameModules = FModuleManager::Get().GetModulesByType(EModuleType::Game);
    LOG("Game modules:");
    for (const auto& modName : gameModules)
    {
        LOG("  - " << modName);
    }

    // 5. Using the Test Application Module for RHI testing
    // 5. 使用测试应用模块进行RHI测试
    LOG("\n[Example 5] Test Application Module (TestRHIApp):");
    LOG("TestRHIApp is now a separate application, not part of Core module.");
    LOG("To use it, build and run the TestRHIApp executable:");
    LOG("  xmake build TestRHIApp");
    LOG("  xmake run TestRHIApp");
    LOG("");
    LOG("TestRHIApp is structured like DefaultGame - as an independent application.");
    LOG("It demonstrates proper separation of application code from engine/core modules.");
    
    // Note: TestApplicationModule is no longer accessible from Core module
    // It's now a standalone application with its own executable
    // 注意：TestApplicationModule 现在不再能从 Core 模块访问
    // 它现在是一个独立的应用程序，有自己的可执行文件

    // 6. Plugin discovery and loading
    // 6. 插件发现和加载
    LOG("\n[Example 6] Plugin management:");
    
    // Discover plugins in a directory
    // 在目录中发现插件
    FString pluginDir = "./Plugins";
    FModuleManager::Get().DiscoverPlugins(pluginDir);
    
    // Get available plugins
    // 获取可用的插件
    auto plugins = FModuleManager::Get().GetAvailablePlugins();
    LOG("Available plugins: " << plugins.Num());
    for (size_t i = 0; i < plugins.Num(); ++i)
    {
        LOG("  - " << plugins[i].PluginName << " (" << plugins[i].FriendlyName << ")");
    }

    // Load a specific plugin
    // 加载特定插件
    // FModuleManager::Get().LoadPlugin("MyPlugin");

    // 7. Module dependencies (automatic loading)
    // 7. 模块依赖（自动加载）
    LOG("\n[Example 7] Module dependencies are handled automatically");
    LOG("When you load a module, its dependencies are loaded first");
    LOG("当你加载一个模块时，它的依赖项会首先被加载");

    LOG("\n=== End of Examples ===");
}

// Example 4: Creating a module with dependencies
// 示例4: 创建一个带依赖的模块
class FAdvancedModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        LOG("AdvancedModule: Starting up");
        
        // Access dependencies
        // 访问依赖项
        auto* rhiModule = FModuleManager::Get().GetModule("RHI");
        if (rhiModule)
        {
            LOG("AdvancedModule: RHI module is available");
        }
        
        auto* coreModule = FModuleManager::Get().GetModule("CoreEngine");
        if (coreModule)
        {
            LOG("AdvancedModule: Core engine module is available");
        }
    }

    virtual void ShutdownModule() override
    {
        LOG("AdvancedModule: Shutting down");
        // Dependencies are still available during shutdown
        // 依赖项在关闭期间仍然可用
    }
};

// Example 5: Best practices for module implementation
// 示例5: 模块实现的最佳实践
class FBestPracticeModule : public IModuleInterface
{
private:
    // Module-specific data
    struct ModuleData
    {
        bool bInitialized = false;
        FString ModuleName = "BestPracticeModule";
        // Add your module data here
    };
    
    ModuleData Data;

public:
    virtual void StartupModule() override
    {
        LOG("BestPracticeModule: Starting");
        
        // 1. Initialize module data
        Data.bInitialized = true;
        
        // 2. Load dependencies (handled by ModuleManager)
        // Dependencies are automatically loaded before this module
        
        // 3. Initialize subsystems
        InitializeSubsystems();
        
        // 4. Register callbacks or event handlers
        RegisterCallbacks();
        
        LOG("BestPracticeModule: Startup complete");
    }

    virtual void ShutdownModule() override
    {
        LOG("BestPracticeModule: Shutting down");
        
        // 1. Unregister callbacks or event handlers
        UnregisterCallbacks();
        
        // 2. Shutdown subsystems (in reverse order)
        ShutdownSubsystems();
        
        // 3. Cleanup module data
        Data.bInitialized = false;
        
        LOG("BestPracticeModule: Shutdown complete");
    }

    virtual bool SupportsDynamicReloading() override
    {
        // Return true if your module can be hot-reloaded
        return true;
    }

    virtual bool SupportsAutomaticShutdown() override
    {
        // Return true if your module should be shut down automatically
        return true;
    }

private:
    void InitializeSubsystems()
    {
        LOG("BestPracticeModule: Initializing subsystems");
        // Initialize your subsystems here
    }

    void ShutdownSubsystems()
    {
        LOG("BestPracticeModule: Shutting down subsystems");
        // Shutdown your subsystems here
    }

    void RegisterCallbacks()
    {
        LOG("BestPracticeModule: Registering callbacks");
        // Register your callbacks here
    }

    void UnregisterCallbacks()
    {
        LOG("BestPracticeModule: Unregistering callbacks");
        // Unregister your callbacks here
    }
};

/*
 * How to run these examples:
 * 如何运行这些示例:
 * 
 * 1. Include this file in your project
 *    在项目中包含此文件
 * 
 * 2. Call ExampleModuleUsage() from your application entry point
 *    从应用程序入口点调用 ExampleModuleUsage()
 * 
 * 3. Build and run your project
 *    构建并运行你的项目
 * 
 * Example in main.cpp or Launch.cpp:
 * 
 * #include "ModuleExamples.h"
 * 
 * int main()
 * {
 *     ExampleModuleUsage();
 *     // ... rest of your application code
 *     return 0;
 * }
 */
