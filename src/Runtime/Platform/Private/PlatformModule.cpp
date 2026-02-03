#include "Platform/PlatformModule.h"

namespace CarrotToy {
namespace Platform {

// Platform module implementation
class PlatformModule : public IPlatformModule {
public:
    bool initialize() override {
        return true;
    }
    
    void shutdown() override {
    }
    
    const char* getName() const override {
        return "Platform";
    }
};

// Module instantiation
IPlatformModule* createPlatformModule() {
    return new PlatformModule();
}

} // namespace Platform
} // namespace CarrotToy
