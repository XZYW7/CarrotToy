#pragma once

#ifndef INPUT_API
    #if defined(_WIN32) || defined(_WIN64)
        #ifdef INPUT_BUILD_SHARED
            #define INPUT_API __declspec(dllexport)
        #elif defined(INPUT_IMPORT_SHARED)
            #define INPUT_API __declspec(dllimport)
        #else
            #define INPUT_API
        #endif
    #else
        #define INPUT_API
    #endif
#endif

#include "Modules/ModuleInterface.h"

#ifndef INPUT_API
// ... existing code ...
#endif

namespace CarrotToy {
namespace Input {

// Input module interface
class IInputModule : public IModuleInterface {
public:
    virtual ~IInputModule() = default;
    
    // Legacy support (optional, can be removed if specific IInputModule features aren't used externally)
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual const char* getName() const = 0;
};

// Factory function to create input module
INPUT_API IInputModule* createInputModule();

} // namespace Input
} // namespace CarrotToy

/**
 * Initialize the Input module and register it with ModuleManager.
 * Must be called explicitly to ensure proper DLL loading and module registration.
 */
extern "C" INPUT_API void InitializeModuleInput();
