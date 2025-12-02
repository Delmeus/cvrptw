#pragma once

#include <vector>
#include <cstddef>

class CCVRPTWTemplate;

class CCVRPTWSolution
{
public:
    CCVRPTWSolution(CCVRPTWTemplate& problemTemplate);

    [[nodiscard]] float GetTotalDistance() const;
    [[nodiscard]] float GetTotalDuration() const;

    void BuildSolution(const std::vector<int>& initialAssignment);
    [[nodiscard]] const std::vector<int>& GetSolution() const { return m_Solution; }

private:

    void PrepareData(const std::vector<int>& initialAssignment);
    [[nodiscard]] bool CanSatisfyDemand(size_t carIdx, size_t cityIdx) const;
    void MoveCarToNextCity(size_t carIdx, size_t nextCityIdx);
    void HandleTimeOnCity(size_t carIdx, size_t nextCityIdx);
    void MoveCarToDepoLoad(size_t carIdx, size_t depotIdx);
    void MoveCarToDepoLoadAndThenToCity(size_t carIdx, size_t depotIdx, size_t nextCityIdx);

    CCVRPTWTemplate& m_ECVRPTWTemplate;

    int m_CurrentLoad;
    size_t m_CurrentPosition;
    float m_Distance;
//    std::vector<float> m_CurrentTankCapacity;
    float m_CurrentTime;

    std::vector<int> m_Solution;
    size_t m_CurrentSolutionIdx;
};

