/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineKKKK.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketKKKK : public NRMKHelper::NRMKSimulationSocket<KKKK>
{
public:
	inline SimulationSocketKKKK(KKKK & robot) 
		: NRMKHelper::NRMKSimulationSocket<KKKK>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkKKKK.cpp.
	static const unsigned int _bodyIndexArray[KKKK::NUM_BODIES];
};
