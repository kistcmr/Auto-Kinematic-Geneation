/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Definebb.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketbb : public NRMKHelper::NRMKSimulationSocket<bb>
{
public:
	inline SimulationSocketbb(bb & robot) 
		: NRMKHelper::NRMKSimulationSocket<bb>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkbb.cpp.
	static const unsigned int _bodyIndexArray[bb::NUM_BODIES];
};
