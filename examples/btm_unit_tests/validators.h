#ifndef __VALIDATORS_H__
#define __VALIDATORS_H__

#include <string>
#include "vector.h"

bool almostEqual(double a, double b, double epsilon = 1e-6);
bool almostEqual(const btm::dvec3& a, const btm::dvec3& b, double epsilon = 1e-6);
void validate_results(const btm::dvec3& result, const btm::dvec3& expected, const std::string& testName);
void validate_results(double result, double expected, const std::string& testName);
void validate_results(bool result, bool expected, const std::string& testName);
void validate_results(int result, int expected, const std::string& testName);


#endif // __VALIDATORS_H__
