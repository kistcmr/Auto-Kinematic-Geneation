/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Definevvv.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketvvv : public NRMKHelper::NRMKSimulationSocket<vvv>
{
public:
	inline SimulationSocketvvv(vvv & robot) 
		: NRMKHelper::NRMKSimulationSocket<vvv>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkvvv.cpp.
	static const unsigned int _bodyIndexArray[vvv::NUM_BODIES];
};
