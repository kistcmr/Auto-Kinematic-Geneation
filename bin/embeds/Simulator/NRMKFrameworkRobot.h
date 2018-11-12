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

#else	// __RTX__

#if !defined(_INTEGRATED_APP)
#define _USE_COMMAND_SOCKET	// use a command socket
#define _USE_CONTROL_SOCKET	// use a control socket
#define _USE_TP_SOCKET // use a teach pendant(tp) socket by guhee
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
#define UNIT_DELT	(10*MILLI_SECOND)
#define TIME_TO_TICK(time)	static_cast<long>((time)/UNIT_DELT)

// System definition
#include "DefineConstant.h"
#include "DefineNRMK_ROBOT_NAME.h"
typedef NRMK_ROBOT_NAME			ROBOT;

#if defined(_CONTROLLER)

#if !defined(_APP_MANAGER)
// Control Algorithm
#include "ControlNRMK_ROBOT_NAME/ControlNRMK_ROBOT_NAME.h"
typedef ControlNRMK_ROBOT_NAME		CONTROLLER;
#endif

#endif	// _CONTROLLER

// Control clients do not need simulation and data socket.
#if !defined(_CONTROL_CLIENT)

// Simulation Socket
#include "SimulationSocketNRMK_ROBOT_NAME.h"
typedef SimulationSocketNRMK_ROBOT_NAME SIMULATION_SOCKET;

// Data Socket
#include "DataSocketNRMK_ROBOT_NAME.h"
typedef DataSocketNRMK_ROBOT_NAME DATA_SOCKET;

#endif	// _CONTROL_CLIENT

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
#include "GraphicsControlNRMK_ROBOT_NAME.h"
typedef GraphicsControlNRMK_ROBOT_NAME GRAPHICS_CONTROL;
#else	// _CONTROL_CLIENT
// Graphics Socket
#include "GraphicsSocketNRMK_ROBOT_NAME.h"
typedef GraphicsSocketNRMK_ROBOT_NAME GRAPHICS_SOCKET;
#endif // _CONTROL_CLIENT

#endif  // _GOBJECTS_ENABLED

// Controller and system should interface each other via system interface object.
#if defined(_CONTROLLER)

// Added since v1.6.5
#if defined(_IMPEDANCE_CONTROL)
#define _FORCE_SENSOR_FEEDBACK
#endif

#include "SystemInterface.h"

#if defined(_INTEGRATED_APP)
// Use the direct system interface for integrated app by default.
#if defined(_SYS_INTERFACE_VER_BUFFER)
typedef NRMKHelper::SystemInterface_Buffer<ROBOT, 2*ROBOT::JOINT_DOF*sizeof(float), ROBOT::JOINT_DOF*sizeof(float)> SYSTEM_INTERFACE;
#else
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

#if defined(_USE_ETHERCAT)
//FIXME@20140930
#include "ecrt.h"

/// TO DO: Define your EtherCAT SystemInterface object
// #if 1	
// #define NUM_YASKAWA_DRIVES (ROBOT::JOINT_DOF)
// 
// #include "SystemInterface_EtherCAT_Yaskawa_Driver.h"
// typedef NRMKHelper::SystemInterface_EtherCAT_Yaskawa_Driver<NUM_YASKAWA_DRIVES> SYSTEM_INTERFACE_ECAT;
// #else	// Dummy Ethercat system
// #include "SystemInterface_EtherCAT_Null.h"
// typedef NRMKHelper::SystemInterface_EtherCAT_Null<ROBOT> SYSTEM_INTERFACE_ECAT;
// #endif 

#endif // _USE_ETHERCAT

// FIXME@20141001
#if defined(_USE_CONTROL_PENDANT_SOCKET)
#define _USE_PENDANT_SOCKET
#endif

#ifdef _USE_CONTROL_PENDANT_SOCKET
#include "NRMKPendantSocket.h"
typedef NRMKHelper:: NRMKControlStateSocket<ROBOT::JOINT_DOF> PENDANT_SOCKET;

#else //_USE_CONTROL_PENDANT_SOCKET

#if defined(_USE_PENDANT_SCOKET)
#include "NRMKPendantSocket.h"
typedef NRMKHelper:: NRMKAnimationSocket<ROBOT::JOINT_DOF> PENDANT_SOCKET;
#endif // _USE_PENDANT_SOCKET

#include "NRMKControlSocket.h"
#include "SystemInterface_Socket.h"

#if defined(_CONTROL_CLIENT)
#define NUM_BYTES_PACKET_CONTROL_GRAPHICS GRAPHICS_CONTROL::MAX_PACKET_BYTES
#else	// _CONTROL_CLIENT
#define NUM_GOBJECT_FRAME 2
#define NUM_BYTES_PACKET_CONTROL_GRAPHICS ((sizeof(unsigned int)*2 + (NRMKHelper::NRMKGObject::PACKET_HEADER_BYTE + NRMKHelper::NRMKGObjectFrame::MAX_PACKET_BYTE)*NUM_GOBJECT_FRAME))
#undef NUM_GOBJECT_FRAME
#endif	// _CONTROL_CLIENT

#if defined(_GOBJECTS_ENABLED)
#if !defined(_FORCE_SENSOR_FEEDBACK)
typedef NRMKHelper::NRMKControlSocket<2*ROBOT::JOINT_DOF*sizeof(float), ROBOT::JOINT_DOF*sizeof(float), 0, ROBOT::JOINT_DOF, NUM_BYTES_PACKET_CONTROL_GRAPHICS> CONTROL_SOCKET;
#else
typedef NRMKHelper::NRMKControlSocket<(2*ROBOT::JOINT_DOF + 6)*sizeof(float), ROBOT::JOINT_DOF*sizeof(float), 0, ROBOT::JOINT_DOF, NUM_BYTES_PACKET_CONTROL_GRAPHICS> CONTROL_SOCKET;
#endif //_FORCE_SENSOR_FEEDBACK
#else	// _GOBJECTS_ENABLED
#if !defined(_FORCE_SENSOR_FEEDBACK)
typedef NRMKHelper::NRMKControlSocket<2*ROBOT::JOINT_DOF*sizeof(float), ROBOT::JOINT_DOF*sizeof(float), 0, ROBOT::JOINT_DOF> CONTROL_SOCKET;
#else
typedef NRMKHelper::NRMKControlSocket<(2*ROBOT::JOINT_DOF + 6)*sizeof(float), ROBOT::JOINT_DOF*sizeof(float), 0, ROBOT::JOINT_DOF> CONTROL_SOCKET;
#endif	// _FORCE_SENSOR_FEEDBACK
#endif // _GOBJECTS_ENABLED

typedef NRMKHelper::SystemInterface_Socket<ROBOT, CONTROL_SOCKET> SYSTEM_INTERFACE;
#endif	// _USE_CONTROL_PENDANT_SOCKET

#endif	// _SYS_INTERFACE_VER_SHMEM

#endif	// defined(_APP_MANAGER) || defined(_CONTROL_CLIENT)

#endif // _CONTROLLER
