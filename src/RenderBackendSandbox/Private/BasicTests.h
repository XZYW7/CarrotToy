/**
 * BasicTests - Simple Concrete Tests for Sandbox
 * 
 * Provides actual test implementations that validate basic functionality
 * without requiring complex graphics API setup.
 */

#pragma once
#include <string>
#include <vector>
#include <cstdint>

/**
 * Basic Testing Framework
 * 
 * This class provides simple, platform-independent tests that demonstrate
 * the testing framework works with actual validation logic.
 */
class BasicTests
{
public:
    BasicTests();
    ~BasicTests();

    // Initialize basic test environment
    bool Initialize();
    
    // Shutdown and cleanup
    void Shutdown();
    
    // Run all basic tests
    void RunTests();
    
    // Individual test methods (actual implementations)
    void TestMemoryAllocation();
    void TestStringOperations();
    void TestMathOperations();
    void TestPlatformDetection();
    void TestBufferOperations();
    
    // Query test status
    bool IsInitialized() const { return bInitialized; }
    const std::vector<std::string>& GetTestResults() const { return TestResults; }
    int GetPassedTests() const { return PassedTests; }
    int GetFailedTests() const { return FailedTests; }

private:
    void LogTestResult(const std::string& testName, bool passed, const std::string& details = "");
    
    // Helper methods for actual testing
    bool AllocateTestBuffer(size_t size);
    void FreeTestBuffer();
    bool ValidateBuffer(uint8_t expectedValue);
    
    bool bInitialized = false;
    std::vector<std::string> TestResults;
    int PassedTests = 0;
    int FailedTests = 0;
    
    // Test buffer for memory tests
    uint8_t* TestBuffer = nullptr;
    size_t TestBufferSize = 0;
};
