#pragma once

#include <algorithm>
#include "method/operators/mutation/AMutation.h"

class CCVRPTW;

class CECVRPTWRandomClientRemoval : public AMutation
{
public:
	explicit CECVRPTWRandomClientRemoval(CCVRPTW& problemDefinition);
	void Mutate(SProblemEncoding& problemEncoding, AIndividual& child) override;

private:
    CCVRPTW& m_ProblemDefinition; // TODO - should be const
};