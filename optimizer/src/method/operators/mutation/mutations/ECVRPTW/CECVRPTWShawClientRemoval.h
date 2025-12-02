#pragma once

#include <algorithm>
#include "method/operators/mutation/AMutation.h"

class CCVRPTW;

class CECVRPTWShawClientRemoval : public AMutation
{
public:
	explicit CECVRPTWShawClientRemoval(CCVRPTW& problemDefinition);
	void Mutate(SProblemEncoding& problemEncoding, AIndividual& child) override;

private:
    CCVRPTW& m_ProblemDefinition; // TODO - should be const
	std::vector<int> m_CustomerIndexes;
};