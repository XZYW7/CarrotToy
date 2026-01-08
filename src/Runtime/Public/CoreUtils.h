#pragma once
#include <iostream>
#include <vector>

#define LOG(X) std::cout << X << std::endl

// Type definitions for TArray
template<typename T>
class TArray {
public:
    TArray() = default;

    void Add(const T& item) {
        data.push_back(item);
    }

    size_t Num() const {
        return data.size();
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

private:
    std::vector<T> data;
};


// Type definition for data types

using uint = unsigned int;
using uint32 = uint32_t;
using uint64 = uint64_t;