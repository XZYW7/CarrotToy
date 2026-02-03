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

namespace CarrotToy {
namespace Input {

// Input module interface
class IInputModule {
public:
    virtual ~IInputModule() = default;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual const char* getName() const = 0;
};

// Factory function to create input module
INPUT_API IInputModule* createInputModule();

} // namespace Input
} // namespace CarrotToy
