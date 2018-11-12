/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#include "NRMKFrameworkBBB.h"

#if defined(_NRMK_FRAMEWORK_SUPPORT)

/// TO DO: Fill in the body index array from NRMK CADKit Viewer
// This is very critical for proper animation in CADKit Viewer. 
const unsigned int SimulationSocketBBB::_bodyIndexArray[] = { NRMK_BODY_INDEX };

/// TO DO: Fill in the number of data and the data channel index array
const unsigned int DataSocketBBB::_dataChannelArray[]

	= { 
		NRMK_SCOKET_PACKET_DATA_CH(0), NRMK_SOCKET_PACKET_DATA_STYLE_TIME_CHART, ROBOT::JOINT_DOF, NRMK_DATA_INDEX0 
		NRMK_SCOKET_PACKET_DATA_CH(1), NRMK_SOCKET_PACKET_DATA_STYLE_TIME_CHART, ROBOT::JOINT_DOF, NRMK_DATA_INDEX1 
		NRMK_SCOKET_PACKET_DATA_CH(2), NRMK_SOCKET_PACKET_DATA_STYLE_TIME_CHART, ROBOT::JOINT_DOF, NRMK_DATA_INDEX2
	};

const unsigned int DataSocketBBB::_sizeDataChannelArray = sizeof(_dataChannelArray)/sizeof(unsigned int); 

#endif