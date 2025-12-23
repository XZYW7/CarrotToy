# Changelog

All notable changes to CarrotToy will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
