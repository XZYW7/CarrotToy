# Changelog

All notable changes to CarrotToy will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0] - 2026-01-06

### Added
- **Platform Abstraction Layer**: New cross-platform window and input management system
  - `Platform.h` and `PlatformTypes.h` interfaces for platform abstraction
  - GLFW-based implementation supporting Windows, Linux, and macOS
  - `IPlatform` interface for platform-wide operations
  - `IPlatformWindow` interface for window management
  - Factory function for platform creation
  - Display/monitor information queries
  - High-precision time queries
- **Platform Documentation**: Comprehensive `PLATFORM.md` documentation
- **Implementation Summary**: `PLATFORM_IMPLEMENTATION.md` detailing all changes

### Changed
- **RHI Structure Improvements**:
  - Removed direct GLFW dependencies from OpenGLRHI
  - RHI now expects OpenGL context initialized by Platform layer
  - Cleaner separation between RHI and platform concerns
- **Renderer Updates**:
  - Refactored to use Platform abstraction instead of direct GLFW calls
  - Replaced `GLFWwindow*` with `std::shared_ptr<Platform::IPlatformWindow>`
  - Added `getWindowHandle()` for native handle access
- **MaterialEditor Updates**:
  - Updated to use platform window handle through Renderer
- **Build System**:
  - Updated `xmake.lua` to include Platform sources
- **Documentation**:
  - Updated `README.md` with Platform abstraction information
  - Updated `ARCHITECTURE.md` with new Platform layer
  - Enhanced architecture diagrams and component descriptions

### Removed
- Direct GLFW initialization code from OpenGLRHI
- Commented-out code from RHIExample.cpp
- Unnecessary GLFW includes from RHI layer

### Fixed
- Better separation of concerns between platform and rendering code
- Improved code organization and maintainability

## [0.1.0] - 2025-12-23

### Added
- Initial release of CarrotToy Material Editor
- Core rendering system with OpenGL 3.3+
- Material system with shader parameter management
- Real-time shader editing and hot-reloading
- ImGui-based material editor interface
- Forward rendering pipeline with PBR support
- CPU-based offline ray tracer
- Material preview with rotating sphere
- Default PBR shader implementation
- Unlit shader template
- Xmake build system configuration
- CMake build system as alternative
- Comprehensive documentation:
  - README with feature overview
  - BUILD guide for compilation
  - EXAMPLES with code samples
  - TUTORIAL for step-by-step learning
  - ARCHITECTURE for system design
  - QUICKSTART for rapid onboarding
- Material parameter types:
  - Float values
  - Vec2/Vec3/Vec4 vectors
  - Integer values
  - Boolean values
  - Texture2D support (placeholder)
  - Matrix4 support
- Material manager singleton for global access
- Scene export for ray tracing
- Shader compilation error reporting
- Cross-platform support (Windows, Linux, macOS)

### Features
- Runtime shader compilation and recompilation
- Material parameter editing via ImGui
- Multiple render modes (Rasterization, Ray Tracing)
- Automatic shader hot-reloading
- PBR material properties (albedo, metallic, roughness)
- Interactive material preview
- Sphere geometry generation for preview
- Basic lighting system
- Ray-sphere intersection for ray tracing
- PNG output for ray-traced images

### Technical Details
- C++17 implementation
- OpenGL 3.3+ core profile
- GLFW for window management
- GLAD for OpenGL loading
- GLM for mathematics
- ImGui for UI
- STB for image I/O
- Physically-based rendering (Cook-Torrance BRDF)
- GGX distribution for specular highlights
- Schlick approximation for Fresnel effect

## [Unreleased]

### Planned Features
- [ ] Texture loading and display
- [ ] More geometry types (cube, plane, custom meshes)
- [ ] Material save/load to JSON
- [ ] Shader preset library
- [ ] Node-based shader editor
- [ ] Deferred rendering pipeline
- [ ] Shadow mapping
- [ ] Post-processing effects
- [ ] SSAO (Screen Space Ambient Occlusion)
- [ ] HDR and tone mapping options
- [ ] Real-time ray tracing (DXR/OptiX)
- [ ] BVH acceleration for ray tracing
- [ ] Multi-threaded ray tracing
- [ ] Progressive rendering
- [ ] Material animation system
- [ ] Custom geometry import (OBJ, FBX)
- [ ] Multiple light sources
- [ ] Light editor UI
- [ ] Environment maps (HDR)
- [ ] IBL (Image-Based Lighting)
- [ ] Material instancing
- [ ] Performance profiler
- [ ] Shader debugging tools
- [ ] Material graph visualization
- [ ] Export to various formats
- [ ] Plugin system
- [ ] Scripting support (Lua/Python)

### Known Issues
- Ray tracer is CPU-only (slow for high resolutions)
- Limited geometry types (sphere only)
- No texture support yet
- Single light source
- No shadows
- Limited error recovery in shader compilation
- No undo/redo for material edits
- Cannot save/load projects

### Future Improvements
- Better error messages for shader compilation
- Async shader compilation
- Material preview caching
- Optimization for large material counts
- GPU ray tracing integration
- Better memory management for parameters
- Material inheritance/variants
- Shader include system
- Profiling and performance metrics
- Unit tests
- Integration tests
- CI/CD pipeline

## Version History

### Version Numbering
- **0.x.y** - Pre-release versions
  - 0.1.0 - Initial implementation
  - 0.2.0 - Planned: Texture support
  - 0.3.0 - Planned: Node-based editor
- **1.x.y** - Stable releases
  - 1.0.0 - Planned: First stable release

### Breaking Changes
None yet (pre-1.0)

### Deprecations
None yet

### Security
No security issues reported

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for details on how to contribute to this project.

## License

This project is available for educational purposes.

## Acknowledgments

- Inspired by Unreal Engine and Unity material editors
- PBR implementation based on LearnOpenGL tutorials
- Built with Xmake build system
- Uses ImGui for immediate mode GUI
