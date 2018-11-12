/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineFourAxisArm.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketFourAxisArm : public NRMKHelper::NRMKSimulationSocket<FourAxisArm>
{
public:
	inline SimulationSocketFourAxisArm(FourAxisArm & robot) 
		: NRMKHelper::NRMKSimulationSocket<FourAxisArm>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkFourAxisArm.cpp.
	static const unsigned int _bodyIndexArray[FourAxisArm::NUM_BODIES];
};
