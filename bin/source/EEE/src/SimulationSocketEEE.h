/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineEEE.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketEEE : public NRMKHelper::NRMKSimulationSocket<EEE>
{
public:
	inline SimulationSocketEEE(EEE & robot) 
		: NRMKHelper::NRMKSimulationSocket<EEE>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkEEE.cpp.
	static const unsigned int _bodyIndexArray[EEE::NUM_BODIES];
};
