#include "CECVRPTWShawClientInsertion.h"
#include "problem/problems/CVRPTW/CCVRPTW.h"
#include "utils/random/CRandom.h"

#define TIMEWINDOWWEIGHT 0.6
#define DISTANCEWEIGHT 0.4

CECVRPTWShawClientInsertion::CECVRPTWShawClientInsertion(CCVRPTW& problemDefinition)
    : m_ProblemDefinition(problemDefinition)
{}

void CECVRPTWShawClientInsertion::Mutate(SProblemEncoding& problemEncoding, AIndividual& child)
{
	auto& genotype = child.m_Genotype.m_IntGenotype;
    auto genotypeCopy = genotype;
	std::sort(genotypeCopy.begin(), genotypeCopy.end());
	auto& problemTemplate = m_ProblemDefinition.GetECVRPTWTemplate();
	auto& cities = problemTemplate.GetCities();
	auto& distanceMatrix = problemTemplate.GetDistInfoMtx();
	auto& allCustomers = problemTemplate.GetCustomers();

    std::vector<int> missingCustomers;
    std::set_difference(allCustomers.begin(), allCustomers.end(),
                        genotypeCopy.begin(), genotypeCopy.end(),
                        std::inserter(missingCustomers, missingCustomers.begin()));
    CRandom::Shuffle(0, missingCustomers.size(), missingCustomers);

	for (int i = 0; i < missingCustomers.size(); i++)
    {
		auto& customerToCompare = cities[missingCustomers[i]];
		float minDistance = std::numeric_limits<float>::max();
		size_t customerWithMinDistanceIdx = -1;
		for (int j = 0; j < genotype.size(); j++)
        {
			if (genotype[j] != VEHICLE_DELIMITER)
            {
				float distanceTimeWindow = TIMEWINDOWWEIGHT * abs(cities[genotype[j]].m_ReadyTime - customerToCompare.m_ReadyTime);
				float distanceDistance = DISTANCEWEIGHT * distanceMatrix[missingCustomers[i]][genotype[j]].m_Distance;
				if (distanceTimeWindow + distanceDistance < minDistance)
                {
					customerWithMinDistanceIdx = j;
					minDistance = distanceTimeWindow + distanceDistance;
				}
			}
		}
		genotype.insert(genotype.begin() + customerWithMinDistanceIdx, missingCustomers[i]);
	}
}