#include "GameModules.h"
#include "Modules/Module.h"
#include "CoreUtils.h"

// Game Module Implementation
void FGameModule::StartupModule()
{
	LOG("GameModule: Startup");
	LOG("GameModule: Initializing game-specific systems");
	// Initialize game systems: asset loading, UI, etc.
}

void FGameModule::ShutdownModule()
{
	LOG("GameModule: Shutdown");
	LOG("GameModule: Shutting down game-specific systems");
	// Clean up game systems
}

// Gameplay Module Implementation
void FGameplayModule::StartupModule()
{
	LOG("GameplayModule: Startup");
	LOG("GameplayModule: Initializing gameplay systems");
	// Initialize gameplay systems: player controller, game mode, etc.
}

void FGameplayModule::ShutdownModule()
{
	LOG("GameplayModule: Shutdown");
	LOG("GameplayModule: Shutting down gameplay systems");
	// Clean up gameplay systems
}

// Register the game modules
IMPLEMENT_GAME_MODULE(FGameModule, DefaultGame)
IMPLEMENT_GAME_MODULE(FGameplayModule, GameplayModule)
