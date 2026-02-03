#include "Input/InputModule.h"
#include "Modules/Module.h"
#include "CoreUtils.h"

namespace CarrotToy {
namespace Input {

// Input module implementation
class InputModule : public IInputModule {
public:
    bool initialize() override {
        LOG("InputModule: Startup");
        return true;
    }
    
    void shutdown() override {
        LOG("InputModule: Shutdown");
    }
    
    const char* getName() const override {
        return "Input";
    }
    
    // Module interface implementation
    void StartupModule() { initialize(); }
    void ShutdownModule() { shutdown(); }
};

// Module instantiation (Legacy, kept for compatibility if needed)
IInputModule* createInputModule() {
    return new InputModule();
}

// Register Input Module using INPUT_API export macro
IMPLEMENT_MODULE_WITH_API(InputModule, Input, INPUT_API)

} // namespace Input
} // namespace CarrotToy
