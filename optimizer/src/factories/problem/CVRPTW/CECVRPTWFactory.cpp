#include "CCVRPTWFactory.h"
#include "utils/fileReader/CReadUtils.h"
#include <regex>
#include <iostream>


const std::string CCVRPTWFactory::s_Delimiter = ": ";
const std::string CCVRPTWFactory::s_CapacityKey = "C";
//const std::string CCVRPTWFactory::s_TankCapacityKey = "Q";
//const std::string CCVRPTWFactory::s_FuelConsumptionKey = "r";
//const std::string CCVRPTWFactory::s_InverseRefuelingRateKey = "g";
const std::string CCVRPTWFactory::s_VelocityKey = "v";
//const std::string CCVRPTWFactory::s_VehicleCountKey = "n";
const std::string CCVRPTWFactory::s_CitiesSectionKey = "StringID";

CCVRPTWTemplate* CCVRPTWFactory::cvrpTemplate = nullptr;

CCVRPTW* CCVRPTWFactory::CreateCVRPTW(const char* problemDefinitionPath) {
    cvrpTemplate = ReadCVRPTWTemplate(problemDefinitionPath);
    return new CCVRPTW(*cvrpTemplate);
}

void CCVRPTWFactory::DeleteObjects() {
    delete cvrpTemplate;
}

//TODO validate the loader and fix - DUDEK
CCVRPTWTemplate* CCVRPTWFactory::ReadCVRPTWTemplate(const char* problemDefinitionPath) {
    auto* result = new CCVRPTWTemplate();

    std::ifstream readFileStream(problemDefinitionPath);

    int dimension = 0;
    std::vector<SCityCVRPTW> cities;

    float capacity = 0;

    std::string line;
    while (std::getline(readFileStream, line))
    {
        if (line.find("NUMBER") != std::string::npos)
            break;
    }

    std::string vehicleDataLine;
    std::getline(readFileStream, vehicleDataLine);

    while (vehicleDataLine.find_first_not_of(" \t\r\n") == std::string::npos)
        std::getline(readFileStream, vehicleDataLine);

    auto tokens = CReadUtils::SplitLine(vehicleDataLine);
    if (tokens.size() < 2)
        throw std::runtime_error("Invalid VEHICLE section format");

    float vehicleCount = std::stof(tokens[0]);
    capacity = std::stof(tokens[1]);

    ReadCities(readFileStream, dimension, cities);

    readFileStream.close();

    std::vector<size_t> depotIndexes;
    std::vector<size_t> customerIndexes;

    for (int i = 0; i < cities.size(); i++)
    {
        switch (cities[i].m_Type)
        {
            case ENodeType::Depot:
                depotIndexes.emplace_back(i);
                break;
            case ENodeType::Customer:
                customerIndexes.emplace_back(i);
                break;
        }
    }

    std::string pathString(problemDefinitionPath);
    size_t fileNameStartPos = pathString.rfind("/") + 1;
    size_t fileNameEndPos = pathString.rfind(".");
    result->SetFileName(pathString.substr(fileNameStartPos, fileNameEndPos - fileNameStartPos));
    result->SetData(cities,
                    static_cast<int>(capacity),
                    90.0,
                    depotIndexes,
                    customerIndexes
    );

    if (!result->Validate())
    {
        throw std::runtime_error("Instance is invalid: " + std::string(problemDefinitionPath));
    }

    return result;
}

void CCVRPTWFactory::ReadCities(std::ifstream& fileStream, int& dimension, std::vector<SCityCVRPTW>& cities) {
    std::string line;

//    if (CReadUtils::GotoLineByKey(fileStream, s_CitiesSectionKey, line))
//    {
        dimension = 0;
        while (std::getline(fileStream, line) && !line.empty())
        {
            auto const re = std::regex{ R"(\s+)" };
            auto const vec = std::vector<std::string>(std::sregex_token_iterator{ line.begin(), line.end(), re, -1 }, std::sregex_token_iterator{});
            cities.emplace_back(dimension++,
                                vec[0],
                                (ENodeType)vec[1][0],
                                std::stof(vec[2]),
                                std::stof(vec[3]),
                                (int)std::stof(vec[4]),
                                std::stof(vec[5]),
                                std::stof(vec[6]),
                                std::stof(vec[7])
            );
        }
//    }
//    else
//    {
//        throw std::runtime_error("Error reading cities for CVRPTW");
//    }
}