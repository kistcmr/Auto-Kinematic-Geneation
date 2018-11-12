/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#ifdef __XENO__

//-system-/////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>		// string function definitions
#include <fcntl.h>		// File control definitions
#include <errno.h>		// Error number definitions
#include <termios.h>	// POSIX terminal control definitions
#include <time.h>		// time calls
#include <sys/ioctl.h>

//-xenomai-///////////////////////////////////////////////////////////////
#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <rtdk.h>		//The rdtk real-time printing library

//NRMK ///////////////////////////////////////////////////////////////////
#include "Eigen/Eigen"
using namespace Eigen;

#include "LieGroup/LieGroup.h"
using namespace LieGroup;

#include "NRMKFrameworkArm3.h"
using namespace NRMKFoundation;

#include "KIMMRobotControlSocket.h"

// XENOMAI RT-TASK
RT_TASK control_task;
RT_TASK print_task;
RT_TASK gui_task;

//variables must be global---------------
CONTROLLER controller;

#if defined(_USE_COMMAND_SOCKET)
COMMAND_SOCKET	commandsocket;
#endif

#if defined(_USE_PENDANT_SOCKET)

RT_TASK pendantsocketsend_task;
//PENDANT_SOCKET pendantsocket;
PENDANT_SOCKET pendantsocket(controller.robot());

#endif	// _USE_PENDANT_SOCKET

#if defined(_USE_ETHERCAT)

ANIMATION_SOCKET animationsocket;
SYSTEM_INTERFACE sys_interface;

#else	// !(_USE_ETHERCAT)

RT_TASK controlsocket_task;
CONTROL_SOCKET controlsocket;
SYSTEM_INTERFACE sys_interface(controlsocket);

#endif	// _USE_ETHERCAT


#if defined(_GOBJECTS_ENABLED)
// Create and start NRMKGraphicsSocket server
GRAPHICS_CONTROL control_graphics(&controller);
#endif

DATA_SOCKET datasocket(controller.robot());

volatile bool data_ready = false;
volatile double t = 0;
static int step_control = 1;
static double dt_control = step_control*UNIT_DELT;	// control period = simulation period

// number of steps for data socket
volatile int step_data = 10;
volatile int tickData = 0; 

volatile bool run = false;
volatile bool quit = false;
volatile bool working = false;

#if defined(_USE_ETHERCAT)

#if defined(_FORCE_SENSOR_FEEDBACK)
static double states[3*ROBOT::JOINT_DOF + 6] = { 0 };
#else // !(_FORCE_SENSOR_FEEDBACK)
static double states[2*ROBOT::JOINT_DOF] = { 0 };
#endif // _FORCE_SENSOR_FEEDBACK

static double actuators[ROBOT::JOINT_DOF] = { 0 };

#else // !(_USE_ETHERCAT)

#if defined(_FORCE_SENSOR_FEEDBACK)
static float sensors[2*ROBOT::JOINT_DOF + 6] = { 0 };
#else // !(_FORCE_SENSOR_FEEDBACK)
static float sensors[2*ROBOT::JOINT_DOF] = { 0 };
#endif

static float actuators[ROBOT::JOINT_DOF] = { 0 };

#endif // _USE_ETHERCAT

NRMKHelper::KIMMRobotControlSocket<ROBOT::NUM_JOINTS> _gui_socket;

//control task
void control_run(void *arg)
{
	UINT* period = (UINT* ) arg;
	rt_task_set_periodic(NULL, TM_NOW, *period);

	while (working)
	{
		rt_task_wait_period(NULL);	
	
#if defined(_USE_ETHERCAT)	
		sys_interface.processReadData();
#endif

		// modified @20140611
		unsigned char key = controller.getTimerKey(); //0;

		if (key == 0) // key = 0 means no keyboard input
			commandsocket.getKey(key);

#if defined(_USE_PENDANT_SOCKET)
		if (key == 0) // key = 0 means no keyboard input
			pendantsocket.getKey(key);
#endif

		if (key != 0) // key = 0 means no keyboard input
			controller.kbCommand(key);

		if (key == VK_TAB)	// Application manager starts/holds/resumes
			run = !run;
		else if (key == VK_ESCAPE)
		{
			working = false;
			quit = true;
		}

		if (quit)
			break;

		if (!run)
		{
#if defined(_USE_ETHERCAT)
			// Before starting...
			// FIXED BY THACH DO 20150919
			sys_interface.readData(states);
			for (unsigned int k = 0; k < ROBOT::JOINT_DOF; k++)
			{
				controller.q()[k] = states[k];
				controller.qdot()[k] = states[k + ROBOT::JOINT_DOF];
				
#if defined(KIN_CONTROL)	// --> CSP
				controller.tau()[k] = controller.q()[k];

				actuators[k] = (double) controller.tau()[k];				
				
#else	// DYNAMIC_CONTROL --> CST

				controller.tau()[k] = 0;

				actuators[k] = (double) controller.tau()[k];		
				
#endif	// KIN_CONTROL
			}

			sys_interface.writeData(actuators);
			sys_interface.processWriteData();
#endif // _USE_ETHERCAT
			
			continue;
		}

#if !defined(_USE_ETHERCAT)
		// wait till the sensor data is ready
		if (sys_interface.waitForReadDataReady())
		{
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
		
#else	// _USE_ETHERCAT
			// FIXED BY THACH DO 20150919
			sys_interface.readData(states);
			for (int k = 0; k < ROBOT::JOINT_DOF; k++)
			{
				controller.q()[k] = states[k];
				controller.qdot()[k] = states[k + ROBOT::JOINT_DOF];
				
				if (controller.qdot()[k] > PI)
					working = false;
			}
#endif	// _USE_ETHERCAT
		
		//controller.estimate();
		controller.reflectJointState();
		controller.update(t);

		// Before setting the actuator data, it is necessary to set the control data and graphics data.
#if !defined(_USE_ETHERCAT)
		// The following is for control data transmission
		if (tickData-- == 0/* && controlsocket.hasConnection()*/)
		{
			float cdata[CONTROL_SOCKET::NUM_FLOATS_CONTROL_DATA];
			for (int k = 0; k < ROBOT::JOINT_DOF; k++)
				cdata[k] = (float) controller.tau()[k];

			controlsocket.setControlData((unsigned char const * const) cdata);

			tickData = step_data - 1; // 'minus one' is necessary for intended operation
		}

#if defined(_GOBJECTS_ENABLED)
		// The following is for control graphics transmission
		int num_bytes = control_graphics.updatePacketData();
		if (num_bytes > 0)
			controlsocket.setGraphicsData(control_graphics.data(), num_bytes);
#endif

		controlsocket.signal_off(1);//turn off signal 1: complete computation time
		controlsocket.signal_on(0); //turn on signal 0:  start writing output data

		// Now write the data..
		for (int k = 0; k < ROBOT::JOINT_DOF; k++)
			actuators[k] = (float) controller.tau()[k];

		sys_interface.writeData(actuators);
#else	// _USE_ETHERCAT
		// Now write the data..
		for (int k = 0; k < ROBOT::JOINT_DOF; k++)
			actuators[k] = (double) controller.tau()[k];

		// TO DO: We intentionally block sending actuator command. 
		sys_interface.writeData(actuators);
		sys_interface.processWriteData();
		//**************************************************************
#endif	// _USE_ETHERCAT

		// lapse the time
		t += dt_control;
	}
}

void print_run(void *arg)
{
	rt_task_set_periodic(NULL, TM_NOW, 2e9);
	while (working)
	{
		rt_printf("time = %f\n", t);

#if defined(_USE_ETHERCAT)
		// Print status word
		rt_printf("status: ");
		for (unsigned int k = 0; k < ROBOT::JOINT_DOF; k++)
			rt_printf("0x%x,  ", sys_interface.getAxisStatus(k));

		rt_printf("\n");

#if defined(KIN_CONTROL) // --> CSP
		rt_printf("target position (cnt): ");
		for (unsigned int k = 0; k < ROBOT::JOINT_DOF; k++)
			rt_printf("%i,  ", sys_interface.getActPos(k));		
			
#else	// DYNAMIC_CONTROL --> CST

		rt_printf("target torque (cnt): ");
		for (unsigned int k = 0; k < ROBOT::JOINT_DOF; k++)
			rt_printf("%i,  ", sys_interface.getActTor(k));
#endif

		rt_printf("\n");
#endif	// _USE_ETHERCAT

		rt_printf("q (deg): ");
		for (unsigned int k = 0; k < ROBOT::JOINT_DOF; k++)
			rt_printf("%f,  ", controller.q()[k]*RADIAN);

		rt_printf("\n");

		rt_printf("control torque: ");
		for (unsigned int k = 0; k < ROBOT::JOINT_DOF; k++)
			rt_printf("%f,  ", controller.tau()[k]);
		
		rt_printf("\n\n");

		rt_task_wait_period(NULL); //wait for next cyclert_printf("position: ");
	}
}

#if (0) //defined(_USE_PENDANT_SOCKET)
void pendantsocketsend_run(void *arg)
{
	rt_task_set_periodic(NULL, TM_NOW, 2e7);

	while (working)
	{
		if (run && pendantsocket.hasConnection())
		{
#if defined(_USE_ETHERCAT)
			// FIXME@20140930
			pendantsocket.setControlState((float) t, states);

#else
#if defined(KIN_CONTROL)
			/// TO DO: Set joint angles for animation
			for (int i = 0; i < ROBOT::JOINT_DOF; i++)
				// Send IK solution
				sensors[i] = (float) controller.tau()[i];
#endif	// KIN_CONTROL	

			pendantsocket.setJointPos((float) t, sensors);
#endif	// _USE_ETHERCAT
		}

		rt_task_wait_period(NULL);
	}
}

#else // !(_USE_PENDANT_SOCKET)


void gui_run(void *arg)
{
	float animationData[ROBOT::JOINT_DOF];

	double q[ROBOT::JOINT_DOF];
	double qdot[ROBOT::JOINT_DOF];
	double tau[ROBOT::JOINT_DOF];
	
	rt_task_set_periodic(NULL, TM_NOW, 1e7);

	while (working)
	{
		rt_task_wait_period(NULL);

#if defined(_USE_ETHERCAT)
		if (animationsocket.hasConnection())
		{
			for (int k = 0; k < ROBOT::JOINT_DOF; k++)
			{
				animationData[k] = (float) states[k];
				//animationData[k + ROBOT::JOINT_DOF] = (float) sensors[k + ROBOT::JOINT_DOF];
			}
			animationsocket.setJointPos((float) t, animationData);
		}
		
#endif // _USE_ETHERCAT

		if (datasocket.hasConnection())
		{
			datasocket.update((float) t);	
		}

#if defined(_USE_PENDANT_SOCKET)
		// The following is for pendant data transmission
		if (pendantsocket.hasConnection())
			pendantsocket.update(t);
#endif

		if (_gui_socket.hasConnection())
		{
			unsigned char key = 0;
			if (key == 0) // key = 0 means no keyboard input
				_gui_socket.getKey(key);


			if (key != 0) // key = 0 means no keyboard input
				controller.kbCommand(key);

			char _cmd = _gui_socket.getCommandMode();

			_gui_socket.sendCurrentData(t, controller.q());
		}
	}
}

#if !defined(_USE_ETHERCAT)
//reading socket data
void controlsocket_run(void *arg)
{
	UINT* period = (UINT* ) arg;
	rt_task_set_periodic(NULL, TM_NOW, *period);

	while (controlsocket.IsOpen())
	{
		controlsocket.ReadCommRT(); //this must be called here
		rt_task_wait_period(NULL);
	}
}
#endif // _USE_ETHERCAT

#endif // (_USE_PENDANT_SOCKET)


void signal_handler(int signum) 
{
	rt_task_delete(&control_task);
	rt_task_delete(&print_task);

#if (0) //defined(_USE_PENDANT_SOCKET)
	rt_task_delete(&pendantsocketsend_task);
	
#else // !(_USE_PENDANT_SOCKET)


#if defined(_USE_ETHERCAT)
	// FIXED BY THACHDO 20150919
	rt_task_delete(&gui_task);
	sys_interface.deinit();
	
#else // !(_USE_ETHERCAT)

	rt_task_delete(&controlsocket_task);
#endif // (_USE_ETHERCAT)

#endif // (_USE_PENDANT_SOCKET)

	printf("Stopped!\n");

	usleep(20000);
	exit(1);
}

int main(int argc, char* argv[])
{	
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	printf("++++ CONTROL CLIENT : Arm3 ++++\n");

	//////////////////////////////////////////////////////////////////////////
	// Setup controller
	//
	// First, set the control period
	controller.setPeriod(dt_control);
	// and next initialize the controller including gains and the nominal system
	controller.initController();	

	// Connect to application manager 
	char sIP[256] = "127.0.0.1"; //default host: localhost
	// Default ip where application manager is to run is localhost.
	// Otherwise, please pass IP as an command line argument 
	if (argc > 1) 
		strcpy(sIP, argv[1]);

#if defined(_USE_COMMAND_SOCKET)
	// Connect to the command socket server to get the command
	while (commandsocket.connect(sIP, NRMK_PORT_COMMAND) != 0) //commandsocket runs in normal mode
		Sleep(100);

	// in order to initiate the socket communication
	commandsocket.setConnected();
	printf("Command client connected to IP of : %s on Port: %i\n", sIP, NRMK_PORT_COMMAND);
#endif

	// TO DO: Create control socket server
	if (_gui_socket.startServer(SOCK_TCP, 8888))
		printf("Control server started at IP of : %s on Port: %d\n", _gui_socket.getAddress(), 8888);

	printf("Waiting for GUI Control Tool to connect...\n");
	_gui_socket.waitForConnection(0);

#if defined(_USE_PENDANT_SOCKET)
	if (pendantsocket.startServer(SOCK_TCP, NRMK_PORT_PENDANT))
		std::cout << "Pendant server started at IP of : " << pendantsocket.getAddress() << " on Port: " << NRMK_PORT_PENDANT << std::endl;

	pendantsocket.waitForConnection(0);
	
#else	// !(_USE_PENDANT_SOCKET)
#endif	// _USE_PENDANT_SOCKET

#if defined(_USE_ETHERCAT)
	// Initialize EtherCAT system interface
#if defined(KIN_CONTROL)
	
	sys_interface.init(sIP, OP_MODE_CYCLIC_SYNC_POSITION, dt_control);
	
#else // DYNAMIC_CONTROL
	
	sys_interface.init(sIP, OP_MODE_CYCLIC_SYNC_TORQUE, dt_control);
	
#endif

	if (animationsocket.startServer(SOCK_TCP, NRMK_PORT_ANIMATION))
		std::cout << "Animation server started at IP of : " << animationsocket.getAddress() << " on Port: " << NRMK_PORT_ANIMATION << std::endl;

	animationsocket.waitForConnection(0);

#else // !(_USE_ETHERCAT)

	// Connect to the control socket server to interact with the control signals
	while (controlsocket.connect(sIP, NRMK_PORT_CONTROL, REALTIME_RUN) != 0)  //controlsocket runs in RT mode
		Sleep(100);
	
	// in order to initiate the socket communication
	controlsocket.setConnected();
	printf("Control client connected to IP of : %s on Port: %i\n", sIP, NRMK_PORT_CONTROL);
#endif // _USE_ETHERCAT

#if defined(_GOBJECTS_ENABLED)
	// Create and start NRMKGraphicsSocket server
	//////////////////////////////////////////////////////////////////////////
	GRAPHICS_CONTROL control_graphics(&controller);
#endif

//#endif	// _USE_PENDANT_SOCKET

	// TO DO: Create data socket server
	if (datasocket.startServer(SOCK_TCP, NRMK_PORT_DATA))
		printf("Data server started at IP of : %s on Port: %d\n", datasocket.getAddress(), NRMK_PORT_DATA);
	
	printf("Waiting for Data Scope to connect...\n");
	datasocket.waitForConnection(0);
	

	// Perform auto-init of rt_print buffers if the task doesn't do so
	rt_print_auto_init(1);
	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);

	// print message
	working = true; //condition for control loop

	rt_printf("Now ready to run NRMKFoundation CONTROL CLIENT for Arm3 ...\n");
	rt_printf("Start the application manager ...\n");
	rt_printf("Quiting the application manager terminates the control client...\n");

	/// TO DO: Fix your control frequency according to UNIT_DELT in NRMKFrameworkArm3.h. 
	UINT control_period = dt_control*1e9;  	//sampling period for control task (unit in nano-second)
	
	// control task: create and start
	/// TO DO: In case of segmentation fault due to stack size, set larger stack.
	rt_task_create(&control_task, "control", 100*1024, 90, 0); //stack size of 100KB
	rt_task_start(&control_task, &control_run, &control_period);

	// printing: create and start
	rt_task_create(&print_task, "printing", 0, 75, 0);
	rt_task_start(&print_task, &print_run, NULL);

	rt_task_create(&gui_task, "Graphic UI", 0, 80, 0); //predefined stack size will be used for this task
	rt_task_start(&gui_task, &gui_run, NULL);
	
#if (0) //defined(_USE_PENDANT_SOCKET)
	rt_task_create(&pendantsocketsend_task, "pendant", 0, 80, 0); //predefined stack size will be used for this task
	rt_task_start(&pendantsocketsend_task, &pendantsocketsend_run, NULL);
	
#else	// !(_USE_PENDANT_SOCKET)

#if !defined(_USE_ETHERCAT)
	// Usually, the socket should be sampled faster, e.g. 10 times, than the control.
	UINT socket_period = 2e5; 	//sampling period for control socket (unit in nano-second)

	// controlsocket_task: create and start
	rt_task_create(&controlsocket_task, "socket", 0, 99, 0); //predefined stack size will be used for this task
	rt_task_start(&controlsocket_task, &controlsocket_run, &socket_period);
#endif // _USE_ETHERCAT

#endif	// _USE_PENDANT_SOCKET

	while (working)
	{
		usleep(10);
	}

	signal_handler(0);

	return 0;
}
#endif


