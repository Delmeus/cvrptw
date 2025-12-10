#pragma once

#include "problem/problems/CVRPTW/CCVRPTWTemplate.h"
#include <string>
#include <vector>

/**
 * @brief Validator for CVRPTW problem instances
 * 
 * Provides comprehensive validation and analysis of parsed CVRPTW problems
 */
class CCVRPTWValidator
{
public:
    /**
     * @brief Validation result structure
     */
    struct ValidationResult
    {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::string summary;
    };

    /**
     * @brief Validate a CVRPTW problem template
     * @param problemTemplate The template to validate
     * @return ValidationResult with detailed information
     */
    static ValidationResult Validate(const CCVRPTWTemplate& problemTemplate);

    /**
     * @brief Print validation results to console
     * @param result The validation result to print
     */
    static void PrintValidationResult(const ValidationResult& result);

    /**
     * @brief Generate a detailed report of the problem instance
     * @param problemTemplate The template to analyze
     * @return Detailed report string
     */
    static std::string GenerateReport(const CCVRPTWTemplate& problemTemplate);
};
