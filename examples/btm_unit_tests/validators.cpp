#include <iostream>
#include "validators.h"
#include "vector.h"

using namespace btm;

// almostEqual function to compare floating-point numbers with a tolerance
bool almostEqual(double a, double b, double epsilon/*= 1e-6*/) {
    return std::fabs(a - b) < epsilon;
}

bool almostEqual(const dvec3& a, const dvec3& b, double epsilon/*= 1e-6*/) {
    return almostEqual(a.x(), b.x(), epsilon) &&
        almostEqual(a.y(), b.y(), epsilon) &&
        almostEqual(a.z(), b.z(), epsilon);
}

// Function to validate results and print test outcomes
void validate_results(const dvec3& result, const dvec3& expected, const std::string& testName) {
    if (!almostEqual(result, expected)) {
        std::cout << "Test failed: " << testName << std::endl;
        std::cout << "Expected: (" << expected.x() << ", " << expected.y() << ", " << expected.z() << ")" << std::endl;
        std::cout << "Got: (" << result.x() << ", " << result.y() << ", " << result.z() << ")" << std::endl;
    }
    else {
        std::cout << "Test passed: " << testName << std::endl;
    }
}

void validate_results(double result, double expected, const std::string& testName) {
    if (!almostEqual(result, expected)) {
        std::cout << "Test failed: " << testName << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Got: " << result << std::endl;
    }
    else {
        std::cout << "Test passed: " << testName << std::endl;
    }
}

void validate_results(bool result, bool expected, const std::string& testName) {
    if (result != expected) {
        std::cout << "Test failed: " << testName << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Got: " << result << std::endl;
    }
    else {
        std::cout << "Test passed: " << testName << std::endl;
    }
}

void validate_results(int result, int expected, const std::string& testName) {
    if (result != expected) {
        std::cout << "Test failed: " << testName << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Got: " << result << std::endl;
    }
    else {
        std::cout << "Test passed: " << testName << std::endl;
    }
}
