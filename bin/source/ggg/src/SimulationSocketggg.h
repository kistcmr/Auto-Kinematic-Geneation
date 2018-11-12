/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Defineggg.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketggg : public NRMKHelper::NRMKSimulationSocket<ggg>
{
public:
	inline SimulationSocketggg(ggg & robot) 
		: NRMKHelper::NRMKSimulationSocket<ggg>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkggg.cpp.
	static const unsigned int _bodyIndexArray[ggg::NUM_BODIES];
};
