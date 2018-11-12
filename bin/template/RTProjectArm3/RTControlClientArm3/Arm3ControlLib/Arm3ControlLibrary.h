/*
 * Arm3ControlLibrary.cpp
 *
 *  Created on: Jul 21, 2016
 *      Author: ThachDo
 */
#ifndef Arm3CONTROLLIBRARY_H_
#define Arm3CONTROLLIBRARY_H_

#if !defined(_USE_SHARED_LIB_)

#include "AbstractControlLibrary.h"
#include "Poco/ClassLibrary.h"

#include "DefineArm3.h"
typedef Arm3 ROBOT;

// For joint controller and interpolator
#include "Controller/PositionController.h"

//#include "MomentumKinematics.h"
//#include "Kinematics.h"

#include "Interpolator/Interpolator.h"

// Extended task consists of primary task and null velocity task.
#include "Framework/ExtendedTask.h"

// For inverse kinematics algorithm
#include "InverseKinematics/InverseKinematics.h"

// Teaching waypoint
#include "Framework/Waypoint.h"

#define NUM_WAYPOINTS				(ROBOT::JOINT_DOF + 1)	// one for initial
/// TO DO: Define the maximum number of waypoints
#define CRUISE_TIME_WAYPOINT				1.0			// time duration between two waypoints

/// TO DO: If you want to add resting at waypoints, decomment the following.
//#define REST_TIME_WAYPOINT		1.0			// time duration for resting at waypoints

class Arm3ControlLibrary : public AbstractControlLibrary<Arm3>
{
public:
	typedef typename AbstractControlLibrary<Arm3>::JointVec JointVec;
	typedef typename AbstractControlLibrary<Arm3>::TaskPosition TaskPosition;
	typedef typename AbstractControlLibrary<Arm3>::TaskVelocity TaskVelocity;
	typedef typename AbstractControlLibrary<Arm3>::TaskAcceleration TaskAcceleration;
	enum
	{
		JOINT_DOF = ROBOT::JOINT_DOF,
	};

	// Type-definition of task kinematics, controller, and interpolator
	/// TO DO: In case of relative transform kinematics , use the followings
	typedef NRMKFoundation::CompositeAbsHTransformTaskKinematics<ROBOT> TaskKinematics;
	typedef NRMKFoundation::CompositeRelHTransformTaskKinematics<ROBOT> TaskKinematicsRel;
	typedef NRMKFoundation::CompositeHTransformTaskController TaskController;

	// For  single segment interpolator
	// typedef NRMKFoundation::VectorInterpolator<3, NRMKFoundation::internal::_QuinticPolynomialInterpolator> Interpolator3;
	// For multi-segment interpolator
	typedef NRMKFoundation::VectorInterpolator<3,
			NRMKFoundation::internal::_MultiSegmentInterpolator<NRMKFoundation::POLYNOMIAL_INTERPOLATOR_QUINTIC, NUM_WAYPOINTS>
		> Interpolator3;

	typedef NRMKFoundation::CompositeHTransformTaskInterpolator<Interpolator3> TaskInterpolator;

	typedef typename TaskKinematics::TaskJac TaskJacobian;
	typedef typename TaskKinematics::TaskVec TaskVec;

	typedef typename TaskKinematicsRel::TaskJac TaskJacobianRel;
	typedef typename TaskKinematicsRel::TaskVec TaskVecRel;

	/// FIXME @20160312
	typedef TaskController InverseTaskController;
	typedef typename AbstractControlLibrary<Arm3>::TaskPosition InverseTaskPosition;
	typedef NRMKFoundation::NumericInverseKinematics<ROBOT, TaskKinematics> InverseKinematics;

	// Type-definition of joint controller
	typedef NRMKFoundation::PositionController<JOINT_DOF> JointController;
	// For single segment interpolator
	// typedef NRMKFoundation::VectorInterpolator<JOINT_DOF, NRMKFoundation::internal::_QuinticPolynomialInterpolator> JointInterpolator;
	// For multi-segment interpolator
	typedef NRMKFoundation::VectorInterpolator<JOINT_DOF,
			NRMKFoundation::internal::_MultiSegmentInterpolator<NRMKFoundation::POLYNOMIAL_INTERPOLATOR_QUINTIC, NUM_WAYPOINTS>
		> JointInterpolator;

	typedef NRMKFoundation::Waypoint<ROBOT, TaskKinematics, NUM_WAYPOINTS>	Waypoint;
	typedef NRMKFoundation::Waypoint<ROBOT, TaskKinematicsRel, NUM_WAYPOINTS> WaypointRel;

public:
	Arm3ControlLibrary();
	~Arm3ControlLibrary();

	ROBOT & robot() { return _robot;}
	ROBOT const & robot() const { return _robot; }

	void init();
	void setPeriod(double dT);

	void reset();
//	void updateRobot();

	void setAbsJointTarget(double tCur, JointVec const & qDes, JointVec const & qdotDes = JointVec::Zero(), JointVec const & qddotDes = JointVec::Zero(), double period = -1);
	void setRelJointTarget(double tCur, JointVec const & qDes, JointVec const & qdotDes = JointVec::Zero(), JointVec const & qddotDes = JointVec::Zero(), double period = -1);

	void setAbsTaskTarget(double tCur, TaskPosition const & tDes, TaskVelocity const & vDes = TaskVelocity::Zero(), TaskAcceleration const & aDes = TaskAcceleration::Zero(), double period = -1);
	void setRelTaskTarget(double tCur, TaskPosition const & tDes, TaskVelocity const & vDes = TaskVelocity::Zero(), TaskAcceleration const & aDes = TaskAcceleration::Zero(), double period = -1);

	void importWaypoint(std::string const & fileName);
	void setWaypointRestTime(double restTime);
	void setWaypointCruiseTime(double cruiseTime);
	void insertWaypoint();
	void resetWaypoint();
	void deleteLastWaypoint();
	void executeWaypoint(double tCur);
	void finishWaypoint(double tCur);
	double getWaypointDuration();
	bool & waypointTaskMode() { return _waypointTaskMode; }
	bool const & waypointTaskMode() const { return _waypointTaskMode; }

	void controlKeepState(double tCur, JointVec & qCur);
	void controlMoveJoint(double tCur);
	void controlMoveTask(double tCur);
	void controlGravity(double tCur);

	void forwardKinematics(TaskPosition & tPos, TaskVelocity & tVel);

	// Added since v1.6.5
#if defined(_IMPEDANCE_CONTROL)
	LieGroup::Wrench const & Fext() const { return _Fext; }
	LieGroup::Wrench & Fext() { return _Fext; }
#endif

private:
	ROBOT  				_robot;
	JointController 	_home;
	JointInterpolator 	_jinterpolator;
	TaskInterpolator 	_tinterpolator;

	InverseKinematics 	_ikin;
	TaskController 		_task;
	TaskKinematics 		_tkin;

	TaskController 		_taskRel;
	TaskKinematicsRel 	_tkinRel;

	// For teaching waypoints
	Waypoint			_waypoint;
	WaypointRel			_waypointRel;

	bool 				_waypointTaskMode;
	double				_delT;

#if defined(_IMPEDANCE_CONTROL)
	LieGroup::Wrench _Fext;
	LieGroup::Wrench _Fdes;
#endif

public:
 EIGEN_MAKE_ALIGNED_OPERATOR_NEW
 
};

#endif // _USE_SHARED_LIB_


#endif /* Arm3CONTROLLIBRARY_H_ */
