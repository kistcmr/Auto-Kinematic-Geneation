/*
 * AbstractPlugin.h
 *
 *  Created on: Jul 21, 2016
 *      Author: Win8-64-ENG-PC
 */

#ifndef ABSTRACTCONTROLLIBRARY_H_
#define ABSTRACTCONTROLLIBRARY_H_

#include <stdio.h>

// For composite transform task deals with rotation and displacement separately.
// You may use HTransformTask instead.
#include "Framework/CompositeHTransformTask.h"

template <typename SubsysType>
class AbstractControlLibrary
{
public:
	typedef SubsysType ROBOT;
	enum
	{
		JOINT_DOF = ROBOT::JOINT_DOF,
	};

#if 1
	typedef typename ROBOT::JointVec JointVec;
#endif

	// Type-definition of task variables
	typedef NRMKFoundation::CompositeHTransformTaskPosition TaskPosition;
	typedef NRMKFoundation::CompositeHTransformTaskVelocity TaskVelocity;
	typedef NRMKFoundation::CompositeHTransformTaskVelocity TaskAcceleration;
	typedef NRMKFoundation::CompositeHTransformTaskForce TaskForce;

public:
	AbstractControlLibrary(){}
	virtual ~AbstractControlLibrary(){}

	virtual ROBOT & robot() = 0;
	virtual ROBOT const & robot() const = 0;

	virtual void init() = 0;
	virtual void setPeriod(double dT) = 0;

	virtual void reset() = 0;

	virtual void setAbsJointTarget(double tCur, JointVec const & qDes, JointVec const & qdotDes = JointVec::Zero(), JointVec const & qddotDes = JointVec::Zero(), double period = -1) = 0;
	virtual void setRelJointTarget(double tCur, JointVec const & qDes, JointVec const & qdotDes = JointVec::Zero(), JointVec const & qddotDes = JointVec::Zero(), double period = -1) = 0;
	virtual void setAbsTaskTarget(double tCur, TaskPosition const & tDes, TaskVelocity const & vDes = TaskVelocity::Zero(), TaskAcceleration const & aDes = TaskAcceleration::Zero(), double period = -1) = 0;
	virtual void setRelTaskTarget(double tCur, TaskPosition const & tDes, TaskVelocity const & vDes = TaskVelocity::Zero(), TaskAcceleration const & aDes = TaskAcceleration::Zero(), double period = -1) = 0;

#if 1
	virtual void importWaypoint(std::string const & fileName) = 0;
	virtual void setWaypointRestTime(double restTime) = 0;
	virtual void setWaypointCruiseTime(double cruiseTime) = 0;
	virtual void insertWaypoint() = 0;
	virtual void resetWaypoint() = 0;
	virtual void deleteLastWaypoint() = 0;
	virtual void executeWaypoint(double tCur) = 0;
	virtual void finishWaypoint(double tCur) = 0;
	virtual double getWaypointDuration() = 0;
	virtual bool & waypointTaskMode() = 0;
	virtual bool const & waypointTaskMode() const = 0;
#endif

	virtual void controlKeepState(double tCur, JointVec & qCur) = 0;
	virtual void controlMoveJoint(double tCur) = 0;
	virtual void controlMoveTask(double tCur) = 0;
	virtual void controlGravity(double tCur) = 0;

	virtual void forwardKinematics(TaskPosition & tPos, TaskVelocity & tVel) = 0;

#if defined(_IMPEDANCE_CONTROL)
	virtual LieGroup::Wrench const & Fext() const = 0;
	virtual LieGroup::Wrench & Fext() = 0;
#endif
};

#endif /* ABSTRACTCONTROLLIBRARY_H_ */
