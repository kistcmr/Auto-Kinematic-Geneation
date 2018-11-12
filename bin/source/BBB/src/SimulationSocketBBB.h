/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineBBB.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketBBB : public NRMKHelper::NRMKSimulationSocket<BBB>
{
public:
	inline SimulationSocketBBB(BBB & robot) 
		: NRMKHelper::NRMKSimulationSocket<BBB>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkBBB.cpp.
	static const unsigned int _bodyIndexArray[BBB::NUM_BODIES];
};
