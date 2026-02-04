#include "BasicTests.h"
#include "CoreUtils.h"
#include <cstring>
#include <cmath>
#include <algorithm>

BasicTests::BasicTests()
{
    LOG("BasicTests: Constructor");
}

BasicTests::~BasicTests()
{
    Shutdown();
}

bool BasicTests::Initialize()
{
    LOG("BasicTests: Initializing basic test environment");
    
    PassedTests = 0;
    FailedTests = 0;
    TestResults.clear();
    
    bInitialized = true;
    LOG("BasicTests: Initialization complete");
    return true;
}

void BasicTests::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }
    
    LOG("BasicTests: Shutting down basic test environment");
    
    // Clean up any allocated resources
    FreeTestBuffer();
    
    bInitialized = false;
    LOG("BasicTests: Shutdown complete");
}

void BasicTests::RunTests()
{
    if (!bInitialized)
    {
        LOG("BasicTests: Error - Cannot run tests, not initialized");
        return;
    }
    
    LOG("=== Running Basic Concrete Tests ===");
    TestResults.clear();
    PassedTests = 0;
    FailedTests = 0;
    
    TestMemoryAllocation();
    TestStringOperations();
    TestMathOperations();
    TestPlatformDetection();
    TestBufferOperations();
    
    LOG("=== Basic Tests Complete ===");
    LOG("BasicTests: Total tests: " + std::to_string(PassedTests + FailedTests) +
        ", Passed: " + std::to_string(PassedTests) +
        ", Failed: " + std::to_string(FailedTests));
}

void BasicTests::TestMemoryAllocation()
{
    LOG("BasicTests: Test - Memory Allocation");
    
    bool passed = true;
    std::string details;
    
    try
    {
        // Test 1: Allocate small buffer
        const size_t smallSize = 1024;
        if (!AllocateTestBuffer(smallSize))
        {
            passed = false;
            details = "Failed to allocate " + std::to_string(smallSize) + " bytes";
        }
        else
        {
            // Test 2: Write pattern to buffer
            for (size_t i = 0; i < TestBufferSize; ++i)
            {
                TestBuffer[i] = static_cast<uint8_t>(i % 256);
            }
            
            // Test 3: Verify pattern
            for (size_t i = 0; i < TestBufferSize; ++i)
            {
                if (TestBuffer[i] != static_cast<uint8_t>(i % 256))
                {
                    passed = false;
                    details = "Buffer verification failed at offset " + std::to_string(i);
                    break;
                }
            }
            
            if (passed)
            {
                details = "Successfully allocated and verified " + std::to_string(smallSize) + " bytes";
            }
            
            FreeTestBuffer();
        }
        
        // Test 4: Allocate larger buffer
        if (passed)
        {
            const size_t largeSize = 1024 * 1024; // 1MB
            if (!AllocateTestBuffer(largeSize))
            {
                passed = false;
                details = "Failed to allocate " + std::to_string(largeSize) + " bytes";
            }
            else
            {
                details = "Successfully allocated buffers: 1KB and 1MB";
                FreeTestBuffer();
            }
        }
    }
    catch (const std::exception& e)
    {
        passed = false;
        details = std::string("Exception: ") + e.what();
    }
    
    LogTestResult("Memory Allocation", passed, details);
}

void BasicTests::TestStringOperations()
{
    LOG("BasicTests: Test - String Operations");
    
    bool passed = true;
    std::string details;
    
    try
    {
        // Test 1: String concatenation
        std::string str1 = "Hello";
        std::string str2 = "World";
        std::string result = str1 + " " + str2;
        
        if (result != "Hello World")
        {
            passed = false;
            details = "String concatenation failed";
        }
        
        // Test 2: String length
        if (passed && result.length() != 11)
        {
            passed = false;
            details = "String length check failed";
        }
        
        // Test 3: String comparison
        if (passed)
        {
            std::string test1 = "CarrotToy";
            std::string test2 = "CarrotToy";
            std::string test3 = "Different";
            
            if (test1 != test2 || test1 == test3)
            {
                passed = false;
                details = "String comparison failed";
            }
        }
        
        // Test 4: String substring
        if (passed)
        {
            std::string original = "RenderBackendSandbox";
            std::string sub = original.substr(0, 6);
            
            if (sub != "Render")
            {
                passed = false;
                details = "String substring failed";
            }
        }
        
        if (passed)
        {
            details = "All string operations validated successfully";
        }
    }
    catch (const std::exception& e)
    {
        passed = false;
        details = std::string("Exception: ") + e.what();
    }
    
    LogTestResult("String Operations", passed, details);
}

void BasicTests::TestMathOperations()
{
    LOG("BasicTests: Test - Math Operations");
    
    bool passed = true;
    std::string details;
    
    try
    {
        // Test 1: Basic arithmetic
        int a = 10, b = 5;
        if (a + b != 15 || a - b != 5 || a * b != 50 || a / b != 2)
        {
            passed = false;
            details = "Basic arithmetic failed";
        }
        
        // Test 2: Floating point operations
        if (passed)
        {
            float f1 = 3.14159f;
            float f2 = 2.71828f;
            float sum = f1 + f2;
            
            if (std::abs(sum - 5.85987f) > 0.0001f)
            {
                passed = false;
                details = "Floating point arithmetic failed";
            }
        }
        
        // Test 3: Math functions
        if (passed)
        {
            double sqrtResult = std::sqrt(16.0);
            double powResult = std::pow(2.0, 3.0);
            
            if (std::abs(sqrtResult - 4.0) > 0.0001 || std::abs(powResult - 8.0) > 0.0001)
            {
                passed = false;
                details = "Math function results incorrect";
            }
        }
        
        // Test 4: Min/Max operations
        if (passed)
        {
            int values[] = {5, 2, 8, 1, 9, 3};
            int minVal = *std::min_element(values, values + 6);
            int maxVal = *std::max_element(values, values + 6);
            
            if (minVal != 1 || maxVal != 9)
            {
                passed = false;
                details = "Min/Max operations failed";
            }
        }
        
        if (passed)
        {
            details = "All math operations validated successfully";
        }
    }
    catch (const std::exception& e)
    {
        passed = false;
        details = std::string("Exception: ") + e.what();
    }
    
    LogTestResult("Math Operations", passed, details);
}

void BasicTests::TestPlatformDetection()
{
    LOG("BasicTests: Test - Platform Detection");
    
    bool passed = true;
    std::string details;
    
    try
    {
        // Detect current platform
        std::string platform = "Unknown";
        
#if defined(_WIN32) || defined(_WIN64)
        platform = "Windows";
#elif defined(__linux__)
        platform = "Linux";
#elif defined(__APPLE__) || defined(__MACH__)
        platform = "macOS";
#endif
        
        if (platform == "Unknown")
        {
            passed = false;
            details = "Failed to detect platform";
        }
        else
        {
            details = "Detected platform: " + platform;
            LOG("BasicTests: Running on " + platform);
            
            // Test pointer size (32-bit vs 64-bit)
            size_t pointerSize = sizeof(void*);
            if (pointerSize == 8)
            {
                details += " (64-bit)";
            }
            else if (pointerSize == 4)
            {
                details += " (32-bit)";
            }
            
            // Test endianness
            uint32_t testValue = 0x01020304;
            uint8_t* bytes = reinterpret_cast<uint8_t*>(&testValue);
            bool isLittleEndian = (bytes[0] == 0x04);
            
            details += isLittleEndian ? ", Little Endian" : ", Big Endian";
        }
    }
    catch (const std::exception& e)
    {
        passed = false;
        details = std::string("Exception: ") + e.what();
    }
    
    LogTestResult("Platform Detection", passed, details);
}

void BasicTests::TestBufferOperations()
{
    LOG("BasicTests: Test - Buffer Operations");
    
    bool passed = true;
    std::string details;
    
    try
    {
        const size_t bufferSize = 512;
        if (!AllocateTestBuffer(bufferSize))
        {
            passed = false;
            details = "Failed to allocate test buffer";
        }
        else
        {
            // Test 1: memset
            std::memset(TestBuffer, 0xAA, TestBufferSize);
            if (!ValidateBuffer(0xAA))
            {
                passed = false;
                details = "Buffer fill validation failed (0xAA pattern)";
            }
            
            // Test 2: Different pattern
            if (passed)
            {
                std::memset(TestBuffer, 0x55, TestBufferSize);
                if (!ValidateBuffer(0x55))
                {
                    passed = false;
                    details = "Buffer fill validation failed (0x55 pattern)";
                }
            }
            
            // Test 3: Zero fill
            if (passed)
            {
                std::memset(TestBuffer, 0x00, TestBufferSize);
                if (!ValidateBuffer(0x00))
                {
                    passed = false;
                    details = "Buffer zero-fill validation failed";
                }
            }
            
            // Test 4: memcpy
            if (passed)
            {
                uint8_t* secondBuffer = new uint8_t[bufferSize];
                for (size_t i = 0; i < bufferSize; ++i)
                {
                    TestBuffer[i] = static_cast<uint8_t>(i % 256);
                }
                
                std::memcpy(secondBuffer, TestBuffer, bufferSize);
                
                bool copyValid = true;
                for (size_t i = 0; i < bufferSize; ++i)
                {
                    if (secondBuffer[i] != TestBuffer[i])
                    {
                        copyValid = false;
                        break;
                    }
                }
                
                delete[] secondBuffer;
                
                if (!copyValid)
                {
                    passed = false;
                    details = "Buffer copy validation failed";
                }
            }
            
            if (passed)
            {
                details = "All buffer operations validated successfully";
            }
            
            FreeTestBuffer();
        }
    }
    catch (const std::exception& e)
    {
        passed = false;
        details = std::string("Exception: ") + e.what();
        FreeTestBuffer();
    }
    
    LogTestResult("Buffer Operations", passed, details);
}

void BasicTests::LogTestResult(const std::string& testName, bool passed, const std::string& details)
{
    std::string result = testName + ": " + (passed ? "PASS" : "FAIL");
    if (!details.empty())
    {
        result += " - " + details;
    }
    
    TestResults.push_back(result);
    
    if (passed)
    {
        PassedTests++;
    }
    else
    {
        FailedTests++;
    }
    
    LOG("BasicTests: " + result);
}

bool BasicTests::AllocateTestBuffer(size_t size)
{
    FreeTestBuffer();
    
    try
    {
        TestBuffer = new uint8_t[size];
        TestBufferSize = size;
        return true;
    }
    catch (const std::bad_alloc&)
    {
        TestBuffer = nullptr;
        TestBufferSize = 0;
        return false;
    }
}

void BasicTests::FreeTestBuffer()
{
    if (TestBuffer != nullptr)
    {
        delete[] TestBuffer;
        TestBuffer = nullptr;
        TestBufferSize = 0;
    }
}

bool BasicTests::ValidateBuffer(uint8_t expectedValue)
{
    if (TestBuffer == nullptr || TestBufferSize == 0)
    {
        return false;
    }
    
    for (size_t i = 0; i < TestBufferSize; ++i)
    {
        if (TestBuffer[i] != expectedValue)
        {
            return false;
        }
    }
    
    return true;
}
