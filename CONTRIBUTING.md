# Contributing to CarrotToy

Thank you for your interest in contributing to CarrotToy! This document provides guidelines and instructions for contributing.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [How to Contribute](#how-to-contribute)
- [Development Setup](#development-setup)
- [Coding Standards](#coding-standards)
- [Commit Guidelines](#commit-guidelines)
- [Pull Request Process](#pull-request-process)
- [Testing](#testing)
- [Documentation](#documentation)

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors.

### Expected Behavior

- Be respectful and considerate
- Welcome newcomers and help them get started
- Provide constructive feedback
- Focus on what is best for the project

### Unacceptable Behavior

- Harassment or discrimination
- Trolling or insulting comments
- Publishing others' private information
- Other unprofessional conduct

## Getting Started

### Prerequisites

- C++17 compatible compiler
- Xmake or CMake
- Git
- Basic knowledge of OpenGL and graphics programming

### Fork and Clone

```bash
# Fork the repository on GitHub
# Then clone your fork
git clone https://github.com/YOUR_USERNAME/CarrotToy.git
cd CarrotToy

# Add upstream remote
git remote add upstream https://github.com/XZYW7/CarrotToy.git
```

### Build and Test

```bash
# Build the project
xmake

# Run the application
xmake run CarrotToy

# Verify everything works
```

## How to Contribute

### Reporting Bugs

Before creating a bug report:
- Check if the bug has already been reported
- Verify you're using the latest version
- Collect relevant information (OS, compiler, error messages)

Create a bug report including:
- **Title**: Clear, descriptive title
- **Description**: Detailed description of the issue
- **Steps to Reproduce**: Numbered steps to reproduce the bug
- **Expected Behavior**: What should happen
- **Actual Behavior**: What actually happens
- **Environment**: OS, compiler version, dependencies
- **Screenshots**: If applicable
- **Logs**: Relevant error messages or console output

### Suggesting Features

Feature suggestions are welcome! Include:
- **Use Case**: Why this feature is needed
- **Description**: Detailed description of the feature
- **Examples**: How it would be used
- **Alternatives**: Other solutions you've considered
- **References**: Similar implementations in other tools

### Contributing Code

Areas where contributions are especially welcome:
- Bug fixes
- Performance improvements
- New shader examples
- Material presets
- Documentation improvements
- Test coverage
- Platform-specific fixes
- UI/UX improvements

## Development Setup

### Project Structure

```
CarrotToy/
├── src/                # Source files (.cpp)
├── include/            # Header files (.h)
├── shaders/            # GLSL shader files
├── assets/             # Asset files (future)
├── docs/               # Additional documentation (future)
└── tests/              # Test files (future)
```

### Building from Source

```bash
# Debug build
xmake f -m debug
xmake

# Release build
xmake f -m release
xmake

# Verbose output
xmake -v
```

### Development Workflow

1. Create a feature branch
   ```bash
   git checkout -b feature/my-feature
   ```

2. Make your changes
   - Write code
   - Add tests (if applicable)
   - Update documentation

3. Test your changes
   ```bash
   xmake
   xmake run CarrotToy
   ```

4. Commit your changes
   ```bash
   git add .
   git commit -m "Add: description of changes"
   ```

5. Push to your fork
   ```bash
   git push origin feature/my-feature
   ```

6. Create a Pull Request

## Coding Standards

### C++ Style Guide

- **Naming Conventions**:
  - Classes: `PascalCase` (e.g., `MaterialManager`)
  - Functions: `camelCase` (e.g., `createMaterial`)
  - Variables: `camelCase` (e.g., `materialName`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_MATERIALS`)
  - Private members: no prefix or `m_` prefix

- **Formatting**:
  - Indentation: 4 spaces (no tabs)
  - Line length: 100 characters max (flexible)
  - Braces: Same line for functions, classes
  - Spaces: Around operators, after commas

- **Best Practices**:
  - Use `const` where appropriate
  - Prefer `auto` for complex types
  - Use smart pointers (`shared_ptr`, `unique_ptr`)
  - Avoid raw `new`/`delete`
  - Use RAII for resource management
  - Document public APIs

### Example Code Style

```cpp
// Good
class MaterialManager {
public:
    std::shared_ptr<Material> createMaterial(const std::string& name, 
                                            std::shared_ptr<Shader> shader) {
        auto material = std::make_shared<Material>(name, shader);
        materials[name] = material;
        return material;
    }
    
private:
    std::map<std::string, std::shared_ptr<Material>> materials;
};

// Bad
class MaterialManager {
public:
    Material* CreateMaterial(std::string name,Shader* shader){
        Material* m=new Material(name,shader);
        materials[name]=m;
        return m;
    }
private:
    std::map<std::string,Material*> materials;
};
```

### GLSL Style Guide

- Version directive first: `#version 330 core`
- Consistent indentation
- Meaningful variable names
- Comments for complex calculations
- Group related uniforms

## Commit Guidelines

### Commit Message Format

```
Type: Short description (50 chars or less)

Optional longer description explaining the change in detail.
Can include multiple paragraphs.

Fixes #123
```

### Commit Types

- **Add**: New feature or file
- **Fix**: Bug fix
- **Update**: Update existing feature
- **Refactor**: Code restructuring without changing behavior
- **Docs**: Documentation changes
- **Style**: Code formatting, whitespace
- **Test**: Adding or updating tests
- **Build**: Build system changes
- **Perf**: Performance improvements

### Examples

```
Add: Material texture support with UV mapping

Implements texture loading and display in materials.
Adds UV coordinates to sphere geometry.
Updates shader to sample textures.

Fixes #42
```

```
Fix: Shader compilation error handling

Properly handle shader compilation failures.
Display error messages in console and UI.
```

## Pull Request Process

### Before Submitting

- [ ] Code builds without errors
- [ ] Code follows style guidelines
- [ ] Documentation is updated
- [ ] Commit messages follow guidelines
- [ ] Changes are tested
- [ ] No unrelated changes included

### PR Description Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
How were these changes tested?

## Checklist
- [ ] Code builds successfully
- [ ] Documentation updated
- [ ] Follows coding standards
- [ ] All tests pass (if applicable)

## Screenshots (if applicable)
Add screenshots to demonstrate changes
```

### Review Process

1. Maintainers will review your PR
2. Feedback may be provided
3. Make requested changes
4. Once approved, PR will be merged

### Merge Requirements

- At least one approval from maintainer
- All checks passing
- No merge conflicts
- Documentation updated

## Testing

### Manual Testing

When adding features:
1. Test the specific feature
2. Test integration with existing features
3. Test on different platforms (if possible)
4. Test edge cases

### Test Checklist

- [ ] Application starts without errors
- [ ] UI is responsive
- [ ] Materials can be created/edited
- [ ] Shaders compile successfully
- [ ] Preview renders correctly
- [ ] No memory leaks
- [ ] Performance is acceptable

## Documentation

### Code Documentation

- Document all public APIs
- Explain complex algorithms
- Add examples where helpful
- Keep documentation up-to-date

### Example Documentation

```cpp
/**
 * Creates a new material with the specified shader.
 * 
 * @param name The unique name for the material
 * @param shader The shader to use for rendering
 * @return Shared pointer to the created material
 * 
 * Example:
 * @code
 * auto shader = std::make_shared<Shader>("vert.glsl", "frag.glsl");
 * auto material = manager.createMaterial("MyMaterial", shader);
 * @endcode
 */
std::shared_ptr<Material> createMaterial(const std::string& name, 
                                        std::shared_ptr<Shader> shader);
```

### Documentation Files

Update relevant documentation:
- `README.md` - Overview and features
- `BUILD.md` - Build instructions
- `EXAMPLES.md` - Code examples
- `TUTORIAL.md` - User tutorials
- `ARCHITECTURE.md` - System design
- `CHANGELOG.md` - Version history

## Questions?

- Open an issue for questions
- Check existing documentation
- Review closed issues/PRs for similar topics

## Recognition

Contributors will be recognized in:
- README contributors section
- CHANGELOG for significant contributions
- Git commit history

Thank you for contributing to CarrotToy! 🎉
