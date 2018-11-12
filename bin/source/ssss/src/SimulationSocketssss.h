/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Definessss.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketssss : public NRMKHelper::NRMKSimulationSocket<ssss>
{
public:
	inline SimulationSocketssss(ssss & robot) 
		: NRMKHelper::NRMKSimulationSocket<ssss>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkssss.cpp.
	static const unsigned int _bodyIndexArray[ssss::NUM_BODIES];
};
