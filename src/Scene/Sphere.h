#pragma once
#include "Object.h"
#include <vector>
#include <cmath>

namespace CarrotToy {

inline float degreesToRadians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

class Sphere : public Object
{
public:
    Sphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18)
        : radius(radius), sectorCount(sectorCount), stackCount(stackCount) {
        // buildVertices();
    }

    ~Sphere() {}

    // const std::vector<float>& getVertices() const { return vertices; }
    // const std::vector<unsigned int>& getIndices() const { return indices; }
public:
    float radius;
    int sectorCount;
    int stackCount;
};

}