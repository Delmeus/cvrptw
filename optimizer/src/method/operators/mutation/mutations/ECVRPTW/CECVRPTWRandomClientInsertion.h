#pragma once

#include <algorithm>
#include <chrono>
#include "method/operators/mutation/AMutation.h"

class CCVRPTW;

class CECVRPTWRandomClientInsertion : public AMutation
{
public:
	explicit CECVRPTWRandomClientInsertion(CCVRPTW& problemDefinition);
	void Mutate(SProblemEncoding& problemEncoding, AIndividual& child) override;

private:
    CCVRPTW& m_ProblemDefinition; // TODO - should be const
};