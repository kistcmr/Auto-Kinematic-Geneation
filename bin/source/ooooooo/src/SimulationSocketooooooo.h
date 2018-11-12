/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Defineooooooo.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketooooooo : public NRMKHelper::NRMKSimulationSocket<ooooooo>
{
public:
	inline SimulationSocketooooooo(ooooooo & robot) 
		: NRMKHelper::NRMKSimulationSocket<ooooooo>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkooooooo.cpp.
	static const unsigned int _bodyIndexArray[ooooooo::NUM_BODIES];
};
