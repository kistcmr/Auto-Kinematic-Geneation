/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Definekk00.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketkk00 : public NRMKHelper::NRMKSimulationSocket<kk00>
{
public:
	inline SimulationSocketkk00(kk00 & robot) 
		: NRMKHelper::NRMKSimulationSocket<kk00>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkkk00.cpp.
	static const unsigned int _bodyIndexArray[kk00::NUM_BODIES];
};
