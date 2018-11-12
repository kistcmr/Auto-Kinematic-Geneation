/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Definegggg.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketgggg : public NRMKHelper::NRMKSimulationSocket<gggg>
{
public:
	inline SimulationSocketgggg(gggg & robot) 
		: NRMKHelper::NRMKSimulationSocket<gggg>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkgggg.cpp.
	static const unsigned int _bodyIndexArray[gggg::NUM_BODIES];
};
