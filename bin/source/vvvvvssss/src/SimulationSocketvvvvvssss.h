/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Definevvvvvssss.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketvvvvvssss : public NRMKHelper::NRMKSimulationSocket<vvvvvssss>
{
public:
	inline SimulationSocketvvvvvssss(vvvvvssss & robot) 
		: NRMKHelper::NRMKSimulationSocket<vvvvvssss>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkvvvvvssss.cpp.
	static const unsigned int _bodyIndexArray[vvvvvssss::NUM_BODIES];
};
