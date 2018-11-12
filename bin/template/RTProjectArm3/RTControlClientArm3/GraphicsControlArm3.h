/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#if defined(_GOBJECTS_ENABLED)

#include "ControlArm3/ControlArm3.h"

#include "NRMKGObject.h"

class GraphicsControlArm3
{
public:
	enum 
	{
		NUM_GOBJECT_FRAME = 2,
		MAX_PACKET_BYTES = (sizeof(unsigned int)*2 + (NRMKHelper::NRMKGObject::PACKET_HEADER_BYTE + NRMKHelper::NRMKGObjectFrame::MAX_PACKET_BYTE)*NUM_GOBJECT_FRAME),
	};

public:
	inline GraphicsControlArm3(ControlArm3 * controller) 
		: _controller(controller)
	{
		// TO DO: One need to set proper offset transformation...
		setOffset(_controller->robot().Tref());

		// This guarantees the same setting for graphics objects including bid, gid, etc. 
		initPacketDataController();
	}

	inline void setOffset(LieGroup::HTransform const & Tref) 		
	{
		_Tref = Tref;
	}

	int initPacketDataController()
	{
		// create socket data
		int offset = 0;
		int numUpdatedGObjects = 0;

		if (!_controller)
			return offset;
		
		float size[4] = { 0.3f, 0.01f, // larger
			0.2f, 0.005f }; // smaller

		// for header consisting of numGobjects and num_bytes
		// see _appendHeaderInSending(offset, numUpdatedGObjects);
		offset = sizeof(unsigned int)*2;
		_appendHeaderInSending(offset, numUpdatedGObjects);
		
		// fixed to the ground and its position/orientation is updated only by the commands (from keyboard inputs)
		_target_des.belong(-1);
		_target_des.setId(1);
		_target_des.setOffset(_Tref);

		_target_des.setCommand(NRMKHelper::NRMKGObject::CMD_SET_VISIBLE | NRMKHelper::NRMKGObject::CMD_SET_TRACE);
		_target_des.addCommand(NRMKHelper::NRMKGObject::CMD_SET_SIZE);
		_target_des.setData(size, 2);

		// fixed to the ground and its position/orientation is updated periodically by trajectory interpolation algorithms 
		_target_traj.belong(-1);
		_target_traj.setId(2);
		_target_traj.setOffset(_Tref);

		_target_traj.setCommand(NRMKHelper::NRMKGObject::CMD_SET_VISIBLE | NRMKHelper::NRMKGObject::CMD_SET_TRACE);

		_target_traj.addCommand(NRMKHelper::NRMKGObject::CMD_SET_SIZE);
		_target_traj.setData(size + 2, 2);	// having smaller size

		offset += _target_des.makeSocketData(_data + offset);
		offset += _target_traj.makeSocketData(_data + offset);

		return offset;
	}

	virtual int updatePacketData()
	{
		int offset = 0;
		int numUpdatedGObjects = 0;

		if (!_controller)
			return offset;

		if (_controller->cmode() == 2)
		{
			// for header consisting of numGobjects and num_bytes
			// see _appendHeaderInSending(offset, numUpdatedGObjects);
			offset = sizeof(unsigned int)*2;

			ControlArm3::TaskPosition task_traj;
			ControlArm3::TaskPosition task_des;

			task_traj = _controller->desTask();
			task_des = _controller->targetTask();

			// show _target_traj if it is necessary
			_target_traj.setCommand(NRMKHelper::NRMKGObject::CMD_SET_VISIBLE | NRMKHelper::NRMKGObject::CMD_SET_TRACE);

			// set position/orientation of _target_traj
			_target_traj.addCommand(NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIENTATION | NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIGIN);

			float transform[12];
			_target_traj.makeRotationData(task_traj.R(), transform);
			_target_traj.makePositionData(task_traj.r(), transform + 9);
			_target_traj.setData(transform, 12);

			offset += _target_traj.makeSocketData(_data + offset);
			numUpdatedGObjects++;

	// update _target_des only when new desired value has been set
	// 		if (_newTargetRotAvailable)
	// 		{
	// 			_target_des.setCommand(NRMKHelper::NRMKGObject::CMD_SET_VISIBLE | NRMKHelper::NRMKGObject::CMD_SET_TRACE);
	// 
	// 			_target_des.addCommand(NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIENTATION);
	// 			_target_des.makeRotationData(_task_des.R(), transform);
	// 
	// 			if (_newTargetPosAvailable)
	// 			{
	// 				_target_des.addCommand(NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIGIN);
	// 				_target_des.makePositionData(_task_des.r(), transform + 9);
	// 
	// 				_target_des.setData(transform, 12);
	// 
	// 				_newTargetPosAvailable = false;
	// 			}
	// 			else
	// 			{
	// 				_target_des.setData(transform, 9);
	// 			}
	// 		
	// 			_newTargetRotAvailable = false;
	// 
	// 			offset += _target_des.makeSocketData(data + offset);
	// 			_numUpdatedGObjects++;
	// 		}
	// 		else
	// 		{
	// 			if (_newTargetPosAvailable)
	// 			{
	// 				_target_des.setCommand(NRMKHelper::NRMKGObject::CMD_SET_VISIBLE | NRMKHelper::NRMKGObject::CMD_SET_TRACE);
	// 
	// 				_target_des.addCommand(NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIGIN);
	// 				_target_des.makePositionData(_task_des.r(), transform + 9);
	// 				_target_des.setData(transform + 9, 3);
	// 
	// 				_newTargetPosAvailable = false;
	// 
	// 				offset += _target_des.makeSocketData(data + offset);
	// 				_numUpdatedGObjects++;
	// 			}
	// 		}

			if (_controller->newTargetRotAvailable() || _controller->newTargetPosAvailable())
			{
				_target_des.setCommand(NRMKHelper::NRMKGObject::CMD_SET_VISIBLE | NRMKHelper::NRMKGObject::CMD_SET_TRACE);

				_target_des.addCommand(NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIENTATION );
				_target_des.makeRotationData(task_des.R(), transform);

				_target_des.addCommand(NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIGIN);
				_target_des.makePositionData(task_des.r(), transform + 9);

				_target_des.setData(transform, 12);

				_controller->newTargetPosAvailable() = false;
				_controller->newTargetRotAvailable() = false;

				offset += _target_des.makeSocketData(_data + offset);
				numUpdatedGObjects++;
			}

	// 		if (_needToChangeVisibility)
	// 		{
	// 			// trace is set a priori
	// 			_target_cur.setCommand(NRMKHelper::NRMKGObject::CMD_SET_VISIBLE | NRMKHelper::NRMKGObject::CMD_SET_TRACE);
	// 
	// 			offset += _target_cur.makeSocketData(data + offset);
	// 			_numUpdatedGObjects++;
	// 
	// 			_needToChangeVisibility = false;
	// 		}		

			_appendHeaderInSending(offset, numUpdatedGObjects);
		}
		else
		{
			if (_controller->needToChangeVisibility())
			{
				Sleep(50);

				// for header consisting of numGobjects and num_bytes
				// see _appendHeaderInSending(offset, numUpdatedGObjects);
				offset = sizeof(unsigned int)*2;

				// trace is set a priori
				//			_target_cur.resetCommand();
				_target_traj.resetCommand();
				_target_des.resetCommand();

				//			offset += _target_cur.makeSocketData(data + offset);
				//			_numUpdatedGObjects++;

				offset += _target_des.makeSocketData(_data + offset);
				numUpdatedGObjects++;

				offset += _target_traj.makeSocketData(_data + offset);
				numUpdatedGObjects++;

				_controller->needToChangeVisibility() = false;

				_appendHeaderInSending(offset, numUpdatedGObjects);
			}
		}

		return offset;
	}

	unsigned char const * const data() const { return _data; }

private:
	inline void _appendHeaderInSending(unsigned int const & num_bytes, unsigned int const & num_gbjects)
	{
		memcpy(_data, &num_bytes, sizeof(unsigned int));
		memcpy(_data + sizeof(unsigned int), &num_gbjects, sizeof(unsigned int));
	}

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	ControlArm3 * _controller;

	unsigned char _data[MAX_PACKET_BYTES];
	
	LieGroup::HTransform _Tref;

	NRMKHelper::NRMKGObjectFrame	_target_des;
	NRMKHelper::NRMKGObjectFrame	_target_traj;
};

#endif