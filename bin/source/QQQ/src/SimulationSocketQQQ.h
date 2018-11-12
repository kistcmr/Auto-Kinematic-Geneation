/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineQQQ.h"

#include "NRMKSimulationSocket.h"

class SimulationSocketQQQ : public NRMKHelper::NRMKSimulationSocket<QQQ>
{
public:
	inline SimulationSocketQQQ(QQQ & robot) 
		: NRMKHelper::NRMKSimulationSocket<QQQ>(robot, _bodyIndexArray)
	{
	}

private:
	/// TO DO: The proper body index array should be defined in NRMKFrameworkQQQ.cpp.
	static const unsigned int _bodyIndexArray[QQQ::NUM_BODIES];
};
