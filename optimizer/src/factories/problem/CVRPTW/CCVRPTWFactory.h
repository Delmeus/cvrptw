#pragma once

#include "problem/problems/CVRPTW/CCVRPTW.h"
#include "./CCVRPTWParser.h"
#include <fstream>
#include <vector>

class CCVRPTWFactory {
public:
    static CCVRPTW* CreateCVRPTW(const char* problemDefinitionPath);
    static void DeleteObjects();
private:
    static const std::string s_Delimiter;
    static const std::string s_CapacityKey;
    static const std::string s_CitiesSectionKey;
    static const std::string s_TankCapacityKey;
    static const std::string s_FuelConsumptionKey;
    static const std::string s_InverseRefuelingRateKey;
    static const std::string s_VelocityKey;
    static const std::string s_VehicleCountKey;

    static CCVRPTWTemplate* cvrpTemplate;
    static CCVRPTWTemplate* ReadCVRPTWTemplate(const char* problemDefinitionPath);
    static void ReadCities(std::ifstream& fileStream, int& dimension, std::vector<SCityCVRPTW>& cities);
};