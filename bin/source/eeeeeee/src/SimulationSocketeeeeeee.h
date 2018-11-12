/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Defineeeeeeee.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketeeeeeee : public NRMKHelper::NRMKSimulationSocket<eeeeeee>
{
public:
	inline SimulationSocketeeeeeee(eeeeeee & robot) 
		: NRMKHelper::NRMKSimulationSocket<eeeeeee>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkeeeeeee.cpp.
	static const unsigned int _bodyIndexArray[eeeeeee::NUM_BODIES];
};
