/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Definefffffff.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketfffffff : public NRMKHelper::NRMKSimulationSocket<fffffff>
{
public:
	inline SimulationSocketfffffff(fffffff & robot) 
		: NRMKHelper::NRMKSimulationSocket<fffffff>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkfffffff.cpp.
	static const unsigned int _bodyIndexArray[fffffff::NUM_BODIES];
};
