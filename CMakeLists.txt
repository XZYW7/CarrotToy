cmake_minimum_required(VERSION 3.15)
project(CarrotToy VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIR}
)

# Source files
set(SOURCES
    src/main.cpp
    src/Material.cpp
    src/Renderer.cpp
    src/MaterialEditor.cpp
    src/RayTracer.cpp
)

# Header files
set(HEADERS
    include/Material.h
    include/Renderer.h
    include/MaterialEditor.h
    include/RayTracer.h
)

# Create executable
add_executable(CarrotToy ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(CarrotToy PRIVATE
    OpenGL::GL
    glfw
    glad::glad
    glm::glm
    imgui::imgui
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(CarrotToy PRIVATE opengl32 gdi32 user32 shell32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(CarrotToy PRIVATE GL pthread dl X11)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(CarrotToy PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
endif()

# Copy shaders to build directory
add_custom_command(TARGET CarrotToy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:CarrotToy>/shaders
    COMMENT "Copying shaders to build directory"
)

# Installation rules
install(TARGETS CarrotToy DESTINATION bin)
install(DIRECTORY shaders/ DESTINATION bin/shaders)

# Print configuration
message(STATUS "CarrotToy Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  OpenGL: ${OPENGL_LIBRARIES}")
