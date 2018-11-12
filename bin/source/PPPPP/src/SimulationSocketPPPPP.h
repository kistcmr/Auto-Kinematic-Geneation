/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefinePPPPP.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketPPPPP : public NRMKHelper::NRMKSimulationSocket<PPPPP>
{
public:
	inline SimulationSocketPPPPP(PPPPP & robot) 
		: NRMKHelper::NRMKSimulationSocket<PPPPP>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkPPPPP.cpp.
	static const unsigned int _bodyIndexArray[PPPPP::NUM_BODIES];
};
