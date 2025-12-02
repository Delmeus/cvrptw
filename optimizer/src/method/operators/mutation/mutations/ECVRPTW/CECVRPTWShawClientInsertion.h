#pragma once

#include <algorithm>
#include <chrono>
#include "method/operators/mutation/AMutation.h"

class CCVRPTW;

class CECVRPTWShawClientInsertion : public AMutation
{
public:
	explicit CECVRPTWShawClientInsertion(CCVRPTW& problemDefinition);
	void Mutate(SProblemEncoding& problemEncoding, AIndividual& child) override;

private:
    CCVRPTW& m_ProblemDefinition; // TODO - should be const
};