/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#include "ControlArm3.h"
#include "ControlArm3Cmd.h"

#include "DefineConstant.h"
#include "vkcode.h"

// initializing a static member
int NRMKHelper::TimerKey::_numTimers = 0;

ControlArm3::ControlArm3() 
: _controlLib()
, _cmode(0)
, _t(0)
, _executeWaypoint(false)
, _servo(SERVO_OFF)
, _q0(JointVec::Zero())
, _q_prev(JointVec::Zero())
#if 0
, _newTargetPosAvailable(false), _newTargetRotAvailable(false), _needToChangeVisibility(true)
#endif
{	
#if defined (_USE_SHARED_LIB_)
	std::string libName(SHARED_LIB_FILE);
	libName += Poco::SharedLibrary::suffix();	// append .dll or .so

	_libLoader.loadLibrary(libName);

	_controlLib = _libLoader.create("Arm3ControlLibrary");
	_libLoader.classFor("Arm3ControlLibrary").autoDelete(_controlLib);
#endif

	// Added (since ver. 1.7.2)
#if defined(WAYPOINT_FILE)
	// The number of waypoints to be read from the file should be smaller than the maximum number of waypoints. 
	_controlLib->importWaypoint(WAYPOINT_FILE);
#endif
}

ControlArm3::~ControlArm3()
{
#if defined(_USE_SHARED_LIB_)
	std::string libname(SHARED_LIB_FILE);
	libname += Poco::SharedLibrary::suffix();	// append .dll or .so

	_libLoader.unloadLibrary(libname);
#endif
}

void ControlArm3::_servoOn()
{
	_servo = SERVO_ON;
}

void ControlArm3::_servoHold()
{
	// break joints and ..
	_servo = SERVO_HOLD;
}

void ControlArm3::_servoOff()
{
	_controlLib->reset();
	_servo = SERVO_OFF;

	// FIXED @131219
	// In the next servo-on, the initial control mode, 
	// sustaining the current position, is activated.
	_cmode = 0;
	_q0 = _controlLib->robot().q();
}

void ControlArm3::initController()
{
	_controlLib->init();
}

void ControlArm3::setPeriod(double dT)
{
	_delT = dT;
	_controlLib->setPeriod(_delT);
}

void ControlArm3::update(double t)
{
	_t = t;	

	if (_servo == SERVO_ON)
	{
		// FIXME@20140930 Now it should be called separately()
		//_estimate();
		_compute(t);
	}
}

void ControlArm3::reflectJointState()
{
#if defined(KIN_CONTROL)
	_controlLib->robot().update(true);
#else
	_controlLib->robot().update();
#endif

	if (_cmode == 2)
#if defined(KIN_CONTROL)
		_controlLib->robot().updateJacobian(true);
#else
		_controlLib->robot().updateJacobian();
#endif
}

void ControlArm3::_estimate()
{
	// FIXME@20140930
	_controlLib->robot().qdot() = (_controlLib->robot().q() - _q_prev)/_delT;
	_q_prev = _controlLib->robot().q();
}

void ControlArm3::_compute(double t)
{
	JointVec  qdotref;
	JointVec  qddotref;

#if defined(_IMPEDANCE_CONTROL) || defined(_PASSIVITY_BASED_CONTROL)
	JointVec tauref;
#endif

	switch (_cmode)
	{
	case 0:	
		_controlLib->controlKeepState(_t, _q0);
		break;

	case 1: // joint control
		_controlLib->controlMoveJoint(_t);
		break;

	case 2: // task control
		_controlLib->controlMoveTask(_t);
		break;

	case 3: // gravity compensation
		_controlLib->controlGravity(_t);
		break;
	}
}
	
void ControlArm3::kbCommand(unsigned char key)
{
	switch (key)
	{
	case VK_TAB:
		if (_servo == SERVO_ON)
			_servoHold();

		else //if (_servo == SERVO_HOLD)
			_servoOn();

		break;

	case VK_S:
		// Added (since ver. 1.8.1)
#if defined(WAYPOINT_FILE)
		_waypoint.save(WAYPOINT_FILE);
#endif

		break;

	case VK_ESCAPE:
		_servoOff();

		break;

	case VK_0:
	case VK_1:
	case VK_2:
	case VK_3:
	case VK_4:
	case VK_5:
	case VK_6:
		command(USER_CMD_SELECT_JOINTS, key);
#if defined(_GOBJECTS_ENABLED_)
		_needToChangeVisibility = true;
#endif
		break;

	case VK_H:
	case VK_I:
#if defined(_GOBJECTS_ENABLED_)
		_needToChangeVisibility = true;
#endif

	case VK_ADD:
	case VK_SUBTRACT:
		command(RESERVED_CMD_GO_HOME, key);
		break;

	// Added @20140612
	case VK_Q:
		command(RESERVED_CMD_EMERGENCY_STOP);
		break;

	case VK_X:
	case VK_Y:
	case VK_Z:
	case VK_U:
	case VK_V:
	case VK_W:
		command(USER_CMD_SELECT_KEEP_AXIS, key);
#if defined(_GOBJECTS_ENABLED_)
		_needToChangeVisibility = true;
#endif
		break;

	case VK_T:
	case VK_UP:
	case VK_DOWN:
	case VK_RIGHT:
	case VK_LEFT:
		command(USER_CMD_TASK_CTRL, key);
		break;

	// teaching waypoints
	case VK_SPACE:
		command(USER_CMD_EXECUTE_WAYPOINT);
		break;

	case VK_INSERT:
		command(USER_CMD_INSERT_WAYPOINT);
		break;

	case VK_DELETE:
		command(USER_CMD_DELETE_WAYPOINT);
		break;

	case VK_O:
		command(USER_CMD_SET_WAYPOINT_MODE);
		break;

	case VK_HOME:
		command(USER_CMD_RESET_WAYPOINT);
		break;

	case VK_END:
		command(USER_CMD_FINISH_WAYPOINT);
		break;

	// ADDED for direct teaching since ver. 1.7.2
	case VK_RETURN:
		command(USER_CMD_DIRECT_TEACHING);
		break;

	default:
		break;
	}
}

int ControlArm3::command(const short& cmd, const int& arg)
{
	if (_servo != SERVO_ON)
		return -1;

	static int joints = 0;
	static int axes = 0;

	switch (cmd)
	{
	case USER_CMD_SELECT_JOINTS:
		if (arg == VK_0)
		{
			joints = 0;
			_q0 = _controlLib->robot().q();

			_cmode = 0;
		}
		else
		{
			/// TO DO: If joint has less than nine dof, use this. Otherwise use the followings.
			int joint = (0x01 << (arg - VK_1));
			//int joint = (arg <= VK_9) ? (0x01 << (arg - VK_1)) : (0x01 << (arg - VK_A + 9));

			if (joints & joint)
				joints &= ~joint;
			else
				joints |= joint;

			_controlLib->setAbsJointTarget(_t, _controlLib->robot().q());

			_cmode = 1;
		}

		axes = 0;
		
		break;
	
	case RESERVED_CMD_GO_HOME:
		{
			if (arg != VK_I && arg != VK_H)
			{
				if (_cmode != 1)
					break;
			}

			JointVec qhome = _controlLib->robot().q();

			switch (arg)
			{
			case VK_I:
				qhome.setZero();
				break;

			case VK_H:
				qhome = _controlLib->robot().qhome();
				break;

			case VK_ADD:
			case VK_SUBTRACT:
				{
					static const double increment = 30*DEGREE;

					for (int i = 0; i < JOINT_DOF; i++)
					{
						int joint = (0x01 << i);
						if (joints & joint)
							qhome[i] += (arg == VK_ADD) ? increment : -increment;
					}
				}
				break;
			}			

			/// TO DO: modify the trajectory completion time (which is one by default)
			_controlLib->setAbsJointTarget(_t, qhome, JointVec::Zero(), JointVec::Zero(), 1);

			_cmode = 1;
			axes = 0;
		}

		break;

	// Added@20140612
	case RESERVED_CMD_EMERGENCY_STOP:
		_q0 = _controlLib->robot().q();
		_cmode = 0;
		joints = 0;

		axes = 0;

		_timerKey[0].deactivate();
		_executeWaypoint = false;

		break;
		
	case USER_CMD_SELECT_KEEP_AXIS:
		{
			int axis = 0;
			switch (arg)
			{
			case VK_X: 
			case VK_Y:
			case VK_Z:
				axis = (0x01 << (arg - VK_X));
				break;

			case VK_U:
			case VK_V:
			case VK_W: 
				axis = (0x08 << (arg - VK_U));
				break;
			}

			if (axes & axis)
				axes &= ~axis;
			else
				axes |= axis;

			// Sets the current task as the trajectory
			TaskPosition posTarget = TaskPosition::Zero();
			TaskVelocity velTarget = TaskVelocity::Zero();

			_controlLib->setRelTaskTarget(_t, posTarget, velTarget);
		}

		// changes to _cmode = 2
		_cmode = 2;
		joints = 0;
		
		break;

	case USER_CMD_TASK_CTRL:
		{
			if (_cmode != 2)
				break;
	
			if (arg == VK_T)
			{
				_controlLib->setAbsTaskTarget(_t, _controlLib->robot().Ttask_init());
			}
			else
			{
				TaskPosition posTarget = TaskPosition::Zero();

				static const double increment_disp = 0.1;
				static const double increment_rot = 30*DEGREE;

				for (int i = 0; i < 3; i++)
				{
					int j = (i + 1) % 3;
					int k = (j + 1) % 3;

					// displacement
					int axis = (0x01 << i);
					if (axes & axis)
					{
						if (arg == VK_LEFT || arg == VK_RIGHT)
							posTarget.r()[j] = (arg == VK_RIGHT) ? increment_disp : -increment_disp;

						if (arg == VK_UP || arg == VK_DOWN)
							posTarget.r()[k] = (arg == VK_UP) ? increment_disp : -increment_disp;
					}

					// orientation
					axis = (0x08 << i);
					if (axes & axis)
					{
						typedef void (LieGroup::Rotation::* FN_ROT)(double) ;
						FN_ROT rot[3] = { &LieGroup::Rotation::postRotateX, &LieGroup::Rotation::postRotateY, &LieGroup::Rotation::postRotateZ };

						if (arg == VK_LEFT || arg == VK_RIGHT)
							(posTarget.R().*rot[j])((arg == VK_RIGHT) ? increment_rot : -increment_rot);

						if (arg == VK_UP || arg == VK_DOWN)
							(posTarget.R().*rot[k])((arg == VK_UP) ? increment_rot : -increment_rot);
					}
				}
				// TODO Added Move Abs Target, Move Rel Target
				_controlLib->setRelTaskTarget(_t, posTarget, TaskVelocity::Zero(), TaskAcceleration::Zero(), 1);
			}
		}
		break;
	
	// Teaching waypoints
	case USER_CMD_RESET_WAYPOINT:
		_controlLib->reset();
		break;

	case USER_CMD_SET_WAYPOINT_MODE:
		if (_controlLib->waypointTaskMode() == true)
			_controlLib->waypointTaskMode() = false;
		else
			_controlLib->waypointTaskMode() = true;
		break;

	case USER_CMD_INSERT_WAYPOINT:
		_controlLib->insertWaypoint();
		break;

	case USER_CMD_DELETE_WAYPOINT:
		_controlLib->deleteLastWaypoint();
		break;

	case USER_CMD_FINISH_WAYPOINT:
		_controlLib->finishWaypoint(_t);
		_cmode = 1;

		joints = 0;
		axes = 0;
		break;

	case USER_CMD_EXECUTE_WAYPOINT:
		// Added @ 20140611
		if (!_executeWaypoint)
		{
			// Added @ 20140827
			_timerKey[0].activate(VK_SPACE, static_cast<unsigned long>(_controlLib->getWaypointDuration()/_delT), NRMKHelper::TimerKey::Forever);

			_timerKey[0].begin();
			_executeWaypoint = true;
		}
		else
		{
			if (_timerKey[0].completedExecution())
				_executeWaypoint = false;
		}

		if (!_executeWaypoint)
			break;

		if (_controlLib->waypointTaskMode() == false)
		{
			_cmode = 1;

			// ADDED@20140827
			_controlLib->waypointTaskMode() = false;
			_controlLib->executeWaypoint(_t);
		}
		else
		{
			_cmode = 2;

			// ADDED@20140827
			_controlLib->waypointTaskMode() = true;
			_controlLib->executeWaypoint(_t);
		}

		axes = 0;
		joints = 0;

		break;

	// ADDED for direct teaching (ver. 1.7.2)
	case USER_CMD_DIRECT_TEACHING:
		{
			static bool directTeaching = false;

			if (directTeaching)
			{
				_cmode = 0;
				_q0 = _controlLib->robot().q();

				directTeaching = false;
			}
			else
			{
				_cmode = 3;
				directTeaching = true;
			}
			axes = 0;
			joints = 0;
		}

		break;

	default:
		break;
	}

	return 0;
}