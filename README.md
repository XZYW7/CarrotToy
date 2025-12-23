# CarrotToy

A material editor lab implementation inspired by Unreal and Unity, featuring runtime shader editing capabilities. Built with Xmake, supporting both real-time rasterization and offline ray tracing.

## Features

- **Material System**: Create and manage materials with customizable shader parameters
- **Runtime Shader Editing**: Edit and recompile shaders in real-time without restarting the application
- **Real-time Rasterization**: OpenGL-based forward rendering with PBR (Physically Based Rendering) shaders
- **Offline Ray Tracing**: CPU-based ray tracing for high-quality offline rendering
- **Material Editor UI**: ImGui-based interface for intuitive material editing
- **Hot Reload**: Automatic shader reloading when files are modified
- **PBR Materials**: Built-in physically-based rendering shader support

## Architecture

```
CarrotToy/
├── src/               # Source files
│   ├── main.cpp       # Application entry point
│   ├── Material.cpp   # Material and shader system
│   ├── Renderer.cpp   # Rendering engine (rasterization)
│   ├── MaterialEditor.cpp  # ImGui-based editor interface
│   └── RayTracer.cpp  # Offline ray tracing implementation
├── include/           # Header files
│   ├── Material.h
│   ├── Renderer.h
│   ├── MaterialEditor.h
│   └── RayTracer.h
├── shaders/           # GLSL shader files
│   ├── default.vert   # Default vertex shader
│   ├── default.frag   # PBR fragment shader
│   ├── unlit.vert     # Unlit vertex shader
│   └── unlit.frag     # Unlit fragment shader
└── xmake.lua          # Xmake build configuration
```

## Requirements

- C++17 compatible compiler
- Xmake build system
- OpenGL 3.3+
- GLFW
- GLAD
- GLM
- ImGui
- STB (for image loading/saving)

## Building

### Install Xmake

Follow the [Xmake installation guide](https://xmake.io/#/guide/installation).

### Build the project

```bash
# Configure and build
xmake

# Run the application
xmake run CarrotToy
```

### Build options

```bash
# Build in debug mode
xmake f -m debug
xmake

# Build in release mode
xmake f -m release
xmake

# Clean build
xmake clean
```

## Usage

### Material Editor Interface

The application provides several UI panels:

1. **Materials Panel**: List all materials, create new materials
2. **Material Properties**: Edit shader parameters (albedo, metallic, roughness, etc.)
3. **Shader Editor**: Edit vertex and fragment shaders in real-time
4. **Material Preview**: Preview the material on a 3D sphere with rotation

### Creating a Material

1. Click "Create New Material" in the Materials panel
2. Enter a name for your material
3. Select or create a shader
4. Adjust material parameters in the Properties panel

### Editing Shaders

1. Select a material from the Materials panel
2. Click "Edit Shader" in Material Properties
3. Modify the vertex or fragment shader code
4. Click "Compile and Apply" to see changes immediately

### Render Modes

- **Rasterization**: Real-time OpenGL rendering (default)
- **Ray Tracing**: Offline CPU-based ray tracing for high-quality output

### Example Material Parameters

**PBR Material:**
- `albedo` (vec3): Base color of the material
- `metallic` (float): Metallic property (0.0 = dielectric, 1.0 = metal)
- `roughness` (float): Surface roughness (0.0 = smooth, 1.0 = rough)

## Shader System

### Default PBR Shader

The default shader implements a physically-based rendering model with:
- Cook-Torrance BRDF
- GGX distribution
- Schlick-Beckmann geometry function
- Fresnel-Schlick approximation

### Custom Shaders

Create custom shaders by:
1. Adding `.vert` and `.frag` files to the `shaders/` directory
2. Loading them through the Material Editor
3. Defining custom uniforms and material parameters

### Shader Hot-Reloading

Shaders can be reloaded at runtime:
- Through the Shader Editor UI
- By calling `shader->reload()` programmatically
- Changes take effect immediately without application restart

## Ray Tracing

The offline ray tracer supports:
- Basic geometry (spheres, meshes)
- Simple lighting calculations
- Scene export from rasterization view
- PNG output format

To perform offline ray tracing:
1. Set up your scene in rasterization mode
2. Click "Export Scene for Ray Tracing"
3. Use the RayTracer API to render the scene

## Extending CarrotToy

### Adding New Material Parameters

```cpp
material->setFloat("myParameter", 1.0f);
material->setVec3("myColor", 1.0f, 0.0f, 0.0f);
```

### Creating Custom Materials

```cpp
auto shader = std::make_shared<Shader>("path/to/vertex.vert", "path/to/fragment.frag");
auto material = MaterialManager::getInstance().createMaterial("MyMaterial", shader);
```

### Implementing New Render Features

Extend the `Renderer` class to add:
- Deferred rendering
- Post-processing effects
- Advanced lighting systems
- Shadow mapping

## License

This project is open source and available for educational purposes.

## Acknowledgments

- Inspired by Unreal Engine and Unity material editors
- Built with Xmake build system
- Uses ImGui for the editor interface
- PBR implementation based on LearnOpenGL tutorials