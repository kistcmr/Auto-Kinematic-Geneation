/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Defineuuu.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketuuu : public NRMKHelper::NRMKSimulationSocket<uuu>
{
public:
	inline SimulationSocketuuu(uuu & robot) 
		: NRMKHelper::NRMKSimulationSocket<uuu>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkuuu.cpp.
	static const unsigned int _bodyIndexArray[uuu::NUM_BODIES];
};
