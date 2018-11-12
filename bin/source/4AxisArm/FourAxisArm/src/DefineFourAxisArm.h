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

typedef NRMKFoundation::SubsysBase<7, 7 - 1, 7 - 1, NRMKFoundation::RigidJoint> Subsys_FourAxisArm;

class FourAxisArm : public Subsys_FourAxisArm
{
public:
	/// TO DO: Define the target body and reference body index.
	// The generated code is for absolute transform task
	/*
	enum
	{
		REF_BODY = -1,
		TARGET_BODY = 6,
	};
	*/
	int REF_BODY;
	int TARGET_BODY;

public:
	inline FourAxisArm() : Subsys_FourAxisArm()
		, _qhome(JointVec::Zero())
		, _Ttarget(LieGroup::Rotation(1,0,0,0,1,0,0,0,1), LieGroup::Displacement(0,0,0))
		, _Tref(LieGroup::HTransform(LieGroup::Rotation(1,0,0,0,1,0,0,0,1), LieGroup::Displacement(0,0,0)))
		, REF_BODY(-1)
		, TARGET_BODY(6)
		
	{
		using namespace LieGroup;
		HTransform Offset[7 - 1];

		Offset[0].set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.1000));
Offset[1].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.5000, 0.0000, 0.0000));
Offset[2].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.3000, 0.0000, -0.2500));
Offset[3].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.5000, 0.0000, 0.0000));
Offset[4].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.5000, 0.0000, 0.0000));
Offset[5].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.3000, 0.0000, -0.2500));
		
		Offset[0].set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.1000));
Offset[1].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.5000, 0.0000, 0.0000));
Offset[2].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.3000, 0.0000, -0.2500));
Offset[3].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.5000, 0.0000, 0.0000));
Offset[4].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.5000, 0.0000, 0.0000));
Offset[5].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.3000, 0.0000, -0.2500));

		

#if !defined(_CONTROLLER)
		

#endif

		for (int k = 0; k < NUM_BODIES; k++)
			addBody(k, _body[k]);

		connect(-1, 0, _joint0);	
		
		connect(0, 1, _joint_0);
connect(1, 2, _joint_1);
connect(2, 3, _joint_2);
connect(3, 4, _joint_3);
connect(4, 5, _joint_4);
connect(5, 6, _joint_5);

		update();

		NRMKFoundation::Inertia inertia[NUM_BODIES];
		NRMKFoundation::Inertia inertiaTemp;
		HTransform modelingFrame;
		
		inertia[0].set(6.95, 0, 0, 0.04, 0.02, 0.02, 0.03, 0, 0, 0);
inertiaTemp.set(21.99, 0, 0, 0.25, 0.49, 0.49, 0.06, 0, 0, 0);
modelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));
inertia[1].addInertia(inertiaTemp, modelingFrame);
inertiaTemp.set(24.663, -0.051, 0, 0.223, 0.298, 0.449, 0.216, 0, 0, -0.097);
modelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));
inertia[2].addInertia(inertiaTemp, modelingFrame);
inertiaTemp.set(21.99, 0, 0, 0.25, 0.49, 0.49, 0.06, 0, 0, 0);
modelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));
inertia[3].addInertia(inertiaTemp, modelingFrame);
inertiaTemp.set(21.99, 0, 0, 0.25, 0.49, 0.49, 0.06, 0, 0, 0);
modelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));
inertia[4].addInertia(inertiaTemp, modelingFrame);
inertiaTemp.set(24.663, -0.051, 0, 0.223, 0.298, 0.449, 0.216, 0, 0, -0.097);
modelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));
inertia[5].addInertia(inertiaTemp, modelingFrame);
inertiaTemp.set(2.12, 0, 0, 0.03, 0, 0, 0.01, 0, 0, 0);
modelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));
inertia[6].addInertia(inertiaTemp, modelingFrame);
		
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

	NRMKFoundation::RevoluteJoint _joint_0;
NRMKFoundation::RevoluteJoint _joint_1;
NRMKFoundation::RevoluteJoint _joint_2;
NRMKFoundation::RevoluteJoint _joint_3;
NRMKFoundation::RevoluteJoint _joint_4;
NRMKFoundation::RevoluteJoint _joint_5;

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