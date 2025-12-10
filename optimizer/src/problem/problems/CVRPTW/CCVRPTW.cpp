#include "CCVRPTW.h"
#include "../../../utils/logger/CExperimentLogger.h"
#include "CCVRPTWSolution.h"
#include <iostream>
#include <sstream>

CCVRPTW::CCVRPTW(CCVRPTWTemplate& ecvrptwBase)
    : m_ECVRPTWTemplate(ecvrptwBase)
{
    CreateProblemEncoding();

    m_MaxObjectiveValues = {
        m_ECVRPTWTemplate.GetMaxDistance(),
        m_ECVRPTWTemplate.GetMaxDueTime() * (float)m_ECVRPTWTemplate.GetCustomers().size()
    };

    m_MinObjectiveValues = {
            0, //min distance
            0, //min due time
    };
}

std::vector<int> CCVRPTW::GetRealPath(AIndividual& individual)
{
    CCVRPTWSolution solution(m_ECVRPTWTemplate);
    solution.BuildSolution(individual.m_Genotype.m_IntGenotype);
    return solution.GetSolution();
}

void CCVRPTW::Evaluate(AIndividual& individual)
{
    CCVRPTWSolution solution(m_ECVRPTWTemplate);
    solution.BuildSolution(individual.m_Genotype.m_IntGenotype);

    individual.m_Evaluation[0] = solution.GetTotalDistance();
    individual.m_Evaluation[1] = solution.GetTotalDuration();

    // Normalize
    for (int i = 0; i < 2; i++)
    {
        individual.m_NormalizedEvaluation[i] = (individual.m_Evaluation[i] - m_MinObjectiveValues[i]) / (m_MaxObjectiveValues[i] - m_MinObjectiveValues[i]);
    }
}

void CCVRPTW::CreateProblemEncoding()
{
    auto& customers = m_ECVRPTWTemplate.GetCustomers();

    SEncodingSection citiesSection = SEncodingSection
    {
        std::vector<SEncodingDescriptor>(customers.size() - 1,
            SEncodingDescriptor{
                    (float)customers[0], (float)customers[customers.size()-1]
            }
        ),
        EEncodingType::PERMUTATION
    };

    m_ProblemEncoding = SProblemEncoding{3, {citiesSection} };
}

void CCVRPTW::LogSolution(AIndividual& individual)
{
    auto realPath = GetRealPath(individual);
    std::string solution;
    for (int i = 0; i < realPath.size(); i++) {
        solution += std::to_string(realPath[i]);
        if (i != realPath.size() - 1) {
            solution += ";";
        }
    }
    CExperimentLogger::AddLine(solution.c_str());

    std::ostringstream costTimeData;
    costTimeData << "Distance:" << individual.m_Evaluation[0] << ";Duration:" << individual.m_Evaluation[1];
    CExperimentLogger::AddLine(costTimeData.str().c_str());
}

void CCVRPTW::LogAdditionalData()
{
    std::ostringstream pointsData;
    auto& cityData = m_ECVRPTWTemplate.GetCities();
    for (auto& city : cityData) {
        
        pointsData << city.m_PosX << ';' << city.m_PosY << ';' << (char)city.m_Type << std::endl;
    }
    CExperimentLogger::LogResult(pointsData.str().c_str(), "points.csv");
}

