#pragma once

/**
 * Force RHI module DLL to be loaded by referencing this function.
 * This ensures the module's global constructors run and IMPLEMENT_MODULE registers it.
 */
extern "C" void ForceRHIModuleLoad();
