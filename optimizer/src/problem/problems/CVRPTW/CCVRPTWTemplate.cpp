#include <iostream>
#include "CCVRPTWTemplate.h"

SCityCVRPTW::SCityCVRPTW(const int& id
	, const std::string& strId
	, const ENodeType type
	, const float& x
	, const float& y
	, const int& demand
	, const float& readyTime
	, const float& dueTime
	, const float& serviceTime
) : m_StrID(strId)
{
	m_ID = id;
	m_Type = type;
	m_PosX = x;
	m_PosY = y;
	m_Demand = demand;
	m_ReadyTime = readyTime;
	m_DueTime = dueTime;
	m_ServiceTime = serviceTime;
}

void CCVRPTWTemplate::Clear()
{
	m_Capacity = 0;
	m_AverageVelocity = 0;

    m_Cities.clear();
    m_DepotIndexes.clear();
    m_CustomerIndexes.clear();
	m_DistanceInfoMatrix.clear();
	m_MinDistanceVec.clear();
}

void CCVRPTWTemplate::SetData(std::vector<SCityCVRPTW>& cities
	, int capacity
	, float averageVelocity
	, std::vector<size_t>& depotIndexes
	, std::vector<size_t>& customerIndexes
//	, int vehicleCount
)
{
	Clear();

	m_Cities = cities;
	m_Capacity = capacity;
	m_DepotIndexes = depotIndexes;
    m_CustomerIndexes = customerIndexes;
	m_AverageVelocity = averageVelocity;
//	m_VehicleCount = vehicleCount;

	CalculateContextData();
}

float CCVRPTWTemplate::GetMinDistance() const {
	float dist = 0.f;
	size_t dim = m_DistanceInfoMatrix.size();
	for (size_t i = 0; i < dim; ++i)
	{
		float minDist = FLT_MAX;
		for (size_t j = 0; j < dim; ++j)
		{
			if (i != j)
			{
				minDist = fminf(minDist, m_DistanceInfoMatrix[i][j].m_Distance);
			}
		}
		dist += minDist;
	}
	return dist;
}

float CCVRPTWTemplate::GetMaxDistance() const {
	float dist = 0.f;
	size_t dim = m_DistanceInfoMatrix.size();
	for (size_t i = 0; i < dim; ++i)
	{
		float maxDist = FLT_MIN;
		for (size_t j = 0; j < dim; ++j)
		{
			if (i != j)
			{
				maxDist = fmaxf(maxDist, m_DistanceInfoMatrix[i][j].m_Distance);
			}
		}
		dist += maxDist;
	}
	return dist;
}

float CCVRPTWTemplate::GetMaxTimeService() const {
	float maxTime = FLT_MIN;
	size_t dim = m_Cities.size();
	for (size_t i = 0; i < dim; ++i)
	{
		if (maxTime < m_Cities[i].m_ServiceTime) {
			maxTime = m_Cities[i].m_ServiceTime;
		}
	}
	return maxTime;
}

size_t CCVRPTWTemplate::GetNearestDepotIdx(size_t cityIdx) const
{
    float minDist = FLT_MAX;
    size_t chosenIdx;
    auto& distMtx = GetDistInfoMtx();
    auto& depotIndexes = GetDepots();
    auto& cities = GetCities();

    for (const auto idx : depotIndexes)
    {
        int depotIndex;
        for (int i = 0; i < cities.size(); i++)
        {
            if (cities[i].m_ID == idx)
            {
                depotIndex = i;
                break;
            }
        }
        if (distMtx[cityIdx][depotIndex].m_Distance < minDist)
        {
            chosenIdx = depotIndex;
            minDist = distMtx[cityIdx][depotIndex].m_Distance;
        }
    }
    return chosenIdx;
}

// CHANGE TO - ANTEK
bool CCVRPTWTemplate::Validate() const
{
    bool isValid = true;
//    for (size_t i = 0; i < m_DistanceInfoMatrix.size(); ++i)
//    {
//        if (m_Cities[i].m_Type == ENodeType::ChargingStation)
//        {
//            size_t depotIdx = GetNearestDepotIdx(i);
//            // depot can be reached from any charging station
//            if (m_DistanceInfoMatrix[i][depotIdx].m_FuelConsumption > m_TankCapacity)
//            {
//                std::cout << "Depot cannot be reached from the charging station [" << i << "]!" << std::endl;
//                isValid = false;
//            }
//        }
//        size_t chargingStationIdx = GetNearestChargingStationIdx(i);
//        // any city can be reached from the charging station and back
//        if ((m_DistanceInfoMatrix[chargingStationIdx][i].m_FuelConsumption + m_DistanceInfoMatrix[i][chargingStationIdx].m_FuelConsumption) > m_TankCapacity)
//        {
//            std::cout << "City [" << i << "] cannot be safely reached from the nearest charging station!" << std::endl;
//            isValid = false;
//        }
//    }
    return isValid;
}

void CCVRPTWTemplate::CalculateContextData()
{
	size_t dim = m_Cities.size();
	m_DistanceInfoMatrix = std::vector<std::vector<SDistanceInfo>>(dim, std::vector<SDistanceInfo>(dim, SDistanceInfo{0, 0}));
	for (size_t i = 0; i < dim; ++i)
	{
		for (size_t j = 0; j < dim; ++j)
		{
			float dist = sqrtf(powf(m_Cities[i].m_PosX - m_Cities[j].m_PosX, 2) + powf(m_Cities[i].m_PosY - m_Cities[j].m_PosY, 2));
			float time = dist / m_AverageVelocity;
			m_DistanceInfoMatrix[i][j] = SDistanceInfo
			{
				dist,
				time
			};
		}
	}

	// Calculate minimum distance vector
	m_MinDistanceVec = std::vector<float>(dim, 0.f);
	for (size_t i = 0; i < dim; ++i)
	{
		float minDist = FLT_MAX;
		for (size_t j = 0; j < dim; ++j)
		{
			if (i != j)
			{
				minDist = fminf(minDist, m_DistanceInfoMatrix[i][j].m_Distance);
			}
		}
		m_MinDistanceVec[i] = minDist;
	}
}
