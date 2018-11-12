/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once

#include "../DefineNRMK_ROBOT_NAME.h"

// For joint controller and interpolator
#include "Controller/PositionController.h"
#include "Interpolator/Interpolator.h"



// Teaching waypoint
#include "Framework/Waypoint.h"

/// TO DO: Define the maximum number of waypoints
#define NUM_WAYPOINTS				(NRMK_ROBOT_NAME::JOINT_DOF + 1)	// one for initial
#define CRUISE_TIME_WAYPOINT				1.0			// time duration between two waypoints

/// TO DO: If you want to add resting at waypoints, decomment the following.
//#define REST_TIME_WAYPOINT		1.0			// time duration for resting at waypoints

/// TO DO: If you want to initialize waypoints from file, decomment the following.
#ifdef __GNUC__
//#define WAYPOINT_FILE					"_Waypoint_NRMK_ROBOT_NAME.dat"
#else
//#define WAYPOINT_FILE					"_Waypoint_NRMK_ROBOT_NAME.dat"
#endif

// Timer key
#include "TimerKey.h"
// Number of predefined timer key
#define NUM_TIMER_KEYS			1

// SERVO STATE
#define SERVO_OFF						0x0000
#define SERVO_ON						0x1000
#define SERVO_HOLD					0x2000


class ControlNRMK_ROBOT_NAME
{
public:
	enum
	{
		JOINT_DOF = NRMK_ROBOT_NAME::JOINT_DOF,
	};		

	typedef NRMK_ROBOT_NAME::JointVec JointVec;

public:
	ControlNRMK_ROBOT_NAME();
	~ControlNRMK_ROBOT_NAME();

	// control mode
	int cmode() const { return _cmode; }

	// controller initialization
	void initController();
	//void initSystem(JointVec const & q0, JointVec const & qdot0);
	void setPeriod(double dT);

	// controller context update
	//void reflectJointState(JointVec const & q, JointVec const & qdot);
	void reflectJointState();
	void update(double t);

	JointVec const & q() const { return _robot.q(); }
	JointVec & q() { return _robot.q(); }

	JointVec const & qdot() const { return _robot.qdot(); }
	JointVec & qdot() { return _robot.qdot(); }

	JointVec const & tau() const { return _robot.tau(); }
	JointVec & tau() { return _robot.tau(); }

// Added since v1.6.5
#if defined(_IMPEDANCE_CONTROL)
	LieGroup::Wrench const & Fext() const { return _Fext; }
	LieGroup::Wrench & Fext() { return _Fext; }
#endif

	// user command processing
	void kbCommand(unsigned char key);
	int command(const short& cmd, const int& arg = 0);
	
	// Added@ 20140612
	unsigned char getTimerKey() 
	{ 
		unsigned char key = 0;
		for (int k = 0; k < NUM_TIMER_KEYS; k++)
		{
			key = _timerKey[k].key();
			if (key != 0)
				break;
		}

		return key;
	}

	bool newTargetPosAvailable() const { return _newTargetPosAvailable; }
	bool newTargetRotAvaialble() const { return _newTargetRotAvailable; }
	bool needToChangeVisibility() const { return _needToChangeVisibility; }

	bool & newTargetPosAvailable() { return _newTargetPosAvailable; }
	bool & newTargetRotAvailable() { return _newTargetRotAvailable; }
	bool & needToChangeVisibility() { return _needToChangeVisibility; }

	NRMK_ROBOT_NAME const & robot() const { return _robot; }

private:
	void _estimate();
	void _compute(double t);

	void _servoOn();
	void _servoOff();
	void _servoHold();

	// set cmode
	void _setJointKeepMode() { _cmode = 0; }
	void _setJointControlMode()  { _cmode = 1; }
	void _setTaskControlMode()  { _cmode = 2; }

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	NRMK_ROBOT_NAME	_robot;						// nominal robot model
	
	int		_cmode;		// = 0/1/2 for None/Joint control/Task control
									// = 3 for direct teaching (since ver. 1.7.2)

	// Type-definition of joint controller
	typedef NRMKFoundation::PositionController<JOINT_DOF> JointController;
	// For single segment interpolator
	// typedef NRMKFoundation::VectorInterpolator<JOINT_DOF, NRMKFoundation::internal::_QuinticPolynomialInterpolator> JointInterpolator;
	// For multi-segment interpolator
	typedef NRMKFoundation::VectorInterpolator<JOINT_DOF, 
			NRMKFoundation::internal::_MultiSegmentInterpolator<NRMKFoundation::POLYNOMIAL_INTERPOLATOR_QUINTIC, NUM_WAYPOINTS> 
		> JointInterpolator;

	JointController _home;
	JointInterpolator _jinterpolator;

#if defined(_IMPEDANCE_CONTROL)
	LieGroup::Wrench _Fext;
	LieGroup::Wrench _Fdes;
#endif

	// member variables for control framework management
	JointVec _q0;	// initial configuration
	JointVec _q_prev;	// previous configuration

	double	_delT;
	double	_t;

	bool _pos_only;

	int _servo;

	// for teaching waypoints
	
	NRMKFoundation::JWaypoint<NRMK_ROBOT_NAME, NUM_WAYPOINTS> _waypoint;
	int _mode_waypoint;
	bool _executeWaypoint;

	// for timer key
	NRMKHelper::TimerKey _timerKey[NUM_TIMER_KEYS];

	// for communication with NRMKGraphicsSocket
	
	bool _newTargetPosAvailable;
	bool _newTargetRotAvailable;
	bool _needToChangeVisibility;
};