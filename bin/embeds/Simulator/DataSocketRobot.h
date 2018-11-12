/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "DefineNRMK_ROBOT_NAME.h"

#include "NRMKDataSocket.h"

/// TO DO: Define the proper number of (float) data (transmitted via data socket) in the template arguments.
enum
{
	NUM_DATASCOPE_SIGNALS 
#if defined(KIN_CONTROL)
	= NRMK_ROBOT_NAME::JOINT_DOF,
#elif !defined(_CONTROLLER)
	= NRMK_ROBOT_NAME::JOINT_DOF*2,
#else
	= NRMK_ROBOT_NAME::JOINT_DOF*3,
#endif
};

typedef NRMKHelper::NRMKDataSocket<NUM_DATASCOPE_SIGNALS> _DATA_SOCKET;

class DataSocketNRMK_ROBOT_NAME : public _DATA_SOCKET
{
public:
	enum
	{
		NUM_DATA = _DATA_SOCKET::NUM_DATA,
	};

public:
	inline DataSocketNRMK_ROBOT_NAME(NRMK_ROBOT_NAME const & robot) 
		: _DATA_SOCKET(_dataChannelArray, _sizeDataChannelArray)
		, _robot(robot)
	{
	}

private:
	/// TO DO: Implement the data array.
	// The following is the default behavior sending the joint angles followed by the joint velocities.
	// Note that only float data is allowed for valid plot in Data Scope.
	virtual int _updatePacketData(float * const data)
	{
		for (int k = 0; k < NRMK_ROBOT_NAME::JOINT_DOF; k++)
			data[k] = static_cast<float>(_robot.q()[k]*RADIAN);

#if !defined(KIN_CONTROL)			
		for (int k = 0; k < NRMK_ROBOT_NAME::JOINT_DOF; k++)
			data[NRMK_ROBOT_NAME::JOINT_DOF + k] = static_cast<float>(_robot.qdot()[k]);

#if defined(_CONTROLLER)
		for (int k = 0; k < NRMK_ROBOT_NAME::JOINT_DOF; k++)
			data[NRMK_ROBOT_NAME::JOINT_DOF*2 + k] = static_cast<float>(_robot.tau()[k]);
#endif	// _CONTROLLER

#endif	// KIN_CONTROL

		// Always return the number of bytes of the updated data packet
#if defined(KIN_CONTROL)
		return NRMK_ROBOT_NAME::JOINT_DOF;
#elif !defined(_CONTROLLER)
		return NRMK_ROBOT_NAME::JOINT_DOF*2;
#else
		return NRMK_ROBOT_NAME::JOINT_DOF*3;
#endif
	}

private:
	/// TO DO: Define the proper data channel format array in NRMKFrameworkNRMK_ROBOT_NAME.cpp.
	static const unsigned int _dataChannelArray[];

	/// TO DO: Define the size of actual _dataChannelArray in NRMKFrameworkNRMK_ROBOT_NAME.cpp.
	static const unsigned int _sizeDataChannelArray;

	NRMK_ROBOT_NAME const & _robot;
};

