/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once

#include "../DefinePPP.h"

// For joint controller and interpolator
#include "Controller/PositionController.h"
#include "Interpolator/Interpolator.h"

// For composite transform task deals with rotation and displacement separately. 
// You may use HTransformTask instead.
#include "Framework/CompositeHTransformTask.h"


// For inverse kinematics algorithm
#include "InverseKinematics/InverseKinematics.h"

#include "Framework/ExtendedTask.h"

// Teaching waypoint
#include "Framework/Waypoint.h"

/// TO DO: Define the maximum number of waypoints
#define NUM_WAYPOINTS				(PPP::JOINT_DOF + 1)	// one for initial
#define CRUISE_TIME_WAYPOINT				1.0			// time duration between two waypoints

/// TO DO: If you want to add resting at waypoints, decomment the following.
//#define REST_TIME_WAYPOINT		1.0			// time duration for resting at waypoints

/// TO DO: If you want to initialize waypoints from file, decomment the following.
#ifdef __GNUC__
//#define WAYPOINT_FILE					"_Waypoint_PPP.dat"
#else
//#define WAYPOINT_FILE					"_Waypoint_PPP.dat"
#endif

// Timer key
#include "TimerKey.h"
// Number of predefined timer key
#define NUM_TIMER_KEYS			1

// SERVO STATE
#define SERVO_OFF						0x0000
#define SERVO_ON						0x1000
#define SERVO_HOLD						0x2000


class ControlPPP
{
public:
	enum
	{
		JOINT_DOF = PPP::JOINT_DOF,
	};		

	typedef PPP::JointVec JointVec;

	// Type-definition of task variables
	typedef NRMKFoundation::CompositeHTransformTaskPosition TaskPosition;
	typedef NRMKFoundation::CompositeHTransformTaskVelocity TaskVelocity;
	typedef NRMKFoundation::CompositeHTransformTaskVelocity TaskAcceleration;
	typedef NRMKFoundation::CompositeHTransformTaskForce TaskForce;
	
private:

	// Type-definition of task kinematics, controller, and interpolator
	/// TO DO: In case of relative transform kinematics , use the followings
	typedef NRMKFoundation::CompositeAbsHTransformTaskKinematics<PPP> TaskKinematics;
	typedef NRMKFoundation::CompositeRelHTransformTaskKinematics<PPP> TaskKinematicsRel;
	typedef NRMKFoundation::CompositeHTransformTaskController TaskController;

	// For  single segment interpolator
	// typedef NRMKFoundation::VectorInterpolator<3, NRMKFoundation::internal::_QuinticPolynomialInterpolator> Interpolator3;
	// For multi-segment interpolator
	typedef NRMKFoundation::VectorInterpolator<3, 
			NRMKFoundation::internal::_MultiSegmentInterpolator<NRMKFoundation::POLYNOMIAL_INTERPOLATOR_QUINTIC, NUM_WAYPOINTS> 
		> Interpolator3;

	typedef NRMKFoundation::CompositeHTransformTaskInterpolator<Interpolator3> TaskInterpolator;

	typedef TaskKinematics::TaskJac TaskJacobian;
	typedef TaskKinematics::TaskVec TaskVec;
	
	typedef TaskKinematicsRel::TaskJac TaskJacobianRel;
	typedef TaskKinematicsRel::TaskVec TaskVecRel;

	/// FIXME @20160312
	typedef NRMKFoundation::NumericInverseKinematics<PPP, TaskKinematics> InverseKinematics;
	
public:
	ControlPPP();
	~ControlPPP();

  	PPP const & robot() { return _robot;}
  	
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

	TaskPosition const & desTask() const { return _pos_des; }
	TaskPosition const & targetTask() const { return _pos_target; }
	
	bool newTargetPosAvailable() const { return _newTargetPosAvailable; }
	bool newTargetRotAvaialble() const { return _newTargetRotAvailable; }
	bool needToChangeVisibility() const { return _needToChangeVisibility; }

	bool & newTargetPosAvailable() { return _newTargetPosAvailable; }
	bool & newTargetRotAvailable() { return _newTargetRotAvailable; }
	bool & needToChangeVisibility() { return _needToChangeVisibility; }

	PPP const & robot() const { return _robot; }

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
	PPP	_robot;						// nominal robot model
	
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

	TaskController _task;
	TaskKinematics _tkin;
	
	TaskController _taskRel;
	TaskKinematicsRel _tkinRel;

	TaskInterpolator _tinterpolator;

	InverseKinematics _ikin;
	
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
	NRMKFoundation::Waypoint<PPP, TaskKinematics, NUM_WAYPOINTS>	_waypoint;
	NRMKFoundation::Waypoint<PPP, TaskKinematicsRel, NUM_WAYPOINTS>	_waypointRel;

	int _mode_waypoint;
	bool _executeWaypoint;

	// for timer key
	NRMKHelper::TimerKey _timerKey[NUM_TIMER_KEYS];

	// for communication with NRMKGraphicsSocket
	TaskPosition _pos_des;
	TaskPosition _pos_target;
	
	TaskPosition _pos_des_rel;
	TaskPosition _pos_target_rel;

	bool _newTargetPosAvailable;
	bool _newTargetRotAvailable;
	bool _needToChangeVisibility;
};