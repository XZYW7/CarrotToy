#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <cstdint>

#pragma region FuncDefs

#define LOG(X) std::cout << X << std::endl
#define TEXT(X) X

#pragma endregion // FuncDefs

#pragma region TypeDefs

// Type definition for data types

using uint = unsigned int;
using uint32 = uint32_t;
using uint64 = uint64_t;
using int32 = int32_t;
using int64 = int64_t;

using FString = std::string;
using FText = std::string;
using TCHAR = char;
using FName = std::string;
template<typename T> using FUniquePtr = std::unique_ptr<T>;
template<typename T> using FSharedPtr = std::shared_ptr<T>;
template<typename T> using FWeakPtr = std::weak_ptr<T>;
template<typename T> using FVector = std::vector<T>;
template<typename K, typename V> using FMap = std::unordered_map<K, V>;

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

    // Iterator support for range-based for loops
    typename FVector<T>::iterator begin() { return data.begin(); }
    typename FVector<T>::iterator end() { return data.end(); }
    typename FVector<T>::const_iterator begin() const { return data.begin(); }
    typename FVector<T>::const_iterator end() const { return data.end(); }

private:
    FVector<T> data;
};

#pragma endregion // TypeDefs


#pragma region MacrosDefs
#ifndef CORE_API
#if defined(_WIN32) || defined(_WIN64)
#define CORE_API __declspec(dllexport)
#else
#define CORE_API
#endif
#endif

#pragma endregion // MacrosDefs


#pragma region Globals

extern CORE_API TCHAR GInternalProjectName[64];

#pragma endregion