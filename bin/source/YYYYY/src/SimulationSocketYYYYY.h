/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineYYYYY.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketYYYYY : public NRMKHelper::NRMKSimulationSocket<YYYYY>
{
public:
	inline SimulationSocketYYYYY(YYYYY & robot) 
		: NRMKHelper::NRMKSimulationSocket<YYYYY>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkYYYYY.cpp.
	static const unsigned int _bodyIndexArray[YYYYY::NUM_BODIES];
};
