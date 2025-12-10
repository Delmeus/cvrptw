#include "CCVRPTWParser.h"
#include "utils/fileReader/CReadUtils.h"
#include <regex>
#include <iostream>
#include <sstream>

CCVRPTWTemplate* CCVRPTWParser::ParseCVRPTWFile(const char* filePath)
{
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open())
    {
        throw std::runtime_error("Cannot open file: " + std::string(filePath));
    }

    auto* result = new CCVRPTWTemplate();
    int vehicleCount = 0;
    int capacity = 0;
    std::vector<SCityCVRPTW> cities;

    try
    {
        // Read vehicle section
        ReadVehicleSection(fileStream, vehicleCount, capacity);

        // Read customer section
        ReadCustomerSection(fileStream, cities);

        fileStream.close();

        if (cities.empty())
        {
            throw std::runtime_error("No cities were read from the file");
        }

        // Classify nodes: city 0 is depot, others are customers
        std::vector<size_t> depotIndexes;
        std::vector<size_t> customerIndexes;

        for (size_t i = 0; i < cities.size(); i++)
        {
            if (i == 0)
            {
                depotIndexes.emplace_back(i);
            }
            else
            {
                customerIndexes.emplace_back(i);
            }
        }

        // Extract filename
        std::string fileName = ExtractFileName(filePath);
        result->SetFileName(fileName);

        // Set problem data
        result->SetData(cities, capacity, 90.0f, depotIndexes, customerIndexes);

        // Validate
        if (!ValidateProblem(*result))
        {
            throw std::runtime_error("Instance validation failed: " + std::string(filePath));
        }

        return result;
    }
    catch (const std::exception& e)
    {
        fileStream.close();
        delete result;
        throw std::runtime_error("Error parsing CVRPTW file: " + std::string(e.what()));
    }
}

void CCVRPTWParser::ReadVehicleSection(std::ifstream& fileStream, int& vehicleCount, int& capacity)
{
    std::string line;

    // Find VEHICLE section
    while (std::getline(fileStream, line))
    {
        if (line.find("NUMBER") != std::string::npos)
            break;
    }

    // Skip empty lines and read the first non-empty line (vehicle data)
    while (std::getline(fileStream, line))
    {
        // Skip empty lines
        if (line.find_first_not_of(" \t\r\n") == std::string::npos)
            continue;
        // Found non-empty line, this should be vehicle data
        break;
    }

    if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos)
    {
        throw std::runtime_error("Unexpected end of file while reading VEHICLE section");
    }

    std::istringstream iss(line);
    if (!(iss >> vehicleCount >> capacity)) {
        throw std::runtime_error("Failed to parse vehicle line: " + line);
    }


    if (vehicleCount <= 0 || capacity <= 0)
    {
        throw std::runtime_error("Invalid vehicle count or capacity (must be > 0). Got: vehicleCount=" + std::to_string(vehicleCount) + ", capacity=" + std::to_string(capacity));
    }
}

void CCVRPTWParser::ReadCustomerSection(std::ifstream& fileStream, std::vector<SCityCVRPTW>& cities)
{
    std::string line;
    int dimension = 0;

    // Find CUSTOMER section
    while (std::getline(fileStream, line))
    {
        if (line.find("CUST NO.") != std::string::npos)
            break;
    }

    // Read customer data lines
    while (std::getline(fileStream, line))
    {
        // Skip empty lines and lines with only whitespace
        if (line.find_first_not_of(" \t\r\n") == std::string::npos)
            continue;

        auto tokens = CReadUtils::SplitLine(line);

        tokens.erase(
    std::remove_if(tokens.begin(), tokens.end(),
                   [](const std::string& s) {
                       return s.find_first_not_of(" \t\r\n") == std::string::npos;
                   }),
    tokens.end()
);

        // Validate we have enough fields: CUST_NO, X, Y, DEMAND, READY_TIME, DUE_DATE, SERVICE_TIME
        if (tokens.size() < 7)
        {
            // If we have fewer tokens and we've already read some cities, we've reached the end
            if (!cities.empty())
                break;
            continue;
        }

        try
        {
            int custId = std::stoi(tokens[0]);
            float x = std::stof(tokens[1]);
            float y = std::stof(tokens[2]);
            int demand = std::stoi(tokens[3]);
            float readyTime = std::stof(tokens[4]);
            float dueTime = std::stof(tokens[5]);
            float serviceTime = std::stof(tokens[6]);

            // Determine node type: city 0 is depot, others are customers
            ENodeType nodeType = (custId == 0) ? ENodeType::Depot : ENodeType::Customer;

            cities.emplace_back(dimension++,
                                tokens[0],
                                nodeType,
                                x,
                                y,
                                demand,
                                readyTime,
                                dueTime,
                                serviceTime
            );
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("Error parsing customer data at line: " + line + ". Error: " + std::string(e.what()));
        }
    }

    if (cities.empty())
    {
        throw std::runtime_error("No customer data was read from the file");
    }
}

void CCVRPTWParser::SkipEmptyLines(std::ifstream& fileStream)
{
    std::string line;

    while (std::getline(fileStream, line))
    {
        // If we find a non-empty line, we need to put it back
        if (line.find_first_not_of(" \t\r\n") != std::string::npos)
        {
            // We can't easily "unread" a line with getline, so we'll clear the stream state
            // and let the caller handle reading the next line
            // This function is now only used to skip empty lines before reading vehicle data
            break;
        }
    }
}

std::string CCVRPTWParser::ExtractFileName(const std::string& filePath)
{
    // Find last separator (handle both / and \)
    size_t lastSeparator = filePath.rfind("/");
    if (lastSeparator == std::string::npos)
        lastSeparator = filePath.rfind("\\");

    size_t startPos = (lastSeparator != std::string::npos) ? lastSeparator + 1 : 0;

    // Find last dot for extension
    size_t lastDot = filePath.rfind(".");
    size_t endPos = (lastDot != std::string::npos && lastDot > startPos) ? lastDot : filePath.length();

    return filePath.substr(startPos, endPos - startPos);
}

bool CCVRPTWParser::ValidateProblem(const CCVRPTWTemplate& problemTemplate)
{
    // Check that we have at least depot and one customer
    if (problemTemplate.GetCitiesSize() < 2)
    {
        std::cerr << "Error: Problem must have at least 1 depot and 1 customer" << std::endl;
        return false;
    }

    // Check that depot is at index 0
    if (problemTemplate.GetCities()[0].m_Type != ENodeType::Depot)
    {
        std::cerr << "Error: First city (index 0) must be the depot" << std::endl;
        return false;
    }

    // Check that all other cities are customers
    for (size_t i = 1; i < problemTemplate.GetCitiesSize(); i++)
    {
        if (problemTemplate.GetCities()[i].m_Type != ENodeType::Customer)
        {
            std::cerr << "Error: City at index " << i << " is not a customer" << std::endl;
            return false;
        }
    }

    // Check capacity is positive
    if (problemTemplate.GetCapacity() <= 0)
    {
        std::cerr << "Error: Vehicle capacity must be positive" << std::endl;
        return false;
    }

    // Check that all demands are non-negative and depot demand is 0
    if (problemTemplate.GetCities()[0].m_Demand != 0)
    {
        std::cerr << "Error: Depot demand must be 0" << std::endl;
        return false;
    }

    for (size_t i = 1; i < problemTemplate.GetCitiesSize(); i++)
    {
        if (problemTemplate.GetCities()[i].m_Demand < 0)
        {
            std::cerr << "Error: Customer demand cannot be negative" << std::endl;
            return false;
        }
        if (problemTemplate.GetCities()[i].m_Demand > problemTemplate.GetCapacity())
        {
            std::cerr << "Warning: Customer " << i << " demand exceeds vehicle capacity" << std::endl;
        }
    }

    // Check time windows
    for (size_t i = 0; i < problemTemplate.GetCitiesSize(); i++)
    {
        if (problemTemplate.GetCities()[i].m_ReadyTime > problemTemplate.GetCities()[i].m_DueTime)
        {
            std::cerr << "Error: Ready time cannot be greater than due time for city " << i << std::endl;
            return false;
        }
        if (problemTemplate.GetCities()[i].m_ServiceTime < 0)
        {
            std::cerr << "Error: Service time cannot be negative for city " << i << std::endl;
            return false;
        }
    }

    return true;
}
