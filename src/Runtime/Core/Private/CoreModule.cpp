#include "Modules/Module.h"
#include "Modules/EngineModules.h"

// Register Core Engine Module
// Note: RHI is not registered as a module - it's linked normally as part of the Core library
IMPLEMENT_MODULE(FCoreEngineModule, CoreEngine)
