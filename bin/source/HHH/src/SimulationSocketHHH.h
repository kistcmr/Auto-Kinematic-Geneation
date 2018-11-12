/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineHHH.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketHHH : public NRMKHelper::NRMKSimulationSocket<HHH>
{
public:
	inline SimulationSocketHHH(HHH & robot) 
		: NRMKHelper::NRMKSimulationSocket<HHH>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkHHH.cpp.
	static const unsigned int _bodyIndexArray[HHH::NUM_BODIES];
};
