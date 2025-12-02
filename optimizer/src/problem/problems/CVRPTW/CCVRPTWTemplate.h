#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <cstdint>
#include "ENodeType.h"

constexpr int VEHICLE_DELIMITER = INT32_MAX;
constexpr int DEPOT_CITY_ID = 0;

struct SCityCVRPTW
{
    SCityCVRPTW(const int& id
        , const std::string& strId
        , const ENodeType type
        , const float& x
        , const float& y
        , const int& demand
        , const float& readyTime
        , const float& dueTime
        , const float& serviceTime
    );

    int m_ID;
    std::string m_StrID;
    ENodeType m_Type;
    float m_PosX, m_PosY;
    int m_Demand;
    float m_ReadyTime, m_DueTime, m_ServiceTime;
};

struct SDistanceInfo
{
    float m_Distance;
    float m_TravelTime;
};

class CCVRPTWTemplate
{
public:
    void Clear();
    const std::string& GetFileName() const { return m_FileName; }
    void SetFileName(const std::string& fileName) { m_FileName = fileName; }
    void SetData(std::vector<SCityCVRPTW>& cities
        , int capacity
        , float averageVelocity
        , std::vector<size_t>& depotIndexes
        , std::vector<size_t>& customerIndexes
//        , int vehicleCount
    );

    const std::vector<SCityCVRPTW>& GetCities() const { return m_Cities; }
    const std::vector<std::vector<SDistanceInfo>>& GetDistInfoMtx() const { return m_DistanceInfoMatrix; }
    const std::vector<float>& GetMinDistVec() const { return m_MinDistanceVec; }
    const std::vector<size_t>& GetDepots() const { return m_DepotIndexes; }
    const std::vector<size_t>& GetCustomers() const { return m_CustomerIndexes; }

    int GetCapacity() const { return m_Capacity; }
    float GetAverageVelociy() const { return m_AverageVelocity; }

    size_t GetCitiesSize() const { return m_Cities.size(); }

    float GetMinDistance() const;
    float GetMaxDistance() const;
    float GetMaxTimeService() const;
    float GetMaxDueTime() const { return m_Cities[0].m_DueTime; }

    size_t GetNearestDepotIdx(size_t cityIdx) const;

    bool Validate() const;

private:
    void CalculateContextData();

    std::string m_FileName;

    // File data
    std::vector<SCityCVRPTW> m_Cities;
    std::vector<size_t> m_DepotIndexes;
    std::vector<size_t> m_CustomerIndexes;
    int m_Capacity;
    float m_AverageVelocity;
//    int m_VehicleCount;

    // Context data
    std::vector<std::vector<SDistanceInfo>> m_DistanceInfoMatrix;
    std::vector<float> m_MinDistanceVec;
};
