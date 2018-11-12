/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineNRMK_ROBOT_NAME.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketNRMK_ROBOT_NAME : public NRMKHelper::NRMKSimulationSocket<NRMK_ROBOT_NAME>
{
public:
	inline SimulationSocketNRMK_ROBOT_NAME(NRMK_ROBOT_NAME & robot) 
		: NRMKHelper::NRMKSimulationSocket<NRMK_ROBOT_NAME>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkNRMK_ROBOT_NAME.cpp.
	static const unsigned int _bodyIndexArray[NRMK_ROBOT_NAME::NUM_BODIES];
};
