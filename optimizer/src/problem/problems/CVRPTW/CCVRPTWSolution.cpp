#include "CCVRPTWSolution.h"
#include "problem/problems/CVRPTW/CCVRPTWTemplate.h"
#include <numeric>
#include <stdexcept>

CCVRPTWSolution::CCVRPTWSolution(CCVRPTWTemplate& problemTemplate)
    : m_ECVRPTWTemplate(problemTemplate)
{
}

float CCVRPTWSolution::GetTotalDistance() const
{
    return m_Distance;
}

float CCVRPTWSolution::GetTotalDuration() const
{
    return m_CurrentTime;
}

void CCVRPTWSolution::BuildSolution(const std::vector<int>& initialAssignment)
{
    PrepareData(initialAssignment);

    auto& distMtx = m_ECVRPTWTemplate.GetDistInfoMtx();
    auto& cities = m_ECVRPTWTemplate.GetCities();

    int currentCar = 0;
    for (m_CurrentSolutionIdx = 0; m_CurrentSolutionIdx < m_Solution.size(); ++m_CurrentSolutionIdx)
    {
        size_t nextCityIdx = m_Solution[(m_CurrentSolutionIdx + 1) % m_Solution.size()];
        if (nextCityIdx == VEHICLE_DELIMITER)
        {
            currentCar++;
            continue;
        }

        if (CanSatisfyDemand(currentCar, nextCityIdx))
        {
            MoveCarToNextCity(currentCar, nextCityIdx);
        }
        else
        {
            size_t depotIdx = m_ECVRPTWTemplate.GetNearestDepotIdx(m_CurrentPosition);
            MoveCarToDepoLoad(currentCar, depotIdx);

        }
    }
}

bool CCVRPTWSolution::CanSatisfyDemand(size_t carIdx, size_t cityIdx) const
{
    return m_CurrentLoad >= m_ECVRPTWTemplate.GetCities()[cityIdx].m_Demand;
}


void CCVRPTWSolution::PrepareData(const std::vector<int>& initialAssignment)
{
//    size_t vehicleCount = m_ECVRPTWTemplate.GetVehicleCount();
    m_CurrentLoad = m_ECVRPTWTemplate.GetCapacity();
    m_CurrentPosition = DEPOT_CITY_ID;
    m_Distance = 0.f;
    m_CurrentTime = 0.f;

    m_Solution = initialAssignment;
    m_Solution.emplace(m_Solution.begin(), DEPOT_CITY_ID);
    m_Solution.emplace(m_Solution.end(), DEPOT_CITY_ID);
    for (size_t i = 0; i < m_Solution.size(); ++i)
    {
        if (m_Solution[i] == VEHICLE_DELIMITER)
        {
            m_Solution.emplace(m_Solution.begin() + i, DEPOT_CITY_ID);
            m_Solution.emplace(m_Solution.begin() + i + 2, DEPOT_CITY_ID);
            i += 2;
        }
    }
}

void CCVRPTWSolution::MoveCarToDepoLoad(size_t carIdx, size_t depotIdx)
{
    auto& distMtx = m_ECVRPTWTemplate.GetDistInfoMtx();
    size_t& currentCityIdx = m_CurrentPosition;

    //To depot
    m_Distance += distMtx[currentCityIdx][depotIdx].m_Distance;
    m_CurrentTime += distMtx[currentCityIdx][depotIdx].m_TravelTime;

    //Depot car loading
    m_CurrentLoad= m_ECVRPTWTemplate.GetCapacity();

    //Add depot visit to solution
    m_Solution.emplace(m_Solution.begin() + (int)m_CurrentSolutionIdx + 1, depotIdx);
    currentCityIdx = depotIdx;
}

void CCVRPTWSolution::MoveCarToDepoLoadAndThenToCity(size_t carIdx, size_t depotIdx, size_t nextCityIdx)
{
    auto& distMtx = m_ECVRPTWTemplate.GetDistInfoMtx();
    auto& cities = m_ECVRPTWTemplate.GetCities();

    size_t& currentCityIdx = m_CurrentPosition;

    //To depot
    m_Distance += distMtx[currentCityIdx][depotIdx].m_Distance;
    m_CurrentTime+= distMtx[currentCityIdx][depotIdx].m_TravelTime;

    //Depot car loading
    m_CurrentLoad = m_ECVRPTWTemplate.GetCapacity();

    //To next city
    m_Distance += distMtx[depotIdx][nextCityIdx].m_Distance;
    m_CurrentTime += distMtx[depotIdx][nextCityIdx].m_TravelTime;

    m_CurrentLoad -= cities[nextCityIdx].m_Demand;

    //Add depot visit to solution
    m_Solution.emplace(m_Solution.begin() + (int)m_CurrentSolutionIdx + 1, depotIdx);

    HandleTimeOnCity(carIdx, nextCityIdx);
    m_CurrentSolutionIdx++;
    currentCityIdx = nextCityIdx;
}

void CCVRPTWSolution::MoveCarToNextCity(size_t carIdx, size_t nextCityIdx)
{
    auto& distMtx = m_ECVRPTWTemplate.GetDistInfoMtx();
    auto& cities = m_ECVRPTWTemplate.GetCities();

    size_t& currentCityIdx = m_CurrentPosition;
    m_Distance += distMtx[currentCityIdx][nextCityIdx].m_Distance;
    m_CurrentTime += distMtx[currentCityIdx][nextCityIdx].m_TravelTime;
    m_CurrentLoad -= cities[nextCityIdx].m_Demand;

    HandleTimeOnCity(carIdx, nextCityIdx);

    m_CurrentTime += cities[nextCityIdx].m_ServiceTime;
    currentCityIdx = nextCityIdx;
}

void CCVRPTWSolution::HandleTimeOnCity(size_t carIdx, size_t nextCityIdx)
{
    auto& cities = m_ECVRPTWTemplate.GetCities();
    auto dayLength = m_ECVRPTWTemplate.GetMaxDueTime();

    if (std::fmod(m_CurrentTime, dayLength) < cities[nextCityIdx].m_ReadyTime)
    {
        m_CurrentTime += cities[nextCityIdx].m_ReadyTime - std::fmod(m_CurrentTime, dayLength);
    }
    else if (std::fmod(m_CurrentTime, dayLength) > cities[nextCityIdx].m_DueTime)
    {
        float timeToEndOfDay = dayLength - std::fmod(m_CurrentTime, dayLength);
        m_CurrentTime += timeToEndOfDay;
        m_CurrentTime += cities[nextCityIdx].m_ReadyTime;
    }
}
