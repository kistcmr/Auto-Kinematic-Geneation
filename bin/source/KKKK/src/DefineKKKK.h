/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "LieGroup/LieGroup.h"

#include "AMBS/Subsys.h"

#include "DefineConstant.h"

// For automatic modified DH modeling
#include "MDHModeling.h"

typedef NRMKFoundation::SubsysBase<NRMK_NUMBER_OF_BODIES, NRMK_NUMBER_OF_BODIES - 1, NRMK_NUMBER_OF_BODIES - 1, NRMKFoundation::RigidJoint> Subsys_KKKK;

class KKKK : public Subsys_KKKK
{
public:
	/// TO DO: Define the target body and reference body index.
	// The generated code is for absolute transform task
	/*
	enum
	{
		REF_BODY = -1,
		TARGET_BODY = NRMK_TARGET_INDEX,
	};
	*/
	int REF_BODY;
	int TARGET_BODY;

public:
	inline KKKK() : Subsys_KKKK()
		, _qhome(JointVec::Zero())
		, _Ttarget(LieGroup::Rotation(NRMK_TARGET_ROTATION), LieGroup::Displacement(NRMK_TARGET_POS))
		, _Tref(LieGroup::HTransform(LieGroup::Rotation(NRMK_REF_ROTATION), LieGroup::Displacement(NRMK_REF_POS)))
		, REF_BODY(NRMK_REF_INDEX)
		, TARGET_BODY(NRMK_TARGET_INDEX)
		
	{
		using namespace LieGroup;
		HTransform Offset[NRMK_NUMBER_OF_BODIES - 1];

		NRMK_ADD_OFFSET
		
		NRMK_SET_JOINT

		NRMK_ROBOT_DAMPING

#if !defined(_CONTROLLER)
		

#endif

		for (int k = 0; k < NUM_BODIES; k++)
			addBody(k, _body[k]);

		connect(-1, 0, _joint0);	
		
		NRMK_CONNECT

		update();

		NRMKFoundation::Inertia inertia[NUM_BODIES];
		NRMKFoundation::Inertia inertiaTemp;
		HTransform modelingFrame;
		
		NRMK_INERTIA
		
		for (int k = 0; k < NUM_BODIES; k++)
			_body[k].addInertia(inertia[k]);
		
		_defineJointHome();
		_defineSettingData();
		
		/// ADDED @20160312
		_setKinematicJoints();
	}

	inline void setInitialConfiguration()
	{
	}

	inline void cleanup()
	{
	}

	JointVec const & qhome() const { return _qhome; }

	LieGroup::HTransform const & Ttarget() const { return _Ttarget; }
	LieGroup::HTransform const & Tref() const { return _Tref; }
	LieGroup::HTransform const & Ttask_init() const { return _Ttask_init; }

	void SetTtarget(LieGroup::Rotation const & R, LieGroup::Displacement const & r){_Ttarget.set(R, r);};
	void SetTReference(LieGroup::Rotation const & R, LieGroup::Displacement const & r){_Tref.set(R, r);};

	float GetHomePos(int axisNum){return (float)_qhome[axisNum];};
	float GetPulsePerRev(int axisNum){return (float)_pulsePerRev[axisNum];};
	float GetQDirection(int axisNum){return (float)_qDirection[axisNum];};
	float GetTDirection(int axisNum){return (float)_tDirection[axisNum];};
	float GetRatedTorque(int axisNum){return (float)_ratedTorque[axisNum];};
	float GetGearRatio(){return _gearRatio;};

	int ikin(LieGroup::HTransform const & T)
	{
		int num_roots = 0;
		return num_roots;
	}

private:
	void _defineJointHome() 
	{
		//way.1
		_qhome.setConstant(90*DEGREE);
		//way.2
		//_qhome << 0, 0, 60, -30, 30, 60, -30, 30, -30, -60, 30, -30, -60, 30, -30, 30;
		//_qhome *= DEGREE;
	}

	void _setKinematicJoints()
	{
		
		NRMK_KINEMATIC_JOINT
	}

	void _defineSettingData()
	{
		/* User have to input values directly
		_pulsePerRev	<< 524288, 524288, 524288, 262144, 262144, 262144;
		_qDirection		<< -1, 1, -1, 1, -1, -1;
		_tDirection		<< -1, 1, -1, 1, -1, -1;
		_ratedTorque	<< 1.273, 1.273, 1.273, 0.318, 0.318, 0.318;
		_gearRatio		= 100;
		*/
	}

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	EarthJoint	_joint0;

	NRMK_INIT_JOINT_VAR

	NRMKFoundation::Body _body[NUM_BODIES];

	JointVec _qhome;

	JointVec _pulsePerRev;
	JointVec _qDirection;
	JointVec _tDirection;
	JointVec _ratedTorque;
	
	float _gearRatio;

	LieGroup::HTransform _Ttarget;
	LieGroup::HTransform _Tref;

	LieGroup::HTransform _Ttask_init;
};