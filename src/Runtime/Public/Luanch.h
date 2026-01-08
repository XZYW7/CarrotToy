#pragma once

#include "CoreUtils.h"
#include <memory>
#include <chrono>

namespace CarrotToy {
class Renderer;
class MaterialEditor;
class Material;
}

class FMainLoop
{
public:
	/** Default constructor. */
	FMainLoop();
	~FMainLoop();

	// Main loop entry point
	bool PreInit(int argc, char** argv); // parse args, init Path, basic platform
	bool Init();                        // initialize renderer/editor/resources
	void Tick();                        // called repeatedly in loop
	void Exit();                        // shutdown/cleanup

public:
	bool ShouldExit = false;

protected:
	// Timing / profiling
	std::chrono::high_resolution_clock::time_point LastTime;
	double Accumulator = 0.0; // seconds
	uint64_t FrameCounter = 0;
	std::vector<float> FrameTimes; // ms
	double TotalTickTime = 0.0;

	// Fixed timestep target (seconds)
	double FixedDt = 1.0 / 60.0;
	// Max accumulated time to avoid spiral of death
	double MaxAccumulatorSeconds = 0.5;

	// Owned engine subsystems
	std::unique_ptr<CarrotToy::Renderer> renderer;
	std::unique_ptr<CarrotToy::MaterialEditor> editor;
	std::shared_ptr<CarrotToy::Material> defaultMaterial;
};