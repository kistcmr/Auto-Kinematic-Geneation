/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */

#ifdef _APP_MANAGER
#include <iostream>

#include "Eigen/Eigen"
using namespace Eigen;

#include "LieGroup/LieGroup.h"
using namespace LieGroup;

#include "NRMKFrameworkArm3.h"

#if !defined(KIN_CONTROL)
#include "AMBS/ForwardDynamics.h"
#include "AMBS/DynamicsSolver.h"
#endif // KIN_CONTROL

using namespace NRMKFoundation;

// Generate key inputs by time
unsigned char TimerKey(double time) 
{
	static long tick = 0L;

	unsigned char key = 0;

	// TO DO: You can generate key command by time.

	// For example, if you want simulation to quit automatically at simulation time of 10(sec)
	// activate the following.
// 	static const long tick_final = TIME_TO_TICK(10.0);
// 	if (tick == tick_final)
// 		key = VK_ESCAPE;

	// Also, you can automatically starts the simulation by activating the following.
// 	if (tick == 0L)
// 		key = VK_TAB;

	tick++;

	return key;
}

#ifdef __GNUC__
int main(int argc, char* argv[])
#else
int _tmain(int argc, _TCHAR* argv[])
#endif
{
	std::cout << "++++ APPLICATION MANAGER : Arm3 ++++" << std::endl;
	
	// create a robot instance
	// When the robot is created, it should have zero configurations.
	// This is very important for proper animation in CADKit Viewer.
	ROBOT robot;
	
	// Create and start NRMKSimulationSocket server 
	// TO DO: You have to fill in the body index array. 
	// See NRMKFrameworkArm3.cpp.
	SIMULATION_SOCKET simsocket(robot);	

	if (simsocket.startServer(SOCK_TCP, NRMK_PORT_SIM))
		std::cout << "Simulation server started at IP of : " << simsocket.getAddress() << " on Port: " << NRMK_PORT_SIM << std::endl;

	simsocket.waitForConnection(0);

	DATA_SOCKET datasocket(robot); 

	if (datasocket.startServer(SOCK_TCP, NRMK_PORT_DATA))
		std::cout << "Data server started at IP of : " << datasocket.getAddress() << " on Port: " << NRMK_PORT_DATA << std::endl;

	datasocket.waitForConnection(0);

	DATA_SOCKET datasocket2(robot); 

	if (datasocket2.startServer(SOCK_TCP, NRMK_PORT_DATA+1))
		std::cout << "Data server started at IP of : " << datasocket2.getAddress() << " on Port: " << NRMK_PORT_DATA+1 << std::endl;

	datasocket2.waitForConnection(0);
#if defined(_SYNCHRO)
	datasocket2.enhanceSpeed(false);//sychro (no data lose)
#else
	datasocket2.enhanceSpeed(true);//real time (data lose)
#endif

	// number of steps for data socket
	int step_data = 10; // (int) (DELT_DATA/UNIT_DELT);

#if defined(_GOBJECTS_ENABLED)
	// Create and start NRMKGraphicsSocket server
	GRAPHICS_SOCKET graphicssocket(robot);

	if (graphicssocket.startServer(SOCK_TCP, NRMK_PORT_GRAPHICS))
		std::cout << "Graphics server started at IP of : " << graphicssocket.getAddress() << " on Port: " << NRMK_PORT_GRAPHICS << std::endl;

	graphicssocket.waitForConnection(0);
#endif

#if defined(_CONTROLLER)
	// First, set the control period
	int step_control = 1;

#if defined(_USE_COMMAND_SOCKET)
	// setup the command socket server (to send a command to the control clients)
	COMMAND_SOCKET	commandsocket;

	if (commandsocket.startServer(SOCK_TCP, NRMK_PORT_COMMAND))
	{
		std::cout << "Command server started at IP of : " << commandsocket.getAddress() << " on Port: " << NRMK_PORT_COMMAND << std::endl;
		std::cout << "Waiting for a control client to connect to the command server..." << std::endl;
	}

	// wait indefinitely for control socket to be connected
	commandsocket.waitForConnection(0);
#endif	// _USE_COMMAND_SOCKET

/// TO DO: Define _SYS_INTERFACE_VER_SHMEM if the control client application runs in the same platform.
///		   Otherwise, socket-based system interface is used.
#if defined(_SYS_INTERFACE_VER_SHMEM)
	// Instantiate a system interface object (using shared memory)
	SYSTEM_INTERFACE sys_interface;
#else	// _SYS_INTERFACE_VER_SHMEM
#if !defined(_USE_PENDANT_SOCKET) // FIXED BY THACHDO 20150918
	// Control client communicates with this application manager via socket.
	CONTROL_SOCKET controlsocket;

	if (controlsocket.startServer(SOCK_TCP, NRMK_PORT_CONTROL))
	{
		std::cout << "Control server started at IP of : " << controlsocket.getAddress() << " on Port: " << NRMK_PORT_CONTROL << std::endl;
		std::cout << "Waiting for a control client to connect..." << std::endl;
	}

	// wait indefinitely for control socket to be connected
	controlsocket.waitForConnection();

	// Once connection has been established, system interface is instantiated.
	SYSTEM_INTERFACE sys_interface(controlsocket);
	sys_interface.setSystem(&robot);
#endif	// _USE_CONTROL_PENDANT_SOCKET
#endif	// _SYS_INTERFACE_VER_SHMEM

#endif	// _CONTROLLER

	// Connect to application manager 
	NRMK_CHAR sIP[256] = "127.0.0.1"; 
	// Default ip where application manager is to run is localhost.
	// Otherwise, please pass IP as an command line argument 
	if (argc > 1)
#ifdef __GNUC__
		strcpy(sIP, argv[1]);
#else
		::WideCharToMultiByte(CP_ACP, 0, argv[1], 256, sIP, 256, NULL, NULL);
#endif

#if defined(_USE_PENDANT_SOCKET)
	PENDANT_SOCKET	pendantsocket;

	while (pendantsocket.connect(sIP, NRMK_PORT_ANIMATION) != 0)
		Sleep(100);

	pendantsocket.setConnected();
	std::cout << "Pendant client connected to IP of : " << sIP << " on Port: " << NRMK_PORT_ANIMATION << std::endl;
#endif // _USE_PENDANT_SOCKET

	// print message
	std::cout << "Now ready to run NRMKFoundation simulation for Arm3 ..." << std::endl;
	std::cout << "Press TAB to start/hold/resume simulation." << std::endl;
	std::cout << "Press Escape to quit simulation." << std::endl;

	// setup clock and timer
	double t = 0;
	// set the simulation period
	double dt = UNIT_DELT;	// control period <= simulation period

	int tickControl = 0;
	int tickData = 0; 
	int tickData2 = 0;

#if !defined(KIN_CONTROL)
	// setup integrator as well as forward dynamics algorithm
	ABMethod<ROBOT> abmethod(robot, Vector3D(0, 0, -GRAV_ACC));

	DynamicsSolver< ABMethod<ROBOT>, EulerCromerMethod<ROBOT> > stepper(abmethod);
	// TO DO: De-comment the following when the robot has joint damping
	stepper.setPeriod(dt);	// This is optional. If it is called with nonzero period,
	// joint stiffness and damping is simulated with implicit scheme.
#endif

	// set initial configuration 
	// FIXED (@20131107): Do not move this. 
	robot.setInitialConfiguration();

	// run simulator
	bool quit = false;
	bool run = false;
	do
	{
		// process keyboard events
		unsigned char key = 0; 
		if (key == 0)
		{
			if (run || t == 0)
				key = TimerKey(t);
		}

		if (key == 0) 
		{
			if (simsocket.hasConnection())
				simsocket.getKey(key);
			else	// in case no key input from console, get key from NRMKSocket
				key = NRMKHelper::GetVKKey();
		}

#if defined(_CONTROLLER)
		if (key != 0) // key = 0 means no keyboard input
		{
#if defined(_USE_COMMAND_SOCKET)
			commandsocket.sendKey(key);
#else	// _USE_COMMAND_SOCKET
#if defined(_SYS_INTERFACE_VER_SHMEM)
			sys_interface.setKey(key);
#endif	// _SYS_INTERFACE_VER_SHMEM
#endif	// _USE_COMMAND_SOCKET
		}
#endif	// _CONTROLLER

		if (key == VK_TAB)	// start simulation
			run = !run;
		
		else if (key == VK_ESCAPE)
		{
			quit = true;
			// The following is necessary for the control client to get signaled 
#if defined(_CONTROLLER) && !defined(_USE_PENDANT_SOCKET)
			sys_interface.setReadData();
#endif
		}

		// execution control
		if (quit)
			break;

		if (!run)
		{
			Sleep(10);
			continue;
		}

		/// TO DO: Here is a good place to display system states if you want.
		// 	std::cout << "time = " << t << " : ";
		//	std::cout << "q = " << robot.q().array()*RADIAN << std::endl;

		// first, robot kinematics context is updated..
#if defined(KIN_CONTROL)
		robot.update(true);
#else
		//robot.update();
		stepper.updateRobot();
#endif

		// send transformations to CADKitVIewer
		if (simsocket.hasConnection())
		{
			simsocket.updateTransform((float) t);

#if !defined(KIN_CONTROL)
			// add force/moment
			for (int k = 0; k < ROBOT::NUM_BODIES; k++)
			{
				if (simsocket.interactivity(k).hasInteractivity())
					robot.body(k).addExtWrench(simsocket.interactivity(k).F(), simsocket.interactivity(k).at());
			}
#endif
		}

#if defined(_CONTROLLER)
		if (tickControl-- == 0)
		{
			tickControl = step_control - 1; // 'minus one' is necessary for intended operation

#if defined(_USE_CONTROL_PENDANT_SOCKET)
			float const *x; 
			float t_pendant;

			pendantsocket.getControlState(t_pendant, x);			
			if (x != NULL)
			{
				t = t_pendant;
				std::cout << "t = " << t << " :  q = [ ";

				for (int k = 0; k < ROBOT::JOINT_DOF; k++)
				{
					robot.q()[k] = (double) x[k];
					std::cout << robot.q()[k] << " ";
					robot.qdot()[k] = (double) x[k + ROBOT::JOINT_DOF];
					robot.tau()[k] = (double) x[k + ROBOT::JOINT_DOF*2];
				}
				std::cout << std::endl;
			}
			
#elif defined(_USE_PENDANT_SOCKET)

			float const *q; 
			float t_pendant;

			pendantsocket.getJointPos(t_pendant, q);
			if (q != NULL)
			{
				t = t_pendant;
				std::cout << "t = " << t << " :  q = [ ";

				for (int k = 0; k < ROBOT::JOINT_DOF; k++)
				{
					robot.q()[k] = (double) q[k];
					std::cout << robot.q()[k] << " ";
				}
				std::cout << std::endl;
			}
			
#else	// !(_USE_PENDANT_SOCKET ||_USE_CONTROL_PENDANT_SOCKET)

			sys_interface.setReadData();

			// Wait until the control client completes actuator value..
			if (sys_interface.waitForWriteDataReady())		
				sys_interface.setWriteData();
#endif	// _USE_CONTROL_PENDANT_SOCKET
		}
#endif // _CONTROLLER

		// send simulation/control data to ...
		// TO DO: You have to prepare data for NRMKDataSocket 
		if (datasocket.hasConnection() && tickData-- == 0)
		{
			tickData = step_data - 1; // 'minus one' is necessary for intended operation
		
			datasocket.update((float) t);
		}

		if (datasocket2.hasConnection() && tickData2-- == 0)
		{
			tickData2 = step_data - 1; // 'minus one' is necessary for intended operation
		
			datasocket2.update((float) t);
		}

#if defined(_GOBJECTS_ENABLED)
#if !defined(_USE_PENDANT_SOCKET)
#if defined(_CONTROLLER)
		unsigned char control_graphics[CONTROL_SOCKET::NUM_BYTES_CONTROL_GRAPHICS];
		if (controlsocket.retrieveControlGraphics(control_graphics) == -1)
			std::cout << "Error in data transmission from control socket.." << std::endl;

		if (graphicssocket.hasConnection())
			graphicssocket.update(controlsocket.numGraphicsObjects(), controlsocket.numBytesGraphicsData(), control_graphics);

		controlsocket.clearControlGraphics();
#else
		if (graphicssocket.hasConnection())
			graphicssocket.update();
#endif	// _CONTROLLER
#endif	// _USE_PENDANT_SOCKET
#endif // _GOBJECTS_ENABLED

#if defined(KIN_CONTROL)
#if !defined(_USE_PENDANT_SOCKET)
		robot.q() = robot.tau();
#endif
#else // KIN_CONTROL
		// solve forward dynamics and integrate the physics. 
		// Upon completion, the system state (not context) is updated.
		stepper.step(t, dt);
#endif	// KIN_CONTROL

#if !defined(_USE_PENDANT_SOCKET)
		// lapse the time
		t += dt;	
#endif // _USE_PENDANT_SOCKET

		//////////////////////////////////////////////////////////////////////////
		// TO DO: You may set the simulation duration
		//	if (t > 10.0)
		// 		quit = true;

	} while (1);

	std::cout << "Now quitting the integrated applications at time = " << t << std::endl;

	// When the application manager is terminated, it also terminates the control client.
	return 0;
}

#endif // _APP_MANAGER
