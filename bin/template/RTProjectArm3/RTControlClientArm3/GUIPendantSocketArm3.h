/*
 * GUIPendantSocketArm3.h
 *
 *  Created on: Mar 13, 2015
 *      Author: Thach Do
 */

#ifndef GUIPENDANTSOCKET_H_
#define GUIPENDANTSOCKET_H_

#pragma once

#include "Eigen/Eigen"
#include "Poco/Event.h"
#include "NRMKSocketBase.h"

#include "Arm3ControlLib/DefineArm3.h"

namespace NRMKHelper
{
	class GUIPendantSocketArm3 : public NRMKSocketBase
	{

	public:
		inline GUIPendantSocketArm3(Arm3 const & robot)
			: NRMKSocketBase()
			, _robot(robot)
			, _commandkey(0), _dataReceiveEvent(true), _hasConnection(false)
		{
		}

		inline int sendCurrentData(Eigen::Matrix<double, Arm3::JOINT_DOF, 1> & q)
		{
			if (!IsOpen() || !_hasConnection)
				return -1;

			unsigned char _buf[2 + sizeof(int) + Arm3::JOINT_DOF*sizeof(double) + 2];
			int index = 0;

			memcpy(_buf, NRMK_SOCKET_START_TOKEN, NRMK_SOCKET_TOKEN_SIZE);							//2 bytes, START_token
			index += NRMK_SOCKET_TOKEN_SIZE;

			int _jointNum = Arm3::JOINT_DOF;
			memcpy(_buf + index, &_jointNum, sizeof(int));											// 4 byte, joint num
			index += sizeof(int);

			for (int i=0; i<Arm3::JOINT_DOF; i++)
			{
				memcpy(_buf + index, &q[i], sizeof(double));										// Joint[i]
				index += sizeof(double);
				//rt_printf("%f\n",q[i]);
			}

			memcpy(_buf + index, NRMK_SOCKET_END_TOKEN, NRMK_SOCKET_TOKEN_SIZE);	//2 bytes, END_token
			index += NRMK_SOCKET_TOKEN_SIZE;

			// Send Data
			WriteComm(_buf, index, INFINITE);

			return index;
		}

		inline int update(double time)
		{
			if (!IsOpen() || !_hasConnection)
				return -1;

			unsigned char _buf[2 + sizeof(double) + sizeof(int) + Arm3::JOINT_DOF*sizeof(double) + 2];
			int index = 0;

			memcpy(_buf, NRMK_SOCKET_START_TOKEN, NRMK_SOCKET_TOKEN_SIZE);							//2 bytes, START_token
			index += NRMK_SOCKET_TOKEN_SIZE;

			memcpy(_buf + index, &time, sizeof(double));											// 4 byte, joint num
			index += sizeof(double);

			int _jointNum = Arm3::JOINT_DOF;
			memcpy(_buf + index, &_jointNum, sizeof(int));											// 4 byte, joint num
			index += sizeof(int);

			double num = 0;
			for (int i=0; i<Arm3::JOINT_DOF; i++)
			{
				num = _robot.q()[i]*RADIAN;
				memcpy(_buf + index, &num, sizeof(double));										// Joint[i]
				index += sizeof(double);
			}

			memcpy(_buf + index, NRMK_SOCKET_END_TOKEN, NRMK_SOCKET_TOKEN_SIZE);	//2 bytes, END_token
			index += NRMK_SOCKET_TOKEN_SIZE;

			// Send Data
			WriteComm(_buf, index, INFINITE);

			return index;
		}

		inline void getKey(unsigned char & key)
		{
			if (_hasConnection) {
				//_dataReceiveEvent.wait();
				key = _commandkey;
			}
			_commandkey = 0;
		}

		inline virtual bool hasConnection() const { return _hasConnection; }

		inline virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
		{
			//LockList();

			if (!_hasConnection)
			{
				_hasConnection = true;
				//_commandkey = lpBuffer[0]; //first byte is Key
				_dataReceiveEvent.set();
				//return;
			}

			if (dwCount == 1)
			{
				if (_hasConnection)
				{
					_commandkey = lpBuffer[0]; //first byte is Key
					_dataReceiveEvent.set();
				}
			}

		}

		inline virtual void OnEvent(UINT uEvent, LPVOID lpvData)
		{
			switch( uEvent )
			{
			case EVT_CONSUCCESS:
				std::cout << "One client has connected to the Pendant Server..." << std::endl;
				_commandkey = 0;
				break;

			case EVT_CONDROP:
				std::cout << "Pendant Server Connection abandoned " << std::endl;

				_hasConnection = false;
				_commandkey = 0;

				StopComm();

				if (IsServer())
					waitForConnection(0);

				break;

			default:
				NRMKSocketBase::OnEvent(uEvent, lpvData);
				break;
			}
		}

	private:
		Arm3 const & _robot;
		volatile char _commandkey;
		Poco::Event _dataReceiveEvent;

		//COMMAND
		char _cmdMode;

		volatile bool _hasConnection;
};
} // namespace NRMKHelper


#endif /* GUIPENDANTSOCKET_H_ */
