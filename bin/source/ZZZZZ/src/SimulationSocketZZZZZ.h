/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineZZZZZ.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketZZZZZ : public NRMKHelper::NRMKSimulationSocket<ZZZZZ>
{
public:
	inline SimulationSocketZZZZZ(ZZZZZ & robot) 
		: NRMKHelper::NRMKSimulationSocket<ZZZZZ>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkZZZZZ.cpp.
	static const unsigned int _bodyIndexArray[ZZZZZ::NUM_BODIES];
};
