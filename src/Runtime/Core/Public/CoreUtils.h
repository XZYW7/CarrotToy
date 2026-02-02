#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <cstdint>

#pragma region FuncDefs

#define LOG(X) std::cout << X << std::endl

/** 
 * TEXT Macro - String literal helper
 * 
 * In Unreal Engine, TEXT() converts string literals to wide strings (TCHAR*) for Unicode support.
 * In CarrotToy, since TCHAR is defined as char (narrow string), TEXT() is a pass-through macro
 * that returns the string unchanged. This maintains API compatibility with UE-style code while
 * using standard C++ narrow strings.
 * 
 * Usage: TEXT("Hello World") expands to "Hello World"
 * 
 * Note: This differs from UE's TEXT macro which may use L prefix (L"string") on some platforms.
 * If migrating from UE code that expects wide strings, you may need to adapt string handling.
 * For CarrotToy's purposes, narrow strings (UTF-8) are sufficient.
 */
#if !defined(TEXT) && !defined(_WIN32)
#define TEXT(X) X
#endif

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
    #ifdef CORE_BUILD_SHARED
        #define CORE_API __declspec(dllexport)
    #elif defined(CORE_IMPORT_SHARED)
        #define CORE_API __declspec(dllimport)
    #else
        #define CORE_API
    #endif
#else
    #define CORE_API
#endif
#endif

#pragma endregion // MacrosDefs


#pragma region Globals

extern TCHAR GInternalProjectName[64];

#pragma endregion