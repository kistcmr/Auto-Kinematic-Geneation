/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#if defined(__RTX__	)

#if defined(_CONTROL_CLIENT)
#define _CONTROL_CLIENT_RTX
#endif	// _CONTROL_CLIENT

#undef _USE_COMMAND_SOCKET	// do not use a command socket. 
#undef _USE_CONTROL_SOCKET	// do not use a control socket. 

#else	// !(__RTX__)

#if !defined(_INTEGRATED_APP)
#define _USE_COMMAND_SOCKET	// use a command socket
#define _USE_CONTROL_SOCKET	// use a control socket
#endif

#endif	// __RTX__

#if defined(_INTEGRATED_APP) || defined(_APP_MANAGER)
#define _NRMK_FRAMEWORK_SUPPORT // for NRMKFramework support
#endif

// For RTX control clients, no framework support.
// Otherwise,
#if !defined(_CONTROL_CLIENT_RTX )

// Socket (These headers should come first..)
#include "NRMKSimulationSocket.h"
#include "NRMKDataSocket.h"

#endif	// _CONTROL_CLIENT_RTX

// keyboard event handler
#include "vkcode.h"		

// Unit update period
/// TO DO: Set the unit simulation period. By default it is set to 2 (msec) = 0.002 (sec).
#define UNIT_DELT	(1*MILLI_SECOND)
#define TIME_TO_TICK(time)	static_cast<long>((time)/UNIT_DELT)

// System definition
#include "DefineConstant.h"

#include "Arm3ControlLib/DefineArm3.h"

typedef Arm3			ROBOT;

#if defined(_CONTROLLER)

#if !defined(_APP_MANAGER)
// Control Algorithm
#include "ControlArm3/ControlArm3.h"
typedef ControlArm3		CONTROLLER;
#endif

#endif	// _CONTROLLER

#if defined(_INTEGRATED_APP)

// Data Socket
#include "DataSocketArm3.h"
typedef DataSocketArm3 DATA_SOCKET;

// Simulation Socket
#include "SimulationSocketArm3.h"
typedef SimulationSocketArm3 SIMULATION_SOCKET;

#endif	// _INTEGRATED_APP
	
// Control clients may need data socket but not simulation socket.
#if defined(_CONTROL_CLIENT)

// Data Socket
#include "DataSocketArm3.h"
typedef DataSocketArm3 DATA_SOCKET;

#endif	// !(_CONTROL_CLIENT)
	
#if defined(_APP_MANAGER)
// Simulation Socket
#include "SimulationSocketArm3.h"
typedef SimulationSocketArm3 SIMULATION_SOCKET;

// Data Socket
#include "DataSocketArm3.h"
typedef DataSocketArm3 DATA_SOCKET;

#endif	// _APP_MANAGER

// TO DO: Set the update period for data socket. By default, it is set to 10 (msec).
//#define DELT_DATA		(10*MILLI_SECOND)	// = 0.01 (sec)

// The following is temporary as of v1.5.
#if defined(_SYS_INTERFACE_VER_SHMEM)
// In case of shared memory system interface, do not create graphics socket.
#undef _GOBJECTS_ENABLED	

#endif	// _SYS_INTERFACE_VER_SHMEM

// For custom graphics object animation
#if defined(_GOBJECTS_ENABLED)

#if defined(_CONTROL_CLIENT)

// Control graphics 
#include "GraphicsControlArm3.h"
typedef GraphicsControlArm3 GRAPHICS_CONTROL;

#else	// !(_CONTROL_CLIENT)

// Graphics Socket
#include "GraphicsSocketArm3.h"
typedef GraphicsSocketArm3 GRAPHICS_SOCKET;

#endif // _CONTROL_CLIENT

#endif  // _GOBJECTS_ENABLED

// Controller and system should interface each other via system interface object.
#if defined(_CONTROLLER)

// Added since v1.6.5
#if defined(_IMPEDANCE_CONTROL)
#define _FORCE_SENSOR_FEEDBACK
#endif

#include "SystemInterface.h"

#if defined(_INTEGRATED_APP) || defined(_CONTROL_CLIENT)
#define _USE_PENDANT_SOCKET
#if defined(_USE_PENDANT_SOCKET)
#include "GUIPendantSocketArm3.h"
typedef NRMKHelper::GUIPendantSocketArm3 PENDANT_SOCKET;
#endif // _USE_PENDANT_SOCKET
#endif

#if defined(_INTEGRATED_APP)

// Use the direct system interface for integrated app by default.
#if defined(_SYS_INTERFACE_VER_BUFFER)
typedef NRMKHelper::SystemInterface_Buffer<ROBOT, 2*ROBOT::JOINT_DOF*sizeof(float), ROBOT::JOINT_DOF*sizeof(float)> SYSTEM_INTERFACE;
#else	// !(_SYS_INTERFACE_VER_BUFFER)
typedef NRMKHelper::SystemInterface_Direct<ROBOT> SYSTEM_INTERFACE;
#endif

#endif	// _INTEGRATED_APP

#if defined(_APP_MANAGER) || defined(_CONTROL_CLIENT)

#if defined(_USE_COMMAND_SOCKET)
#include "NRMKCommandSocket.h"
typedef NRMKHelper::NRMKKeyCommandSocket COMMAND_SOCKET;
#endif 

// Use the socket system interface for separated app by default.
#if defined(_SYS_INTERFACE_VER_SHMEM)

#if !defined(__RTX__)
#include "SystemInterface_ShMem.h"
typedef NRMKHelper::SystemInterface_ShMem<ROBOT, 2*ROBOT::JOINT_DOF*sizeof(float), ROBOT::JOINT_DOF*sizeof(float)> SYSTEM_INTERFACE;
#else	// __RTX__
#include "SystemInterface_RTX.h"
typedef NRMKHelper::SystemInterface_ShMem_RTX<ROBOT, 2*ROBOT::JOINT_DOF*sizeof(float), ROBOT::JOINT_DOF*sizeof(float)> SYSTEM_INTERFACE;
#endif	// __RTX__

#else	// _SYS_INTERFACE_VER_SHMEM

// FIXED BY THACHDO 20150918
#ifdef _USE_CONTROL_PENDANT_SOCKET
#define _USE_PENDANT_SOCKET
#include "NRMKPendantSocket.h"
typedef NRMKHelper:: NRMKControlStateSocket<ROBOT::JOINT_DOF> PENDANT_SOCKET;

#else // !(_USE_CONTROL_PENDANT_SOCKET)

#if defined(_USE_ANIMATION_SOCKET)
#include "NRMKPendantSocket.h"
typedef NRMKHelper:: NRMKAnimationSocket<ROBOT::JOINT_DOF> ANIMATION_SOCKET;
#endif // _USE_ANIMATION_SOCKET


#include "NRMKControlSocket.h"
#include "SystemInterface_Socket.h"

#if defined(_CONTROL_CLIENT)
#define NUM_BYTES_PACKET_CONTROL_GRAPHICS GRAPHICS_CONTROL::MAX_PACKET_BYTES
#else	// !(_CONTROL_CLIENT)
#define NUM_GOBJECT_FRAME 2
#define NUM_BYTES_PACKET_CONTROL_GRAPHICS ((sizeof(unsigned int)*2 + (NRMKHelper::NRMKGObject::PACKET_HEADER_BYTE + NRMKHelper::NRMKGObjectFrame::MAX_PACKET_BYTE)*NUM_GOBJECT_FRAME))
#undef NUM_GOBJECT_FRAME
#endif	// _CONTROL_CLIENT

#if defined(_GOBJECTS_ENABLED)

#if defined(_FORCE_SENSOR_FEEDBACK)
typedef NRMKHelper::NRMKControlSocket<(2*ROBOT::JOINT_DOF + 6)*sizeof(float), ROBOT::JOINT_DOF*sizeof(float), 0, ROBOT::JOINT_DOF, NUM_BYTES_PACKET_CONTROL_GRAPHICS> CONTROL_SOCKET;
#else	// !(_FORCE_SENSOR_FEEDBACK)
typedef NRMKHelper::NRMKControlSocket<2*ROBOT::JOINT_DOF*sizeof(float), ROBOT::JOINT_DOF*sizeof(float), 0, ROBOT::JOINT_DOF, NUM_BYTES_PACKET_CONTROL_GRAPHICS> CONTROL_SOCKET;
#endif //_FORCE_SENSOR_FEEDBACK

#else	// !(_GOBJECTS_ENABLED)

#if defined(_FORCE_SENSOR_FEEDBACK)
typedef NRMKHelper::NRMKControlSocket<(2*ROBOT::JOINT_DOF + 6)*sizeof(float), ROBOT::JOINT_DOF*sizeof(float), 0, ROBOT::JOINT_DOF> CONTROL_SOCKET;
#else	// !(_FORCE_SENSOR_FEEDBACK)
typedef NRMKHelper::NRMKControlSocket<2*ROBOT::JOINT_DOF*sizeof(float), ROBOT::JOINT_DOF*sizeof(float), 0, ROBOT::JOINT_DOF> CONTROL_SOCKET;
#endif	// _FORCE_SENSOR_FEEDBACK

#endif // _GOBJECTS_ENABLED

#endif	// _USE_CONTROL_PENDANT_SOCKET

#endif	// _SYS_INTERFACE_VER_SHMEM


#if defined(_USE_ETHERCAT)

//FIXED BY THACHDO 20150918
#include "UniversalSystemInterfaceArm3.h"
typedef NRMKHelper::UniversalSystemInterfaceArm3<ROBOT> SYSTEM_INTERFACE;

// Animation Socket
#include "NRMKPendantSocket.h"
typedef NRMKHelper:: NRMKAnimationSocket<ROBOT::JOINT_DOF> ANIMATION_SOCKET;

#else // !(_USE_ETHERCAT)

typedef NRMKHelper::SystemInterface_Socket<ROBOT, CONTROL_SOCKET> SYSTEM_INTERFACE;

#endif // _USE_ETHERCAT

#endif	// defined(_APP_MANAGER) || defined(_CONTROL_CLIENT)

#endif // _CONTROLLER
