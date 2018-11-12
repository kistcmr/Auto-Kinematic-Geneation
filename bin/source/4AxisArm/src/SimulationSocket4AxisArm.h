/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Define4AxisArm.h"

#include "NRMKSimulationSocket.h"

class SimulationSocket4AxisArm : public NRMKHelper::NRMKSimulationSocket<4AxisArm>
{
public:
	inline SimulationSocket4AxisArm(4AxisArm & robot) 
		: NRMKHelper::NRMKSimulationSocket<4AxisArm>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFramework4AxisArm.cpp.
	static const unsigned int _bodyIndexArray[4AxisArm::NUM_BODIES];
};
