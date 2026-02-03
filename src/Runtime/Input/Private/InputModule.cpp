#include "Input/InputModule.h"

namespace CarrotToy {
namespace Input {

// Input module implementation
class InputModule : public IInputModule {
public:
    bool initialize() override {
        return true;
    }
    
    void shutdown() override {
    }
    
    const char* getName() const override {
        return "Input";
    }
};

// Module instantiation
IInputModule* createInputModule() {
    return new InputModule();
}

} // namespace Input
} // namespace CarrotToy
