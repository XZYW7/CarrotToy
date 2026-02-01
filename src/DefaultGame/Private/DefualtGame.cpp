#include "Launch.h"
#include "Modules/Module.h"
#include "DefaultGameModule.h"
#include "CoreUtils.h"

void FDefaultGameModule::StartupModule()
{
	LOG("DefaultGameModule: Startup");
	LOG("DefaultGameModule: This is the CarrotToy default game/editor application");
}

void FDefaultGameModule::ShutdownModule()
{
	LOG("DefaultGameModule: Shutdown");
}

IMPLEMENT_APPLICATION_MODULE(FDefaultGameModule, DefaultGame, "CarrotToyProject")