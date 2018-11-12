/* NRMKFoundation, Copyright 2013-2016 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#include "NRMKFrameworkArm3.h"

#if defined(_NRMK_FRAMEWORK_SUPPORT)

/// TO DO: Fill in the body index array from NRMK CADKit Viewer
// This is very critical for proper animation in CADKit Viewer. 
const unsigned int SimulationSocketArm3::_bodyIndexArray[] = { 0,1,2,3,4,5,6 };

#endif	// _NRMK_FRAMEWORK_SUPPORT

//#if defined(_CONTROL_CLIENT) || defined(_INTEGRATED_APP)
/// TO DO: Fill in the number of data and the data channel index array
const unsigned int DataSocketArm3::_dataChannelArray[]
// The following code generates two (or three) channels charts (all of time chart style),
// where each chart plots the signals (as many as the joint dof). 
//
// This is the example to plot the joint angles in the first channel, the joint velocities in the second channel, 
// the joint torque (in controlled simulation) in the third channel.
//
/// TO DO: change the number of signals in the template parameter of DataSocketArm3.
= { 
		NRMK_SCOKET_PACKET_DATA_CH(0), NRMK_SOCKET_PACKET_DATA_STYLE_TIME_CHART, ROBOT::JOINT_DOF, 0,1,2,3,4,5, 
#if !defined(KIN_CONTROL)
		NRMK_SCOKET_PACKET_DATA_CH(1), NRMK_SOCKET_PACKET_DATA_STYLE_TIME_CHART, ROBOT::JOINT_DOF, 6,7,8,9,10,11, 
#if defined(_CONTROLLER)
		NRMK_SCOKET_PACKET_DATA_CH(2), NRMK_SOCKET_PACKET_DATA_STYLE_TIME_CHART, ROBOT::JOINT_DOF, 12,13,14,15,16,17,
#endif // _CONTROLLER
#endif	// KIN_CONTROL
	};

// TO DO: Set the actual size of the data channel format array defined above
const unsigned int DataSocketArm3::_sizeDataChannelArray = sizeof(_dataChannelArray)/sizeof(unsigned int); 
//#endif // _CONTROL_CLIENT || _INTEGRATED_APP