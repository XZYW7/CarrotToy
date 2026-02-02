#pragma once
#include "CoreUtils.h"

/**
 * Describes the type of module
 */
enum class EModuleType
{
	/** Module is part of the core engine runtime */
	Engine,
	
	/** Module is part of the game project */
	Game,
	
	/** Module is a plugin that can be loaded/unloaded dynamically */
	Plugin,
	
	/** Module is an application entry point */
	Application
};

/**
 * Describes loading phase for module initialization
 */
enum class ELoadingPhase
{
	/** Loaded at the start of PreInit, before anything else */
	PreInit,
	
	/** Loaded after PreInit, before default modules */
	Default,
	
	/** Loaded after default modules during Init */
	PostDefault,
	
	/** Loaded after engine initialization */
	PostEngineInit
};

/**
 * Module descriptor containing metadata about a module
 */
struct FModuleDescriptor
{
	/** Name of the module */
	FName ModuleName;
	
	/** Type of module */
	EModuleType Type = EModuleType::Engine;
	
	/** Loading phase */
	ELoadingPhase LoadingPhase = ELoadingPhase::Default;
	
	/** Whether this module can be unloaded at runtime */
	bool bCanUnload = true;
	
	/** List of modules this module depends on */
	TArray<FName> Dependencies;
	
	FModuleDescriptor() = default;
	
	FModuleDescriptor(const FName& InName, EModuleType InType = EModuleType::Engine)
		: ModuleName(InName), Type(InType)
	{
	}
};

/**
 * Plugin descriptor containing metadata about a plugin
 */
struct FPluginDescriptor
{
	/** Name of the plugin */
	FName PluginName;
	
	/** Friendly display name */
	FString FriendlyName;
	
	/** Version of the plugin */
	FString Version;
	
	/** Description of what this plugin does */
	FString Description;
	
	/** Plugin author/company */
	FString CreatedBy;
	
	/** Whether this plugin is enabled by default */
	bool bEnabledByDefault = true;
	
	/** Can this plugin contain content (assets, etc.) */
	bool bCanContainContent = false;
	
	/** Modules included in this plugin */
	TArray<FModuleDescriptor> Modules;
	
	FPluginDescriptor() = default;
	
	FPluginDescriptor(const FName& InName) : PluginName(InName)
	{
		FriendlyName = InName;
	}
};
