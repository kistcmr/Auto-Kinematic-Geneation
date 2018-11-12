/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once

#if defined(_USE_SHARED_LIB_)
#include "../Arm3ControlLib/AbstractControlLibrary.h"
#else
#include "../Arm3ControlLib/Arm3ControlLibrary.h"
#endif

#include "Poco/SharedLibrary.h"
#include "Poco/ClassLoader.h"
#include "Poco/Manifest.h"
#include "Poco/Path.h"

#include "../Arm3ControlLib/DefineArm3.h"
typedef Arm3 ROBOT;

// Timer key
#include "TimerKey.h"
// Number of predefined timer key
#define NUM_TIMER_KEYS			1

// SERVO STATE
#define SERVO_OFF						0x0000
#define SERVO_ON						0x1000
#define SERVO_HOLD						0x2000

#define SHARED_LIB_FILE "./Arm3ControlLib"

/// TO DO: If you want to initialize waypoints from file, decomment the following.
#ifdef __GNUC__
//#define WAYPOINT_FILE					"_Waypoint_Arm3.dat"
#else
//#define WAYPOINT_FILE					"_Waypoint_Arm3.dat"
#endif

class ControlArm3
{
public:
	enum
	{
		JOINT_DOF = Arm3::JOINT_DOF,
	};		

	typedef AbstractControlLibrary<Arm3> AbstractControlLib;
	typedef AbstractControlLib::JointVec JointVec;
	typedef AbstractControlLib::TaskPosition TaskPosition;
	typedef AbstractControlLib::TaskVelocity TaskVelocity;
	typedef AbstractControlLib::TaskAcceleration TaskAcceleration;


public:
	ControlArm3();
	~ControlArm3();

	ROBOT & robot() { return _controlLib->robot(); }
  	
  	JointVec const & q() const { return _controlLib->robot().q(); }
	JointVec & q() { return _controlLib->robot().q(); }

	JointVec const & qdot() const { return _controlLib->robot().qdot(); }
	JointVec & qdot() { return _controlLib->robot().qdot(); }

	JointVec const & tau() const { return _controlLib->robot().tau(); }
	JointVec & tau() { return _controlLib->robot().tau(); }
	
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
	
#if 0
	TaskPosition const & desTask() const { return _pos_des; }
	TaskPosition const & targetTask() const { return _pos_target; }
	
	bool newTargetPosAvailable() const { return _newTargetPosAvailable; }
	bool newTargetRotAvaialble() const { return _newTargetRotAvailable; }
	bool needToChangeVisibility() const { return _needToChangeVisibility; }

	bool & newTargetPosAvailable() { return _newTargetPosAvailable; }
	bool & newTargetRotAvailable() { return _newTargetRotAvailable; }
	bool & needToChangeVisibility() { return _needToChangeVisibility; }
#endif

private:
	void _estimate();
	void _compute(double t);

	void _servoOn();
	void _servoOff();
	void _servoHold();

	// set cmode
	void _setJointKeepMode()	{ _cmode = 0; }
	void _setJointControlMode()	{ _cmode = 1; }
	void _setTaskControlMode()	{ _cmode = 2; }

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
#if defined(_USE_SHARED_LIB_)
	Poco::ClassLoader<AbstractControlLib> 	_libLoader;
	AbstractControlLib*						_controlLib;
#endif
	
#if !defined(_USE_SHARED_LIB_)
	Arm3ControlLibrary					_controlLib;
#endif

	int			_cmode;
	double		_delT;
	double		_t;

	bool 		_executeWaypoint;
	int			_servo;

	JointVec	_q0;	// initial configuration
	JointVec	_q_prev;	// previous configuration

	// for timer key
	NRMKHelper::TimerKey _timerKey[NUM_TIMER_KEYS];

#if 0
	// for communication with NRMKGraphicsSocket
	TaskPosition _pos_des;
	TaskPosition _pos_target;
	
	TaskPosition _pos_des_rel;
	TaskPosition _pos_target_rel;

	bool _newTargetPosAvailable;
	bool _newTargetRotAvailable;
	bool _needToChangeVisibility;
#endif
};
