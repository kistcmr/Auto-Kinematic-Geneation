/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineArm3.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketArm3 : public NRMKHelper::NRMKSimulationSocket<Arm3>
{
public:
	inline SimulationSocketArm3(Arm3 & robot) 
		: NRMKHelper::NRMKSimulationSocket<Arm3>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkArm3.cpp.
	static const unsigned int _bodyIndexArray[Arm3::NUM_BODIES];
};
