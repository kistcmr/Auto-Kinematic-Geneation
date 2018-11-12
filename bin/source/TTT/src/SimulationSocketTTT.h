/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineTTT.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketTTT : public NRMKHelper::NRMKSimulationSocket<TTT>
{
public:
	inline SimulationSocketTTT(TTT & robot) 
		: NRMKHelper::NRMKSimulationSocket<TTT>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkTTT.cpp.
	static const unsigned int _bodyIndexArray[TTT::NUM_BODIES];
};
