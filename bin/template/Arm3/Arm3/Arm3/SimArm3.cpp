/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#include <iostream>

#include "Eigen/Eigen"
using namespace Eigen;

#include "LieGroup/LieGroup.h"
using namespace LieGroup;

#include "NRMKFrameworkArm3.h"

#if !defined(KIN_CONTROL)
#include "AMBS/ForwardDynamics.h"
#include "AMBS/DynamicsSolver.h"
#endif

#include "KIMMRobotControlSocket.h"
typedef Eigen::Matrix<double, ROBOT::JOINT_DOF, 1> JointVec;

using namespace NRMKFoundation;

#if defined(_CONTROLLER)
// Controller header file
#include "ControlArm3/ControlArm3.h"

// TYPE-DEFINITION OF ROBOT CONTROLLER
typedef ControlArm3 CONTROLLER;
#endif 

NRMKHelper::KIMMRobotControlSocket<ROBOT::NUM_JOINTS> _gui_socket;

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
	std::cout << "++++ INTEGRATED APPLICATION : Arm3 ++++" << std::endl;

	// create a robot instance
	// When the robot is created, it should have zero configurations.
	// This is very important for proper animation in CADKit Viewer.
	ROBOT robot;	
	
	// Create and start NRMKSimulationSocket server 
	SIMULATION_SOCKET simsocket(robot);	
	if (simsocket.startServer(SOCK_TCP, NRMK_PORT_SIM))
		std::cout << "Simulation server started at IP of : " << simsocket.getAddress() << " on Port: " << NRMK_PORT_SIM << std::endl;

	simsocket.waitForConnection(0);

	// Create and start NRMKDataSocket server
	DATA_SOCKET datasocket(robot); 
	if (datasocket.startServer(SOCK_TCP, NRMK_PORT_DATA))
		std::cout << "Data server started at IP of : " << datasocket.getAddress() << " on Port: " << NRMK_PORT_DATA << std::endl;

	datasocket.waitForConnection(0);

	DATA_SOCKET datasocket2(robot); 
	if (datasocket2.startServer(SOCK_TCP, NRMK_PORT_DATA+1))
		std::cout << "Data server2 started at IP of : " << datasocket2.getAddress() << " on Port: " << NRMK_PORT_DATA+1 << std::endl;

	datasocket2.waitForConnection(0);
#if defined(_SYNCHRO)
	datasocket2.enhanceSpeed(false);//sychro (no data lose)
#else
	datasocket2.enhanceSpeed(true);//real time (data lose)
#endif

	if (_gui_socket.startServer(SOCK_TCP, 8888))
		std::cout << "Pendant server started at IP of : " << _gui_socket.getAddress() << " on Port: " << 8888 << std::endl;

	_gui_socket.waitForConnection(0);

	// number of steps for data socket
	int step_data 	= 10; // (int) (DELT_DATA/UNIT_DELT);
	int step_gui	= 20;
	int tickGui		= 0;
	
#if defined(_CONTROLLER)
	// Setup controller
	CONTROLLER controller;

	// First, set the control period
	int step_control = 1;
	double dt_control = step_control*UNIT_DELT;	// control period 
	controller.setPeriod(dt_control);
	// and initialize gains ans set nominal system
	controller.initController();

	// Next, set system interface
	SYSTEM_INTERFACE sys_interface(&robot);	
#endif


	// print message
	std::cout << "Now ready to run NRMKFoundation simulation for Arm3 ..." << std::endl;
	std::cout << "Press TAB to start/hold/resume simulation." << std::endl;
	std::cout << "Press Escape to quit simulation." << std::endl;

	// setup clock and timer
	double t = 0;
	// set the simulation period
	double dt = UNIT_DELT; // simulation period <= control period

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

	// set initial configuration: Do not move this. 
	robot.setInitialConfiguration();

	// run simulator
	bool quit = false;
	bool run = false;
	do
	{
		// process keyboard events
		unsigned char key = 0; 

		#if defined(_CONTROLLER)
		key = controller.getTimerKey(); //0;
		
		if (key == 0) // key = 0 means no keyboard input
			_gui_socket.getKey(key);
#endif
		
		if (key == 0)
		{
			if (run || t == 0)
				key = TimerKey(t);
		}

		if (key == 0) 
		{
			if (simsocket.hasConnection())
				simsocket.getKey(key);
		}

		if(key == 0)
			key = NRMKHelper::GetVKKey();	
				
#if defined(_CONTROLLER)
		if (key != 0) // key = 0 means no keyboard input
			controller.kbCommand(key);
#endif

		if (key == VK_TAB)	// start simulation
			run = !run;

		else if (key == VK_ESCAPE)
			quit = true;

		// execution control
		if (quit)
			break;

		if (!run)
		{
			Sleep(100);
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

			sys_interface.setReadData();


			//////////////////////////////////////////////////////////////////////////
			// Control computation			
			// set joint measurement
			if (sys_interface.waitForReadDataReady())
			{
#if defined(_FORCE_SENSOR_FEEDBACK)
				float sensors[2*ROBOT::JOINT_DOF + 6] = { 0 };
#else
				float sensors[2*ROBOT::JOINT_DOF] = { 0 };
#endif
			
				sys_interface.readData(sensors);

				for (int k = 0; k < ROBOT::JOINT_DOF; k++)
				{
					controller.q()[k] = sensors[k];
					controller.qdot()[k] = sensors[k + ROBOT::JOINT_DOF];
				}

#if defined(_FORCE_SENSOR_FEEDBACK)
				for (int k = 0; k < 6; k++)
					controller.Fext()[k] = sensors[k + 2*ROBOT::JOINT_DOF];
#endif
			}
		
			controller.reflectJointState();
			controller.update(t);

			// actuate
			float actuators[ROBOT::JOINT_DOF] = { 0 };
			for (int k = 0; k < ROBOT::JOINT_DOF; k++)
				actuators[k] = (float) controller.tau()[k];

			sys_interface.writeData(actuators);
			//////////////////////////////////////////////////////////////////////////

			if (sys_interface.waitForWriteDataReady())		
				sys_interface.setWriteData();
		}
#endif

		// send joint angle data to ...
		// TO DO: You have to prepare data for NRMKDataSocket 
		if (datasocket.hasConnection() && tickData-- == 0)
		{
			tickData = step_data - 1; // 'minus one' is necessary for intended operation
			
			datasocket.update((float) t);	
		}

		// send joint angle data to ...
		// TO DO: You have to prepare data for NRMKDataSocket 
		if (datasocket2.hasConnection() && tickData2-- == 0)
		{
			tickData2 = step_data - 1; // 'minus one' is necessary for intended operation
			
			datasocket2.update((float) t);	
		}
		
		if(_gui_socket.hasConnection() && tickGui-- == 0){
		
			tickGui = step_gui - 1;

			JointVec _q;

			for (int k = 0; k < ROBOT::JOINT_DOF; k++)
			{
				_q[k] = (float) controller.q()[k];
			}

			_gui_socket.sendCurrentData(t, _q);
		
		}

#if defined(KIN_CONTROL)
		robot.q() = robot.tau();
#else
		// solve forward dynamics and integrate the physics. 
		// Upon completion, the system state (not context) is updated.
		stepper.step(t, dt);
#endif

		Sleep(1);
		// lapse the time
		t += dt;	

		// TO DO: You may set the simulation duration
		//	if (t > 10.0)
		// 		quit = true;

	} while (1);

	std::cout << "Now quitting the integrated applications at time = " << t << std::endl;

	return 0;
}

