#include "CCVRPTWValidator.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

CCVRPTWValidator::ValidationResult CCVRPTWValidator::Validate(const CCVRPTWTemplate& problemTemplate)
{
    ValidationResult result;
    result.isValid = true;

    // Check basic structure
    if (problemTemplate.GetCitiesSize() < 2)
    {
        result.errors.push_back("Problem must have at least 1 depot and 1 customer");
        result.isValid = false;
    }

    // Check depot is at index 0
    if (problemTemplate.GetCitiesSize() > 0 && problemTemplate.GetCities()[0].m_Type != ENodeType::Depot)
    {
        result.errors.push_back("First city (index 0) must be the depot");
        result.isValid = false;
    }

    // Check all other cities are customers
    for (size_t i = 1; i < problemTemplate.GetCitiesSize(); i++)
    {
        if (problemTemplate.GetCities()[i].m_Type != ENodeType::Customer)
        {
            result.errors.push_back("City at index " + std::to_string(i) + " is not a customer");
            result.isValid = false;
        }
    }

    // Check capacity
    if (problemTemplate.GetCapacity() <= 0)
    {
        result.errors.push_back("Vehicle capacity must be positive");
        result.isValid = false;
    }

    // Check depot demand
    if (problemTemplate.GetCities()[0].m_Demand != 0)
    {
        result.errors.push_back("Depot demand must be 0");
        result.isValid = false;
    }

    // Check customer demands
    int totalDemand = 0;
    for (size_t i = 1; i < problemTemplate.GetCitiesSize(); i++)
    {
        int demand = problemTemplate.GetCities()[i].m_Demand;
        if (demand < 0)
        {
            result.errors.push_back("Customer " + std::to_string(i) + " has negative demand");
            result.isValid = false;
        }
        if (demand > problemTemplate.GetCapacity())
        {
            result.warnings.push_back("Customer " + std::to_string(i) + " demand (" + std::to_string(demand) + 
                                    ") exceeds vehicle capacity (" + std::to_string(problemTemplate.GetCapacity()) + ")");
        }
        totalDemand += demand;
    }

    // Check time windows
    for (size_t i = 0; i < problemTemplate.GetCitiesSize(); i++)
    {
        float readyTime = problemTemplate.GetCities()[i].m_ReadyTime;
        float dueTime = problemTemplate.GetCities()[i].m_DueTime;
        float serviceTime = problemTemplate.GetCities()[i].m_ServiceTime;

        if (readyTime > dueTime)
        {
            result.errors.push_back("City " + std::to_string(i) + ": ready time (" + std::to_string((int)readyTime) + 
                                  ") > due time (" + std::to_string((int)dueTime) + ")");
            result.isValid = false;
        }

        if (serviceTime < 0)
        {
            result.errors.push_back("City " + std::to_string(i) + " has negative service time");
            result.isValid = false;
        }

        if (dueTime < 0)
        {
            result.errors.push_back("City " + std::to_string(i) + " has negative due time");
            result.isValid = false;
        }
    }

    // Generate summary
    std::ostringstream summary;
    summary << "Validation Summary:\n";
    summary << "  - Cities: " << problemTemplate.GetCitiesSize() << " (1 depot + " << (problemTemplate.GetCitiesSize() - 1) << " customers)\n";
    summary << "  - Vehicle Capacity: " << problemTemplate.GetCapacity() << "\n";
    summary << "  - Total Demand: " << totalDemand << "\n";
    summary << "  - Min Distance: " << std::fixed << std::setprecision(2) << problemTemplate.GetMinDistance() << "\n";
    summary << "  - Max Distance: " << std::fixed << std::setprecision(2) << problemTemplate.GetMaxDistance() << "\n";
    summary << "  - Max Time Service: " << std::fixed << std::setprecision(2) << problemTemplate.GetMaxTimeService() << "\n";
    summary << "  - Max Due Time: " << std::fixed << std::setprecision(2) << problemTemplate.GetMaxDueTime() << "\n";
    summary << "  - Status: " << (result.isValid ? "VALID" : "INVALID") << "\n";
    summary << "  - Errors: " << result.errors.size() << "\n";
    summary << "  - Warnings: " << result.warnings.size() << "\n";

    result.summary = summary.str();
    return result;
}

void CCVRPTWValidator::PrintValidationResult(const ValidationResult& result)
{
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "CVRPTW VALIDATION REPORT\n";
    std::cout << std::string(60, '=') << "\n\n";

    std::cout << result.summary << "\n";

    if (!result.errors.empty())
    {
        std::cout << "\nERRORS (" << result.errors.size() << "):\n";
        for (const auto& error : result.errors)
        {
            std::cout << "  [ERROR] " << error << "\n";
        }
    }

    if (!result.warnings.empty())
    {
        std::cout << "\nWARNINGS (" << result.warnings.size() << "):\n";
        for (const auto& warning : result.warnings)
        {
            std::cout << "  [WARNING] " << warning << "\n";
        }
    }

    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

std::string CCVRPTWValidator::GenerateReport(const CCVRPTWTemplate& problemTemplate)
{
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "DETAILED CVRPTW PROBLEM REPORT\n";
    report << std::string(70, '=') << "\n\n";

    // Problem metadata
    report << "Problem: " << problemTemplate.GetFileName() << "\n";
    report << "Cities: " << problemTemplate.GetCitiesSize() << "\n";
    report << "Vehicle Capacity: " << problemTemplate.GetCapacity() << "\n";
    report << "Average Velocity: " << problemTemplate.GetAverageVelociy() << "\n\n";

    // Depot information
    report << "DEPOT INFORMATION:\n";
    report << std::string(70, '-') << "\n";
    const auto& depot = problemTemplate.GetCities()[0];
    report << "  ID: " << depot.m_ID << "\n";
    report << "  Position: (" << std::fixed << std::setprecision(2) << depot.m_PosX << ", " << depot.m_PosY << ")\n";
    report << "  Time Window: [" << (int)depot.m_ReadyTime << ", " << (int)depot.m_DueTime << "]\n";
    report << "  Service Time: " << (int)depot.m_ServiceTime << "\n\n";

    // Customer statistics
    report << "CUSTOMER STATISTICS:\n";
    report << std::string(70, '-') << "\n";
    report << std::setw(6) << "ID" << std::setw(12) << "X" << std::setw(12) << "Y" 
           << std::setw(10) << "Demand" << std::setw(12) << "Ready" << std::setw(12) << "Due" 
           << std::setw(10) << "Service\n";
    report << std::string(70, '-') << "\n";

    int totalDemand = 0;
    float minX = depot.m_PosX, maxX = depot.m_PosX;
    float minY = depot.m_PosY, maxY = depot.m_PosY;

    for (size_t i = 1; i < problemTemplate.GetCitiesSize(); i++)
    {
        const auto& city = problemTemplate.GetCities()[i];
        report << std::setw(6) << city.m_ID 
               << std::setw(12) << std::fixed << std::setprecision(2) << city.m_PosX
               << std::setw(12) << std::fixed << std::setprecision(2) << city.m_PosY
               << std::setw(10) << city.m_Demand
               << std::setw(12) << (int)city.m_ReadyTime
               << std::setw(12) << (int)city.m_DueTime
               << std::setw(10) << (int)city.m_ServiceTime << "\n";

        totalDemand += city.m_Demand;
        minX = std::min(minX, city.m_PosX);
        maxX = std::max(maxX, city.m_PosX);
        minY = std::min(minY, city.m_PosY);
        maxY = std::max(maxY, city.m_PosY);
    }

    report << std::string(70, '-') << "\n";
    report << "Total Demand: " << totalDemand << "\n";
    report << "Vehicles Needed (min): " << ((totalDemand + problemTemplate.GetCapacity() - 1) / problemTemplate.GetCapacity()) << "\n";
    report << "Area: X[" << std::fixed << std::setprecision(2) << minX << ", " << maxX << "], "
           << "Y[" << minY << ", " << maxY << "]\n\n";

    // Distance matrix statistics
    report << "DISTANCE MATRIX STATISTICS:\n";
    report << std::string(70, '-') << "\n";
    report << "Min Distance: " << std::fixed << std::setprecision(2) << problemTemplate.GetMinDistance() << "\n";
    report << "Max Distance: " << std::fixed << std::setprecision(2) << problemTemplate.GetMaxDistance() << "\n";
    report << "Max Time Service: " << std::fixed << std::setprecision(2) << problemTemplate.GetMaxTimeService() << "\n";
    report << "Max Due Time: " << std::fixed << std::setprecision(2) << problemTemplate.GetMaxDueTime() << "\n";

    report << "\n" << std::string(70, '=') << "\n\n";

    return report.str();
}
