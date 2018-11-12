/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Defineqqqqq.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketqqqqq : public NRMKHelper::NRMKSimulationSocket<qqqqq>
{
public:
	inline SimulationSocketqqqqq(qqqqq & robot) 
		: NRMKHelper::NRMKSimulationSocket<qqqqq>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkqqqqq.cpp.
	static const unsigned int _bodyIndexArray[qqqqq::NUM_BODIES];
};
