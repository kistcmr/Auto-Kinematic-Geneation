/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefinePPP.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketPPP : public NRMKHelper::NRMKSimulationSocket<PPP>
{
public:
	inline SimulationSocketPPP(PPP & robot) 
		: NRMKHelper::NRMKSimulationSocket<PPP>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkPPP.cpp.
	static const unsigned int _bodyIndexArray[PPP::NUM_BODIES];
};
