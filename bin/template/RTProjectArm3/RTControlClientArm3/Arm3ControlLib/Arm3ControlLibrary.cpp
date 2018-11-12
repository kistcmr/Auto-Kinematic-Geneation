/*
 * Arm3ControlLilbrary.cpp
 *
 *  Created on: Jul 23, 2016
 *      Author: ThachDo
 */
#if !defined(_USE_SHARED_LIB_)

#include "Arm3ControlLibrary.h"

Arm3ControlLibrary::Arm3ControlLibrary()
: _robot()
,_home()
, _task()
, _tkin(_robot)
, _taskRel()
, _tkinRel(_robot)
{
	// TODO Auto-generated constructor stub
	/// TO DO: choose absolute or relative transformation task
	_tkin.set(_robot.TARGET_BODY, _robot.Ttarget(), _robot.Tref());
	_tkinRel.set(_robot.TARGET_BODY, _robot.Ttarget(), _robot.REF_BODY, _robot.Tref());
	//_tkin.set(Arm3::TARGET_BODY, _robot.Ttarget(), Arm3::REF_BODY, _robot.Tref());

	/// FIXME @20160312
	NRMKFoundation::internal::IndexSet<TaskKinematics::DIM> pindex;
	pindex.addIndex(3);
	pindex.addIndex(4);
	pindex.addIndex(5);

	_ikin.setPriorityIndex(pindex);

	// Added @20140611
#if defined(REST_TIME_WAYPOINT)
	_waypoint.setRestTime(REST_TIME_WAYPOINT);
#endif

#if defined(CRUISE_TIME_WAYPOINT)
	_waypoint.setCruiseTime(CRUISE_TIME_WAYPOINT);
#endif
}

Arm3ControlLibrary::~Arm3ControlLibrary()
{
	// TODO Auto-generated destructor stub
}

void Arm3ControlLibrary::init()
{
#if !defined(KIN_CONTROL)
	_home.setPosGain(100);
	_home.setVelGain(20);

#if defined(_PASSIVITY_BASED_CONTROL)
	_home.setInvL2sqr(0.0);
#endif

	TaskVec kp;
	TaskVec kv;

	TaskVecRel kpRel;
	TaskVecRel kvRel;

#if !defined(_IMPEDANCE_CONTROL)
	/// TO DO: Set gains for transform task controller
	kp << 100, 100, 100, 100, 100, 100;
	kv << 20, 20, 20, 20, 20, 20;

	kpRel << 100, 100, 100, 100, 100, 100;
	kvRel << 20, 20, 20, 20, 20, 20;

#else	// _IMPEDANCE_CONTROL
	NRMKFoundation::Impedance<JOINT_DOF> task_impedance;
	JointVec mass;
	JointVec damping;
	JointVec stiffness;

	mass << 1, 1, 1, 1, 1, 1;
	damping << 20, 20, 20, 20, 20, 20;
	stiffness << 100, 100, 100, 0, 0, 0;

	task_impedance.setMass(mass);
	task_impedance.setDamping(damping);
	task_impedance.setStiffness(stiffness);

	TaskVec kf;
	task_impedance.generateGain(kp, kv, kf);

	_task.setForceGain(kf);
#endif	// _IMPEDANCE_CONTROL
	_task.setPosGain(kp);
	_task.setVelGain(kv);

	_taskRel.setPosGain(kpRel);
	_taskRel.setVelGain(kvRel);

#if defined(_PASSIVITY_BASED_CONTROL)
	_task.setInvL2sqr(0.0);
#endif
#else
	_home.setPosGain(1/_delT);	// This is not used.

	TaskVec kp;

	/// TO DO: Set gains for extended task controller
	kp.setConstant(1/_delT);

	_task.setPosGain(kp);
#endif

	// set gains for the subtask controllers
	_task.setSubGains();
	_taskRel.setSubGains();
}

void Arm3ControlLibrary::setPeriod(double dT)
{
	_delT = dT;
	_home.setPeriod(dT);
	_task.setPeriod(dT);
	_taskRel.setPeriod(dT);
}

void Arm3ControlLibrary::reset()
{
	_robot.tau().setZero();
}

void Arm3ControlLibrary::controlKeepState(double tCur, JointVec & qCur)
{
	JointVec qCurVec;
	JointVec  qdotref;
	JointVec  qddotref;

	qCurVec = qCur;

#if defined(KIN_CONTROL)
		_robot.tau() = _robot.q();
#else
		_home.refAcceleration(_robot.q(), _robot.qdot(), qCurVec, JointVec::Zero(), JointVec::Zero(), qdotref, qddotref);

		/// FIXME @20160311  (Control with kinematic joint)
		if (_robot.kinematicJointSize() > 0)
		{
			_robot.fillInKinematicJointVector(qddotref, _robot.qddot());

#if defined(_PASSIVITY_BASED_CONTROL)
			_robot.fillInKinematicJointVector(qdotref, _robot.qdot());
#endif
		}

#if defined(_PASSIVITY_BASED_CONTROL)
		tauref = _home.refForce();
#endif

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

void Arm3ControlLibrary::controlMoveJoint(double tCur)
{
	JointVec  qd;
	JointVec  qdotd;
	JointVec  qddotd;
	JointVec  qdotref;
	JointVec  qddotref;

	_jinterpolator.traj(tCur, qd, qdotd, qddotd);

#if defined(KIN_CONTROL)
	//_home.refVelocity(_robot.q(), qd, qdotd, qdotref);

	/// FIXME @20160311  (Control with kinematic joint)
	if (_robot.kinematicJointSize() > 0)
	{
		_robot.fillInKinematicJointVector(qd, _robot.q());
	}

	_robot.tau() = qd;
#else
	_home.refAcceleration(_robot.q(), _robot.qdot(), qd, qdotd, qddotd, qdotref, qddotref);

	/// FIXME @20160311  (Control with kinematic joint)
	if (_robot.kinematicJointSize() > 0)
	{
		_robot.fillInKinematicJointVector(qddotref, _robot.qddot());

#if defined(_PASSIVITY_BASED_CONTROL)
		_robot.fillInKinematicJointVector(qdotref, _robot.qdot());
#endif
	}

#if defined(_PASSIVITY_BASED_CONTROL)
	tauref = _home.refForce();
#endif

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

void Arm3ControlLibrary::controlMoveTask(double tCur)
{
	// task interpolator
	TaskPosition posDes;
	TaskVelocity vel_des;
	TaskAcceleration acc_des;

	TaskPosition posDesRel;
	TaskVelocity vel_des_rel;
	TaskAcceleration acc_des_rel;

	JointVec  qdotref;
	JointVec  qddotref;

	switch(_robot.REF_BODY)
	{

		case -1:
				_tinterpolator.traj(tCur, posDes, vel_des, acc_des);
				break;

		default:
				_tinterpolator.traj(tCur, posDesRel, vel_des_rel, acc_des_rel);
				break;
	}

#if defined(KIN_CONTROL)
	_ikin.solvePos(_robot, _tkin, _task, posDes);

	// In case that analytic IK is available
	//LieGroup::HTransform Td(_pos_des.R(), _pos_des.r());
	//_robot.ikin(Td);

	_robot.tau() = _robot.q();
// 			//std::cout << num_ik_steps << std::endl;
#else

	// task kinematics
	TaskPosition pos;
	TaskVelocity vel;
	TaskJacobian J;
	TaskJacobian Jdot;

	TaskPosition posRel;
	TaskVelocity velRel;
	TaskJacobianRel JRel;
	TaskJacobianRel JdotRel;

	switch(_robot.REF_BODY)
	{

		case -1:	_tkin.jacobian(_robot, pos, vel, J, Jdot);
					break;

		default:	_tkinRel.jacobian(_robot, posRel, velRel, JRel, JdotRel);
					break;
	}

	// task controller
	TaskVec vel_ref;
	TaskVec acc_ref;

	TaskVecRel vel_ref_rel;
	TaskVecRel acc_ref_rel;

#if !defined(_IMPEDANCE_CONTROL)
	switch(_robot.REF_BODY)
	{

		case -1:	_task.refAcceleration(pos, vel, posDes, vel_des, acc_des, vel_ref, acc_ref);
					break;

		default:	_taskRel.refAcceleration(posRel, velRel, posDesRel, vel_des_rel, acc_des_rel, vel_ref_rel, acc_ref_rel);
					break;
	}

#if defined(_PASSIVITY_BASED_CONTROL)
	tauref.noalias() = J.transpose()*_task.refForce();
#endif
#else
	TaskForce force;
	LieGroup::Wrench Fext = _robot.Ttarget().itransform(_Fext);

	force.n() = Fext.n();
	force.f() = pos.R()*Fext.f();

	TaskForce force_des;
	force_des.n() = _Fdes.n();
	force_des.f() = _Fdes.f();

	_task.refImpedanceAcceleration(pos, vel, _pos_des, vel_des, acc_des, force, force_des, vel_ref, acc_ref);
	tauref.noalias() = J.transpose()*_task.refForce();
#endif

	switch(_robot.REF_BODY)
	{
		case -1:

			// joint acceleration
			/// FIXME @20160311 (Control with kinematic joint)
#if defined(_PASSIVITY_BASED_CONTROL)
			_ikin.solveAcc(_robot, J, Jdot, vel_ref, acc_ref, qdotref, qddotref);
#else
			_ikin.solveAcc(_robot, J, Jdot, acc_ref, qddotref);
#endif	// _PASSIVITY_BASED_CONTROL
			break;

		default:

			// joint acceleration
			/// FIXME @20160311 (Control with kinematic joint)
#if defined(_PASSIVITY_BASED_CONTROL)
			_ikin.solveAcc(_robot, J, Jdot, vel_ref, acc_ref_rel, qdotref, qddotref);
#else
			_ikin.solveAcc(_robot, J, Jdot, acc_ref_rel, qddotref);
#endif	// _PASSIVITY_BASED_CONTROL
			break;
	}

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

void Arm3ControlLibrary::controlGravity(double tCur)
{
#if !defined(KIN_CONTROL)
		_robot.idyn_gravity(LieGroup::Vector3D(0, 0, -GRAV_ACC));
		//_robot.tau() -= 0.01*_robot.qdot();
#endif
}

void Arm3ControlLibrary::setAbsJointTarget(double tCur, JointVec const & qDes, JointVec const & qdotDes, JointVec const & qddotDes, double period)
{
	JointVec qDesVec = qDes;
	JointVec qdotDesVec = qdotDes;
	JointVec qddotDesVec = qddotDesVec;

	if (period <= 0)
		_jinterpolator.setTraj(qDesVec, qdotDesVec, qddotDesVec);
	else
	{
		_jinterpolator.setInitialTraj(tCur, _robot.q(), JointVec::Zero(), JointVec::Zero());
		_jinterpolator.setTargetTraj(tCur + period, qDesVec, qdotDesVec, qddotDesVec);
	}
}

void Arm3ControlLibrary::setRelJointTarget(double tCur, JointVec const & qDes, JointVec const & qdotDes, JointVec const & qddotDes, double period)
{
	JointVec qDesVec;
	JointVec qdotDesVec;
	JointVec qddotDesVec;

	qDesVec = qDes;
	qdotDesVec = qdotDes;
	qddotDesVec = qddotDes;

	if (period <= 0)
		_jinterpolator.setTraj(qDesVec, qdotDesVec, qddotDesVec);
	else
	{
		JointVec qd = _robot.q();
		qd = qd + qDesVec;
		_jinterpolator.setInitialTraj(tCur, _robot.q(), JointVec::Zero(), JointVec::Zero());
		_jinterpolator.setTargetTraj(tCur + period, qd, qdotDesVec, qddotDesVec);
	}
}

void Arm3ControlLibrary::setAbsTaskTarget(double tCur, TaskPosition const & pDes, TaskVelocity const & vDes, TaskAcceleration const & aDes, double period)
{
	TaskPosition pCur, pD;
	TaskVelocity vCur;
	forwardKinematics(pCur, vCur);

	if (period <= 0)
		_tinterpolator.setTraj(pDes, vDes, aDes);
	else
	{
		_tinterpolator.setInitialTraj(tCur, pCur, vCur);
		_tinterpolator.setTargetTraj(tCur + period, pDes, vDes, aDes);//, TaskInterpolator::VelType(), TaskInterpolator::AccType());
	}
}

void Arm3ControlLibrary::setRelTaskTarget(double tCur, TaskPosition const & pDes, TaskVelocity const & vDes, TaskAcceleration const & aDes, double period)
{
	TaskPosition pCur, pD;
	TaskVelocity vCur;
	forwardKinematics(pCur, vCur);

	pD = pCur;

	for (int i = 0; i < 3; i++)
	{
		// Displacement
		pD.r()[i] += pDes.r()[i];
	}

	// Rotation
	pD.R() = pDes.R()*pD.R();

	if (period <= 0)
		_tinterpolator.setTraj(pD, vDes, aDes);
	else
	{
		_tinterpolator.setInitialTraj(tCur, pCur, vCur);
		_tinterpolator.setTargetTraj(tCur + period, pD, vDes, aDes);//, TaskInterpolator::VelType(), TaskInterpolator::AccType());
	}
}

void Arm3ControlLibrary::importWaypoint(std::string const & fileName)
{
	_waypoint.read(_robot, _tkin, fileName);
}

void Arm3ControlLibrary::setWaypointRestTime(double restTime)
{
	_waypoint.setRestTime(restTime);
}

void Arm3ControlLibrary::setWaypointCruiseTime(double cruiseTime)
{
	_waypoint.setCruiseTime(cruiseTime);
}

void Arm3ControlLibrary::insertWaypoint()
{
	_robot.update(true);
	_waypoint.insert(_robot, _tkin);
}

void Arm3ControlLibrary::resetWaypoint()
{
	_waypoint.reset();
}

void Arm3ControlLibrary::deleteLastWaypoint()
{
	_waypoint.remove();
}

void Arm3ControlLibrary::executeWaypoint(double tCur)
{
	TaskPosition tPos;
	TaskVelocity tVel;

	if (_waypointTaskMode)
	{
		forwardKinematics(tPos, tVel);
		_tinterpolator.setInitialTraj(tCur, tPos);

		_waypoint.executeTaskWaypoints(_tinterpolator, tCur);
	}
	else
	{
		_jinterpolator.setInitialTraj(tCur, _robot.q());

		_waypoint.executeJointWaypoints(_jinterpolator, tCur);
	}
}

double Arm3ControlLibrary::getWaypointDuration()
{
	return _waypoint.totalDuration();
}

void Arm3ControlLibrary::finishWaypoint(double tCur)
{
	_waypoint.finish(_robot, _jinterpolator, tCur);
}
void Arm3ControlLibrary::forwardKinematics(TaskPosition & tPos, TaskVelocity & tVel)
{
	TaskPosition	pos;
	TaskVelocity	vel;

#if defined(KIN_CONTROL)
	_robot.update(true);
#else
	_robot.update();
#endif

	if (_robot.REF_BODY == -1)
		_tkin.kinematics(_robot, pos, vel);
	else
		_tkinRel.kinematics(_robot, pos, vel);

	tPos = pos;
	tVel = vel;
}

POCO_BEGIN_MANIFEST(AbstractControlLibrary<Arm3>)
POCO_EXPORT_CLASS(Arm3ControlLibrary)
POCO_END_MANIFEST

#endif // _USE_SHARED_LIB_
