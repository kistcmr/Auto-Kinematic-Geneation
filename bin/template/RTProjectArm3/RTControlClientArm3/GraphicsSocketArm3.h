/* NRMKFoundation, Copyright 2013-2014 Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */
#pragma once 

#include "Arm3ControlLib/DefineArm3.h"

#if defined(_CONTROLLER) && !defined(_APP_MANAGER)
#include "ControlArm3/ControlArm3.h"
#endif 

#include "NRMKGraphicsSocket.h"

enum 
{
	NUM_GOBJECT_FRAME = 2,
	MAX_PACKET_BYTES = ((NRMKHelper::NRMKGObject::PACKET_HEADER_BYTE + NRMKHelper::NRMKGObjectFrame::MAX_PACKET_BYTE)
						*NUM_GOBJECT_FRAME),
};

typedef NRMKHelper::NRMKGraphicsSocket<Arm3, NUM_GOBJECT_FRAME, MAX_PACKET_BYTES> _GRAPHICS_SOCKET;

class GraphicsSocketArm3 : public _GRAPHICS_SOCKET
{
public:
	Arm3 _robot;
	inline GraphicsSocketArm3(Arm3 & robot) 
		: _GRAPHICS_SOCKET(robot)
#if defined(_INTEGRATED_APP) && defined(_CONTROLLER)
		, _controller(NULL)
#endif
		,_robot(robot)
	{
	}

#if defined(_INTEGRATED_APP) && defined(_CONTROLLER)
	void setController(ControlArm3 * controller)
	{
		if (_controller != controller)
			_controller = controller;
	}
#endif	

private:
	virtual int _initPacketData(unsigned char * const data)
	{
		float size[4] = { 0.1f, 0.01f, // shorter/thicker
			0.12f, 0.005f }; // longer/thinner

		float transform[12];

		//////////////////////////////////////////////////////////////////////////
		/// TO DO: Specify the target body index
		_target.belong(_robot.TARGET_BODY);	
		_target.setId(0);

		_target.setCommand(NRMKHelper::NRMKGObject::CMD_SET_VISIBLE | NRMKHelper::NRMKGObject::CMD_SET_TRACE);

		_target.addCommand(NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIENTATION | NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIGIN);
		_target.makeTransformData(_sys.Ttarget(), transform);
		_target.setData(transform, 12);

		_target.addCommand(NRMKHelper::NRMKGObject::CMD_SET_SIZE);
		_target.setDataAt(size, 2, 12);

		//////////////////////////////////////////////////////////////////////////
		/// TO DO: Specify the reference body index (Use '-1' for ground)
		_ref.belong(_robot.REF_BODY);	
		_ref.setId(0);

		_ref.setCommand(NRMKHelper::NRMKGObject::CMD_SET_VISIBLE);

		_ref.addCommand(NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIENTATION | NRMKHelper::NRMKGObjectFrame::CMD_SET_ORIGIN);
		_ref.makeTransformData(_sys.Tref(), transform);
		_ref.setData(transform, 12);

		_ref.addCommand(NRMKHelper::NRMKGObject::CMD_SET_SIZE);
		_ref.setDataAt(size, 2, 12);

		// create socket data
		int offset = 0;

		offset += _target.makeSocketData(data + offset);
		offset += _ref.makeSocketData(data + offset);

		_numUpdatedGObjects = 2;


		return offset;
	}

#if defined(_CONTROLLER)
	int _initPacketDataController(unsigned char * const data)
	{
		// create socket data
		int offset = 0;

		return offset;
	}
#endif

	virtual int _updatePacketData(unsigned char * const data)
	{
		int offset = 0;
		_numUpdatedGObjects = 0;


		return offset;
	}

private:
#if defined(_INTEGRATED_APP) && defined(_CONTROLLER)
	ControlArm3* _controller;
#endif

	NRMKHelper::NRMKGObjectFrame	_target;
	NRMKHelper::NRMKGObjectFrame	_ref;

};