#pragma once
#include "RHITypes.h"

/**
 * Initialize the RHI module and register it with ModuleManager.
 * Must be called explicitly to ensure proper DLL loading and module registration.
 */
extern "C" RHI_API void InitializeModuleRHI();
