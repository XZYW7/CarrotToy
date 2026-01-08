#include "Luanch.h"
#include "Misc/Path.h"
#include "Renderer.h"
#include "Shader.h"
#include "Material.h"
#include "MaterialEditor.h"
#include <iostream>
using namespace CarrotToy;
FMainLoop::FMainLoop()
{
    // nothing heavy here
}

FMainLoop::~FMainLoop()
{
    // ensure cleanup
    Exit();
}

bool FMainLoop::PreInit(int argc, char** argv)
{
    // Initialize Path globals from command line / environment once at startup
    Path::InitFromCmdLineAndEnv(argc, const_cast<const char**>(argv));

    std::cout << "launchDir " << Path::LaunchDir() << std::endl;
    std::cout << "projectDir " << Path::ProjectDir() << std::endl;
    std::cout << "shaderWorkingDir " << Path::ShaderWorkingDir() << std::endl;

    // Initialize timing
    LastTime = std::chrono::high_resolution_clock::now();
    Accumulator = 0.0;
    FrameCounter = 0;
    FrameTimes.clear();
    TotalTickTime = 0.0;

    return true;
}

bool FMainLoop::Init()
{
    try {
        // Initialize renderer
        renderer = std::make_unique<CarrotToy::Renderer>();
        if (!renderer->initialize(1280, 720, "CarrotToy - Material Editor")) {
            std::cerr << "Failed to initialize renderer" << std::endl;
            return false;
        }

        // Create default shader
        auto defaultShader = std::make_shared<CarrotToy::Shader>(
            "shaders/default.vs.spv",
            "shaders/default.ps.spv"
        );

        // Create default material
        defaultMaterial = CarrotToy::MaterialManager::getInstance().createMaterial(
            "DefaultPBR",
            defaultShader
        );
        defaultMaterial->setVec3("albedo", 0.8f, 0.2f, 0.2f);
        defaultMaterial->setFloat("metallic", 0.5f);
        defaultMaterial->setFloat("roughness", 0.5f);

        // Initialize material editor
        editor = std::make_unique<CarrotToy::MaterialEditor>();
        if (!editor->initialize(renderer.get())) {
            std::cerr << "Failed to initialize material editor" << std::endl;
            return false;
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in Init(): " << e.what() << std::endl;
        return false;
    }
    return true;
}

void FMainLoop::Tick()
{
    using clock = std::chrono::high_resolution_clock;
    auto now = clock::now();
    std::chrono::duration<double> frameDelta = now - LastTime;
    LastTime = now;

    double dt = frameDelta.count();
    // clamp very large dt (e.g., when debugger stops)
    if (dt > MaxAccumulatorSeconds) dt = MaxAccumulatorSeconds;

    Accumulator += dt;

    // Profiling: start frame timer
    auto profStart = clock::now();

    // Process fixed-step updates
    while (Accumulator >= FixedDt) {
        // TODO: update game logic / simulation here with FixedDt
        // e.g., physics.update(FixedDt);

        Accumulator -= FixedDt;
    }

    // Interpolation factor for rendering
    double alpha = (FixedDt > 0.0) ? (Accumulator / FixedDt) : 0.0;

    // Render
    if (renderer) {
        // begin frame / draw calls
        renderer->beginFrame();

        // pick preview material
        auto selected = (editor && editor->getSelectedMaterial()) ? editor->getSelectedMaterial() : defaultMaterial;
        if (selected) renderer->renderMaterialPreview(selected);

        // UI
        if (editor) editor->render();

        renderer->endFrame();

        // Check for window close
        if (renderer->shouldClose()) {
            ShouldExit = true;
        }
    }

    // Profiling: end frame timer
    auto profEnd = clock::now();
    std::chrono::duration<double, std::milli> frameMs = profEnd - profStart;
    double frameTimeMs = frameMs.count();
    FrameTimes.push_back(static_cast<float>(frameTimeMs));
    TotalTickTime += frameTimeMs;
    FrameCounter++;

    // Periodically print simple stats
    if ((FrameCounter % 120) == 0) {
        double avg = 0.0;
        for (float f : FrameTimes) avg += f;
        if (!FrameTimes.empty()) avg /= FrameTimes.size();
        std::cout << "Frame " << FrameCounter << " avg ms=" << avg << " last ms=" << frameTimeMs << std::endl;
        // cap stored frames to the last N
        if (FrameTimes.size() > 1000) FrameTimes.erase(FrameTimes.begin(), FrameTimes.begin() + (FrameTimes.size() - 1000));
    }
}

void FMainLoop::Exit()
{
    // reverse-order cleanup
    if (editor) {
        editor->shutdown();
        editor.reset();
    }
    if (renderer) {
        renderer->shutdown();
        renderer.reset();
    }
    defaultMaterial.reset();
}