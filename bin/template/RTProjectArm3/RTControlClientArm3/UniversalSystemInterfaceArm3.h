/*
 * UniversalSystemInterfaceArm3.h
 *
 *  Created on: Sept 19, 2015
 *      Author: Thach Do
 */
 
#if defined(_USE_ETHERCAT)

#ifndef UNIVERSALSYSTEMINTERFACE_H_
#define UNIVERSALSYSTEMINTERFACE_H_

#include <rtdk.h>		//The rdtk real-time printing library
#include "ServoAxis.h"

#include "ecrt.h"
#include "EcatSystem/SystemInterface_EtherCAT_Arm3.h"

namespace NRMKHelper {

template <typename SubsysType>
class UniversalSystemInterfaceArm3 {

	enum
	{
		NUM_AXIS = SubsysType::JOINT_DOF
	};

public:
	
	int init(char* sIP, INT8 ModeOp, double CycleInSecond)
	{		
		for (int i = 0; i < NUM_AXIS; i++)
		{
			axis[i].setPulsePerRevolution(1);
			
			axis[i].setZeroPos(0);
			
			axis[i].setDirQ( 1);
			
			axis[i].setDirTau( 1);
			
			axis[i].setMaxTorInCnt(2000);
			
			axis[i].setRatedTau(1);

			axis[i].setGearRatio(1);

			axis[i].setConversionConstants();

			axis[i].setTrajPeriod((double) CycleInSecond);
			
			_ecatSysInf.setServoOn(i, true);
		}

		return _ecatSysInf.init(ModeOp, (UINT32) (CycleInSecond*1e9));
	}
	
	int deinit()
	{
		return _ecatSysInf.deinit();
	}
	
	bool isSystemReady()
	{
		return _ecatSysInf.isSystemReady();
	}
	
	UINT16 getAxisStatus(int AxisIdx)
	{
		return _ecatSysInf.getAxisCoEStatus(AxisIdx);
	}
	
	INT16 getTarTor(int AxisIdx)
	{
		return TargetTor[AxisIdx];
	}
	INT16 getActTor(int AxisIdx)
	{
		return ActualTor[AxisIdx];
	}
	
	INT32 getActPos(int AxisIdx)
	{
		return ActualPos[AxisIdx];
	}
	INT32 getTarPos(int AxisIdx)
	{
		return TargetPos[AxisIdx];
	}
	
	void processReadData()
	{
		_ecatSysInf.processTxDomain();
	}
	void processWriteData()
	{
		_ecatSysInf.processRxDomain();
	}

	void readData(double * const data)
	{
		_ecatSysInf.readBuffer(COE_POSITION, ActualPos);
		_ecatSysInf.readBuffer(COE_VELOCITY, ActualVel);
		_ecatSysInf.readBuffer(COE_TORQUE, ActualTor);

		for (int i=0; i<NUM_AXIS; ++i)
		{
			axis[i].setCurrentPosInCnt(ActualPos[i]);
			axis[i].setCurrentVelInCnt(ActualVel[i]);

			data[i] = (double) axis[i].getCurrPosInRad();
			data[i+NUM_AXIS] = (double) axis[i].getCurrVelInRad();
		}
	}

	void writeData(double const * const data)
	{
		for (int i=0; i<NUM_AXIS; ++i)
		{
			if (_ecatSysInf.getModeOperation(i) == OP_MODE_CYCLIC_SYNC_TORQUE)
			{
				axis[i].setDesTorInNm(data[i]);
				TargetTor[i] = (INT16) axis[i].getDesTorInCnt();
			}
			else if (_ecatSysInf.getModeOperation(i) == OP_MODE_CYCLIC_SYNC_POSITION)
			{
				axis[i].setDesPosInRad(data[i]);
				TargetPos[i] = (INT32) axis[i].getDesPosInCnt();
			}
		}

		_ecatSysInf.writeBuffer(COE_POSITION, TargetPos);
		_ecatSysInf.writeBuffer(COE_TORQUE, TargetTor);
	}
	
private:
	INT32 ActualPos[NUM_AXIS];
	INT32 ActualVel[NUM_AXIS];
	INT16 ActualTor[NUM_AXIS];

	INT32 TargetPos[NUM_AXIS];
	INT16 TargetTor[NUM_AXIS];

	NRMKHelper::ServoAxis axis[NUM_AXIS];
	
	NRMKHelper::SystemInterface_EtherCAT_Arm3 _ecatSysInf;
};

}

#endif /* UNIVERSALSYSTEMINTERFACE_H_ */

#endif // _USE_ETHERCAT