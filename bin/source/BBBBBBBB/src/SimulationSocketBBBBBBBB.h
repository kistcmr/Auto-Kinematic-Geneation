/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineBBBBBBBB.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketBBBBBBBB : public NRMKHelper::NRMKSimulationSocket<BBBBBBBB>
{
public:
	inline SimulationSocketBBBBBBBB(BBBBBBBB & robot) 
		: NRMKHelper::NRMKSimulationSocket<BBBBBBBB>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkBBBBBBBB.cpp.
	static const unsigned int _bodyIndexArray[BBBBBBBB::NUM_BODIES];
};
