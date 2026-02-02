#include "Modules/Module.h"
#include "Modules/EngineModules.h"

// Register Core Engine Module
IMPLEMENT_MODULE(FCoreEngineModule, CoreEngine)

// Register RHI Module as a standalone module
IMPLEMENT_MODULE(FRHIModule, RHI)
