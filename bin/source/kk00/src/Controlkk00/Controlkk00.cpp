/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#include "Controlkk00.h"
#include "Controlkk00Cmd.h"

#include "DefineConstant.h"
#include "vkcode.h"

// initializing a static member
int NRMKHelper::TimerKey::_numTimers = 0;

Controlkk00::Controlkk00() 
: _robot()
, _home()
, _task()
, _tkin(_robot)
, _taskRel()
, _tkinRel(_robot)
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
	/// TO DO: choose absolute or relative transformation task
	_tkin.set(_robot.TARGET_BODY, _robot.Ttarget(), _robot.Tref());
	_tkinRel.set(_robot.TARGET_BODY, _robot.Ttarget(), _robot.REF_BODY, _robot.Tref());
	//_tkin.set(kk00::TARGET_BODY, _robot.Ttarget(), kk00::REF_BODY, _robot.Tref());
	
	// Added (since ver. 1.7.2)
#if defined(WAYPOINT_FILE)
	// The number of waypoints to be read from the file should be smaller than the maximum number of waypoints. 
	// By default, it is 2*(JOINT_DOF + 1).
	_waypoint.read(_robot, _tkin, WAYPOINT_FILE);

#endif

	// Added @20140611
#if defined(REST_TIME_WAYPOINT)
	_waypoint.setRestTime(REST_TIME_WAYPOINT);
#endif

#if defined(CRUISE_TIME_WAYPOINT)
	_waypoint.setCruiseTime(CRUISE_TIME_WAYPOINT);
#endif

	/// FIXME @20160312
	NRMKFoundation::internal::IndexSet<TaskKinematics::DIM> pindex; 
	pindex.addIndex(3);
	pindex.addIndex(4);
	pindex.addIndex(5);

	_ikin.setPriorityIndex(pindex);
}

Controlkk00::~Controlkk00()
{
}

void Controlkk00::_servoOn()
{
	_servo = SERVO_ON;
}

void Controlkk00::_servoHold()
{
	// break joints and ..
	_servo = SERVO_HOLD;
}

void Controlkk00::_servoOff()
{
	_robot.tau().setZero();
	_servo = SERVO_OFF;

	// FIXED @131219
	// In the next servo-on, the initial control mode, 
	// sustaining the current position, is activated.
	_cmode = 0;
	_q0 = _robot.q();
}

void Controlkk00::initController()
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

void Controlkk00::setPeriod(double dT)
{
	_delT = dT;

	_home.setPeriod(_delT);
	_task.setPeriod(_delT);
	_taskRel.setPeriod(_delT);
}

void Controlkk00::update(double t)
{
	if (_servo == SERVO_ON)
	{
		// FIXME@20140930 Now it should be called separately()
		//_estimate();
		_compute(t);
	}

	_t = t;	
}

void Controlkk00::reflectJointState()
{
#if defined(KIN_CONTROL)
	_robot.update(_pos_only);
#else
	_robot.update();
#endif

	if (_cmode == 2)
#if defined(KIN_CONTROL)
		_robot.updateJacobian(_pos_only);
#else
		_robot.updateJacobian();
#endif
}

void Controlkk00::_estimate()
{
	// FIXME@20140930
	_robot.qdot() = (_robot.q() - _q_prev)/_delT;
	_q_prev = _robot.q();
}

void Controlkk00::_compute(double t)
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
#endif	// KIN_CONTROL
		}

		break;

	case 2:
		{
			// task interpolator
			TaskVelocity vel_des;
			TaskAcceleration acc_des;

			TaskVelocity vel_des_rel;
			TaskAcceleration acc_des_rel;

			switch(_robot.REF_BODY){
			
				case -1:
						_tinterpolator.traj(t, _pos_des, vel_des, acc_des);
						break;	

				default:
						_tinterpolator.traj(t, _pos_des_rel, vel_des_rel, acc_des_rel);
						break;
			}

#if defined(KIN_CONTROL)
			_ikin.solvePos(_robot, _tkin, _task, _pos_des);

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

			switch(_robot.REF_BODY){
				
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
			switch(_robot.REF_BODY){
			
				case -1:	_task.refAcceleration(pos, vel, _pos_des, vel_des, acc_des, vel_ref, acc_ref);
							break;

				default:	_taskRel.refAcceleration(posRel, velRel, _pos_des_rel, vel_des_rel, acc_des_rel, vel_ref_rel, acc_ref_rel);
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

			switch(_robot.REF_BODY){
				
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
#endif
		}
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
	
void Controlkk00::kbCommand(unsigned char key)
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

int Controlkk00::command(const short& cmd, const int& arg)
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

		axes = 0;
		
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
			axes = 0;
		}

		break;

	// Added@20140612
	case RESERVED_CMD_EMERGENCY_STOP:
		_q0 = _robot.q();
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
#if defined(KIN_CONTROL)
			_robot.update(_pos_only);
#else
			_robot.update();
#endif

			switch(_robot.REF_BODY){

				case -1:	
							{
								TaskVelocity	vel;
								_tkin.kinematics(_robot, _pos_target, vel);
								_tinterpolator.setTraj(_pos_target, vel);
								_pos_des = _pos_target;	
							}
							break;

				default:	
							{
								TaskVelocity velRel;
								_tkinRel.kinematics(_robot, _pos_target_rel, velRel);
								_tinterpolator.setTraj(_pos_target_rel, velRel);
								_pos_des_rel = _pos_target_rel;	
							}
							break;		
			}
		}

		// changes to _cmode = 2
		_cmode = 2;
		joints = 0;
		
		break;

	case USER_CMD_TASK_CTRL:
		{
			if (_cmode != 2)
				break;
	
#if defined(KIN_CONTROL)
			_robot.update(_pos_only);
#else
			_robot.update();
#endif

			switch(_robot.REF_BODY){

				case -1:
					{
						TaskVelocity vel;
						_tkin.kinematics(_robot, _pos_target, vel);

						_tinterpolator.setInitialTraj(_t, _pos_target, vel);
						_pos_des = _pos_target;

						if (arg == VK_T)
						{
							_pos_target.r() = _robot.Ttask_init().r();
							_pos_target.R() = _robot.Ttask_init().R(); 
														}
						else
						{
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
										_pos_target.r()[j] += (arg == VK_RIGHT) ? increment_disp : -increment_disp;

									if (arg == VK_UP || arg == VK_DOWN)
										_pos_target.r()[k] += (arg == VK_UP) ? increment_disp : -increment_disp;
									_newTargetPosAvailable = true;
								}

								// orientation
								axis = (0x08 << i);
								if (axes & axis)
								{
									typedef void (LieGroup::Rotation::* FN_ROT)(double) ;
									FN_ROT rot[3] = { &LieGroup::Rotation::postRotateX, &LieGroup::Rotation::postRotateY, &LieGroup::Rotation::postRotateZ };

									if (arg == VK_LEFT || arg == VK_RIGHT)
										(_pos_target.R().*rot[j])((arg == VK_RIGHT) ? increment_rot : -increment_rot);

									if (arg == VK_UP || arg == VK_DOWN)
										(_pos_target.R().*rot[k])((arg == VK_UP) ? increment_rot : -increment_rot);

									_newTargetRotAvailable = true;
								}
							}
						}
			
						/// TO DO: modify the trajectory completion time (which is one by default)
						_tinterpolator.setTargetTraj(_t + 1, _pos_target);//, TaskInterpolator::VelType(), TaskInterpolator::AccType());
					}
					break;

				default:

					{
						TaskVelocity velRel;
						_tkinRel.kinematics(_robot, _pos_target_rel, velRel);

						_tinterpolator.setInitialTraj(_t, _pos_target_rel, velRel);
						_pos_des_rel = _pos_target_rel;

						if (arg == VK_T)
						{
							_pos_target_rel.r() = _robot.Ttask_init().r();
							_pos_target_rel.R() = _robot.Ttask_init().R(); 
						}
						else
						{
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
										_pos_target_rel.r()[j] += (arg == VK_RIGHT) ? increment_disp : -increment_disp;

									if (arg == VK_UP || arg == VK_DOWN)
										_pos_target_rel.r()[k] += (arg == VK_UP) ? increment_disp : -increment_disp;
									_newTargetPosAvailable = true;
								}

								// orientation
								axis = (0x08 << i);
								if (axes & axis)
								{
									typedef void (LieGroup::Rotation::* FN_ROT)(double) ;
									FN_ROT rot[3] = { &LieGroup::Rotation::postRotateX, &LieGroup::Rotation::postRotateY, &LieGroup::Rotation::postRotateZ };

									if (arg == VK_LEFT || arg == VK_RIGHT)
										(_pos_target_rel.R().*rot[j])((arg == VK_RIGHT) ? increment_rot : -increment_rot);

									if (arg == VK_UP || arg == VK_DOWN)
										(_pos_target_rel.R().*rot[k])((arg == VK_UP) ? increment_rot : -increment_rot);

									_newTargetRotAvailable = true;
								}
							}
						}
			
						/// TO DO: modify the trajectory completion time (which is one by default)
						_tinterpolator.setTargetTraj(_t + 1, _pos_target_rel);//, TaskInterpolator::VelType(), TaskInterpolator::AccType());
					}
					break;
			}
		}
		break;

// Teaching waypoints
	case USER_CMD_RESET_WAYPOINT:
		_waypoint.reset();
		break;

	case USER_CMD_SET_WAYPOINT_MODE:
		_mode_waypoint = (++_mode_waypoint) % 2;
		break;

	case USER_CMD_INSERT_WAYPOINT:
		_robot.update(true);
		_waypoint.insert(_robot, _tkin); 

		break;

	case USER_CMD_DELETE_WAYPOINT:
		_waypoint.remove();
		break;

	case USER_CMD_FINISH_WAYPOINT:
		_waypoint.finish(_robot, _jinterpolator, _t);

		_cmode = 1;

		joints = 0;
		axes = 0;
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
		else if (_mode_waypoint == 1)			
		{
			_cmode = 2;

			// ADDED@20140827
			TaskPosition pos;

			_tkin.kinematics(_robot, pos);
			_tinterpolator.setInitialTraj(_t, pos);
			
			_waypoint.executeTaskWaypoints(_tinterpolator, _t); 
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
				_q0 = _robot.q();

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