#include "Renderer.h"
#include "Shader.h"
#include "Material.h"
#include "MaterialEditor.h"
#include "RayTracer.h"
#include <iostream>
#include <memory>
#include "Misc/Path.h"
#include "Luanch.h"
using namespace CarrotToy;

int main(int argc, char** argv) {
    /*
    启动阶段顺序（推荐）：
Parse command-line & env -> Path::Set* / config

Platform / Window / Context 创建（必要时）

RHI device 初始化（需要依赖当前 GL/VK context）

Logger / Profiler 初始化（先于后续模块）

Resource managers / Asset registry

Renderer / UI / Input / Editor 等
    */
    FMainLoop mainLoop;
    if(!mainLoop.PreInit(argc, argv))
    {
        std::cerr << "PreInit failed\n";
        return -1;
    }
    if(!mainLoop.Init())
    {
        std::cerr << "Init failed\n";
        return -1;
    }

    while(!mainLoop.ShouldExit)
    {
        mainLoop.Tick();
    }
    mainLoop.Exit();
    return 0;
}
