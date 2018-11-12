/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#include "ControlNRMK_ROBOT_NAME.h"
#include "ControlNRMK_ROBOT_NAMECmd.h"

#include "DefineConstant.h"
#include "vkcode.h"

// initializing a static member
int NRMKHelper::TimerKey::_numTimers = 0;

ControlNRMK_ROBOT_NAME::ControlNRMK_ROBOT_NAME() 
: _robot()
, _home()
, _cmode(0)
, _t(0)
, _q0(JointVec::Zero())
, _q_prev(JointVec::Zero())
#if defined(KIN_CONTROL)
, _pos_only(true)
#else
, _pos_only(false)
#endif
, _servo(SERVO_OFF)
, _newTargetPosAvailable(false), _newTargetRotAvailable(false), _needToChangeVisibility(true)
, _mode_waypoint(NRMKFoundation::MODE_JOINT_WAYPOINT)	// 0 : Joint, 1: Task
, _executeWaypoint(false)
{	
	// Added (since ver. 1.7.2)
#if defined(WAYPOINT_FILE)
	// The number of waypoints to be read from the file should be smaller than the maximum number of waypoints. 
	// By default, it is 2*(JOINT_DOF + 1).
#endif

	// Added @20140611
#if defined(REST_TIME_WAYPOINT)
	_waypoint.setRestTime(REST_TIME_WAYPOINT);
#endif

#if defined(CRUISE_TIME_WAYPOINT)
	_waypoint.setCruiseTime(CRUISE_TIME_WAYPOINT);
#endif
}

ControlNRMK_ROBOT_NAME::~ControlNRMK_ROBOT_NAME()
{
}

void ControlNRMK_ROBOT_NAME::_servoOn()
{
	_servo = SERVO_ON;
}

void ControlNRMK_ROBOT_NAME::_servoHold()
{
	// break joints and ..
	_servo = SERVO_HOLD;
}

void ControlNRMK_ROBOT_NAME::_servoOff()
{
	_robot.tau().setZero();
	_servo = SERVO_OFF;

	// FIXED @131219
	// In the next servo-on, the initial control mode, 
	// sustaining the current position, is activated.
	_cmode = 0;
	_q0 = _robot.q();
}

void ControlNRMK_ROBOT_NAME::initController()
{
#if !defined(KIN_CONTROL)
	_home.setPosGain(100);
	_home.setVelGain(20);

#if defined(_PASSIVITY_BASED_CONTROL)
	_home.setInvL2sqr(0.0);
#endif
	
#else
	_home.setPosGain(1/_delT);	// This is not used.

#endif
}

void ControlNRMK_ROBOT_NAME::setPeriod(double dT)
{
	_delT = dT;

	_home.setPeriod(_delT);
}

void ControlNRMK_ROBOT_NAME::update(double t)
{
	if (_servo == SERVO_ON)
	{
		// FIXME@20140930 Now it should be called separately()
		//_estimate();
		_compute(t);
	}

	_t = t;	
}

void ControlNRMK_ROBOT_NAME::reflectJointState()
{
#if defined(KIN_CONTROL)
	_robot.update(_pos_only);
#else
	_robot.update();
#endif
}

void ControlNRMK_ROBOT_NAME::_estimate()
{
	// FIXME@20140930
	_robot.qdot() = (_robot.q() - _q_prev)/_delT;
	_q_prev = _robot.q();
}

void ControlNRMK_ROBOT_NAME::_compute(double t)
{
	JointVec  qdotref;
	JointVec  qddotref;

#if defined(_IMPEDANCE_CONTROL) || defined(_PASSIVITY_BASED_CONTROL)
	JointVec tauref;
#endif

	switch (_cmode)
	{
	case 0:	
#if defined(KIN_CONTROL)
		_robot.tau() = _robot.q();
#else
		_home.refAcceleration(_robot.q(), _robot.qdot(), _q0, JointVec::Zero(), JointVec::Zero(), qdotref, qddotref);

#if defined(_PASSIVITY_BASED_CONTROL)
		tauref = _home.refForce();
#endif
#endif	// KIN_CONTROL
		break;

	case 1: // joint control
		{
			JointVec  qd;
			JointVec  qdotd;
			JointVec  qddotd;

			_jinterpolator.traj(t, qd, qdotd, qddotd);
#if defined(KIN_CONTROL)
			//_home.refVelocity(_robot.q(), qd, qdotd, qdotref);
			_robot.tau() = qd;
#else
			_home.refAcceleration(_robot.q(), _robot.qdot(), qd, qdotd, qddotd, qdotref, qddotref);

#if defined(_PASSIVITY_BASED_CONTROL)
			tauref = _home.refForce();
#endif
#endif	// KIN_CONTROL
		}

		break;

	case 2:
			break;
	}

	// Added for direct teaching (ver. 1.7.2)
	if (_cmode == 3)	// Direct teaching
	{
#if !defined(KIN_CONTROL)
		_robot.idyn_gravity(LieGroup::Vector3D(0, 0, -GRAV_ACC));
		//_robot.tau() -= 0.01*_robot.qdot();
#endif
	}
	else
	{
#if !defined(KIN_CONTROL)
#if defined(_PASSIVITY_BASED_CONTROL)
		_robot.idyn_passive(qdotref, qddotref, LieGroup::Vector3D(0, 0, -GRAV_ACC));
#else	// inverse dynamic control
		_robot.idyn(_robot.qdot(), qddotref, LieGroup::Vector3D(0, 0, -GRAV_ACC));
#endif	// _PASSIVITY_BASED_CONTROL
		// _robot.tau() has the computed torque values
#if defined(_IMPEDANCE_CONTROL) || defined(_PASSIVITY_BASED_CONTROL)
		_robot.tau() += tauref;
#endif	// _IMPEDANCE_CONTROL
#endif	// KIN_CONTROL
	}
}
	
void ControlNRMK_ROBOT_NAME::kbCommand(unsigned char key)
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

int ControlNRMK_ROBOT_NAME::command(const short& cmd, const int& arg)
{
	if (_servo != SERVO_ON)
		return -1;

	static int joints = 0;

	switch (cmd)
	{
	case USER_CMD_SELECT_JOINTS:
		if (arg == VK_0)
		{
			joints = 0;
			_q0 = _robot.q();

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

			_jinterpolator.setTraj(_robot.q(), JointVec::Zero(), JointVec::Zero());

			_cmode = 1;
		}

		
		break;
	
	case RESERVED_CMD_GO_HOME:
		{
			if (arg != VK_I && arg != VK_H)
			{
				if (_cmode != 1)
					break;
			}

			JointVec qhome = _robot.q();	

			_jinterpolator.setInitialTraj(_t, qhome, JointVec::Zero(), JointVec::Zero());

			switch (arg)
			{
			case VK_I:
				qhome.setZero();
				break;

			case VK_H:
				qhome = _robot.qhome(); 
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
			_jinterpolator.setTargetTraj(_t + 1, qhome, JointVec::Zero(), JointVec::Zero());

			_cmode = 1;

		}

		break;

	// Added@20140612
	case RESERVED_CMD_EMERGENCY_STOP:
		_q0 = _robot.q();
		_cmode = 0;
		joints = 0;


		_timerKey[0].deactivate();
		_executeWaypoint = false;

		break;
		
// Teaching waypoints
	case USER_CMD_RESET_WAYPOINT:
		_waypoint.reset();
		break;

	case USER_CMD_SET_WAYPOINT_MODE:
		break;

	case USER_CMD_INSERT_WAYPOINT:

		_waypoint.insert(_robot); 
		break;

	case USER_CMD_DELETE_WAYPOINT:
		_waypoint.remove();
		break;

	case USER_CMD_FINISH_WAYPOINT:
		_waypoint.finish(_robot, _jinterpolator, _t);

		_cmode = 1;

		joints = 0;
		break;

	case USER_CMD_EXECUTE_WAYPOINT:
		// Added @ 20140611
		if (!_executeWaypoint)
		{
			// Added @ 20140827
			_timerKey[0].activate(VK_SPACE, static_cast<unsigned long>(_waypoint.totalDuration()/_delT), NRMKHelper::TimerKey::Forever);

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

		if (_mode_waypoint == 0)	
		{
			_cmode = 1;

			// ADDED@20140827
			_jinterpolator.setInitialTraj(_t, _robot.q());

			_waypoint.executeJointWaypoints(_jinterpolator, _t);
		}

		joints = 0;

		break;

	// ADDED for direct teaching (ver. 1.7.2)
	case USER_CMD_DIRECT_TEACHING:
		{
			static bool directTeaching = false;

			if (directTeaching)
			{
				_cmode = 0;
				_q0 = _robot.q();

				directTeaching = false;
			}
			else
			{
				_cmode = 3;
				directTeaching = true;
			}

			joints = 0;
		}

		break;

	default:
		break;
	}

	return 0;
}