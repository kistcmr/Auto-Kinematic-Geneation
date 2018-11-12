/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#include "NRMKFrameworkHHH.h"

#if defined(_NRMK_FRAMEWORK_SUPPORT)

/// TO DO: Fill in the body index array from NRMK CADKit Viewer
// This is very critical for proper animation in CADKit Viewer. 
const unsigned int SimulationSocketHHH::_bodyIndexArray[] = { 0,1,2,3,4,5,6 };

/// TO DO: Fill in the number of data and the data channel index array
const unsigned int DataSocketHHH::_dataChannelArray[]

	= { 
		NRMK_SCOKET_PACKET_DATA_CH(0), NRMK_SOCKET_PACKET_DATA_STYLE_TIME_CHART, ROBOT::JOINT_DOF, 0,1,2,3,4,5, 
		NRMK_SCOKET_PACKET_DATA_CH(1), NRMK_SOCKET_PACKET_DATA_STYLE_TIME_CHART, ROBOT::JOINT_DOF, 6,7,8,9,10,11, 
		NRMK_SCOKET_PACKET_DATA_CH(2), NRMK_SOCKET_PACKET_DATA_STYLE_TIME_CHART, ROBOT::JOINT_DOF, 12,13,14,15,16,17,
	};

const unsigned int DataSocketHHH::_sizeDataChannelArray = sizeof(_dataChannelArray)/sizeof(unsigned int); 

#endif