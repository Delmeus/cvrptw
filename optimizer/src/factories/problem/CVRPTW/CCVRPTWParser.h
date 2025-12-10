#pragma once

#include "problem/problems/CVRPTW/CCVRPTWTemplate.h"
#include <string>
#include <vector>
#include <fstream>

/**
 * @brief Enhanced parser for CVRPTW problem files
 * 
 * This parser handles the standard CVRPTW file format:
 * - NAME section
 * - VEHICLE section with NUMBER and CAPACITY
 * - CUSTOMER section with node data
 */
class CCVRPTWParser
{
public:
    /**
     * @brief Parse a CVRPTW problem file
     * @param filePath Path to the problem definition file
     * @return Parsed CCVRPTWTemplate
     */
    static CCVRPTWTemplate* ParseCVRPTWFile(const char* filePath);

    /**
     * @brief Validate parsed problem data
     * @param problemTemplate The template to validate
     * @return true if valid, false otherwise
     */
    static bool ValidateProblem(const CCVRPTWTemplate& problemTemplate);

private:
    /**
     * @brief Read vehicle section from file
     * @param fileStream Input file stream
     * @param vehicleCount Output: number of vehicles
     * @param capacity Output: vehicle capacity
     */
    static void ReadVehicleSection(std::ifstream& fileStream, int& vehicleCount, int& capacity);

    /**
     * @brief Read customer section from file
     * @param fileStream Input file stream
     * @param cities Output: vector of cities
     */
    static void ReadCustomerSection(std::ifstream& fileStream, std::vector<SCityCVRPTW>& cities);

    /**
     * @brief Skip empty lines in file stream
     * @param fileStream Input file stream
     */
    static void SkipEmptyLines(std::ifstream& fileStream);

    /**
     * @brief Extract filename without extension
     * @param filePath Full file path
     * @return Filename without extension
     */
    static std::string ExtractFileName(const std::string& filePath);
};
