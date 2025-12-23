#include "RayTracer.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace CarrotToy {

RayTracer::RayTracer() 
    : maxBounces(4), samplesPerPixel(1) {
}

RayTracer::~RayTracer() {
}

bool RayTracer::loadScene(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open scene file: " << path << std::endl;
        return false;
    }
    
    // Simple scene format parsing
    // Format: vertex count, vertices, index count, indices
    int vertexCount, indexCount;
    file >> vertexCount;
    
    scene.vertices.resize(vertexCount * 3);
    for (int i = 0; i < vertexCount * 3; ++i) {
        file >> scene.vertices[i];
    }
    
    file >> indexCount;
    scene.indices.resize(indexCount);
    for (int i = 0; i < indexCount; ++i) {
        file >> scene.indices[i];
    }
    
    return true;
}

void RayTracer::render(int width, int height, const std::string& outputPath) {
    std::cout << "Ray tracing scene: " << width << "x" << height << std::endl;
    
    std::vector<unsigned char> imageData(width * height * 3);
    
    // Simple camera setup
    float aspectRatio = (float)width / (float)height;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float u = (float)x / (float)width;
            float v = (float)y / (float)height;
            
            // Generate ray
            Ray ray;
            ray.origin[0] = 0.0f;
            ray.origin[1] = 0.0f;
            ray.origin[2] = 3.0f;
            
            ray.direction[0] = (u - 0.5f) * aspectRatio;
            ray.direction[1] = (0.5f - v);
            ray.direction[2] = -1.0f;
            
            // Normalize direction
            float len = sqrt(ray.direction[0] * ray.direction[0] + 
                           ray.direction[1] * ray.direction[1] + 
                           ray.direction[2] * ray.direction[2]);
            ray.direction[0] /= len;
            ray.direction[1] /= len;
            ray.direction[2] /= len;
            
            // Trace ray
            Hit hit = traceRay(ray, scene);
            
            float color[3] = {0.0f, 0.0f, 0.0f};
            if (hit.hit) {
                computeColor(hit, color);
            } else {
                // Sky color
                float t = 0.5f * (ray.direction[1] + 1.0f);
                color[0] = (1.0f - t) * 1.0f + t * 0.5f;
                color[1] = (1.0f - t) * 1.0f + t * 0.7f;
                color[2] = (1.0f - t) * 1.0f + t * 1.0f;
            }
            
            int index = (y * width + x) * 3;
            imageData[index + 0] = (unsigned char)(std::clamp(color[0], 0.0f, 1.0f) * 255);
            imageData[index + 1] = (unsigned char)(std::clamp(color[1], 0.0f, 1.0f) * 255);
            imageData[index + 2] = (unsigned char)(std::clamp(color[2], 0.0f, 1.0f) * 255);
        }
    }
    
    // Save image
    stbi_write_png(outputPath.c_str(), width, height, 3, imageData.data(), width * 3);
    std::cout << "Ray traced image saved to: " << outputPath << std::endl;
}

RayTracer::Hit RayTracer::traceRay(const Ray& ray, const Scene& scene) {
    Hit result;
    result.hit = false;
    result.t = 1e30f;
    
    // Simple sphere intersection for now
    // Center at origin, radius 1
    float a = ray.direction[0] * ray.direction[0] + 
              ray.direction[1] * ray.direction[1] + 
              ray.direction[2] * ray.direction[2];
    float b = 2.0f * (ray.direction[0] * ray.origin[0] + 
                     ray.direction[1] * ray.origin[1] + 
                     ray.direction[2] * ray.origin[2]);
    float c = ray.origin[0] * ray.origin[0] + 
              ray.origin[1] * ray.origin[1] + 
              ray.origin[2] * ray.origin[2] - 1.0f;
    
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant >= 0) {
        float t = (-b - sqrt(discriminant)) / (2.0f * a);
        if (t > 0.001f) {
            result.hit = true;
            result.t = t;
            
            result.position[0] = ray.origin[0] + t * ray.direction[0];
            result.position[1] = ray.origin[1] + t * ray.direction[1];
            result.position[2] = ray.origin[2] + t * ray.direction[2];
            
            result.normal[0] = result.position[0];
            result.normal[1] = result.position[1];
            result.normal[2] = result.position[2];
            
            float nlen = sqrt(result.normal[0] * result.normal[0] + 
                            result.normal[1] * result.normal[1] + 
                            result.normal[2] * result.normal[2]);
            result.normal[0] /= nlen;
            result.normal[1] /= nlen;
            result.normal[2] /= nlen;
        }
    }
    
    return result;
}

void RayTracer::computeColor(const Hit& hit, float* color) {
    // Simple shading: use normal as color
    float lightDir[3] = {0.5f, 1.0f, 0.5f};
    float len = sqrt(lightDir[0] * lightDir[0] + lightDir[1] * lightDir[1] + lightDir[2] * lightDir[2]);
    lightDir[0] /= len;
    lightDir[1] /= len;
    lightDir[2] /= len;
    
    float ndotl = std::max(0.0f, 
        hit.normal[0] * lightDir[0] + 
        hit.normal[1] * lightDir[1] + 
        hit.normal[2] * lightDir[2]);
    
    color[0] = 0.8f * ndotl + 0.2f;
    color[1] = 0.6f * ndotl + 0.2f;
    color[2] = 0.4f * ndotl + 0.2f;
}

} // namespace CarrotToy
