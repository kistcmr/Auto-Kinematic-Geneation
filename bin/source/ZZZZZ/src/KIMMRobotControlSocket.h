/*
 * KIMMRobotControlSocket.h
 *
 *  Created on: Mar 13, 2015
 *      Author: Thach Do
 */

#ifndef CONTROLSOCKET_H_
#define CONTROLSOCKET_H_

#pragma once

#include "Eigen/Eigen"
#include "Poco/Event.h"
#include "NRMKSocketBase.h"

namespace NRMKHelper
{
	template<int _NUM_JOINTS>
	class KIMMRobotControlSocket : public NRMKSocketBase
	{
	public:
		enum
		{
			NUM_JOINTS				= _NUM_JOINTS,
			NUM_BYTES_CMDMODE		= 1,					// UINT8
			NUM_BYTES_RESMODE		= 1,					// UINT8
			NUM_BYTES_MOTIONMODE	= 1,					// UINT8
			NUM_BYTES_TRAJTIME		= sizeof(double),		// DOUBLE
			NUM_BYTES_TARTASK		= 9*sizeof(double),		// Pos(X, Y, Z) + Vel(X, Y, Z) + Acc(X, Y, Z)

			NUM_BYTES_POSJOINT		= 12*sizeof(double),	// Pos(DOF)
			NUM_BYTES_VELJOINT		= 12*sizeof(double),	// Vel(DOF)
			NUM_BYTES_ACCJOINT		= 12*sizeof(double),	// Acc(DOF)

			NUM_BYTES_KP			= 10*sizeof(double),		// Kp(10)
			NUM_BYTES_KI			= 10*sizeof(double),		// Ki(10)
			NUM_BYTES_KD			= 10*sizeof(double),		// Kd(10)
			NUM_BYTES_KL			= 10*sizeof(double),		// Kl(10)
		};

		enum CommandMode
		{
			CMD_NONE			= -1,
			CMD_TASK 			= 1,
			CMD_JOINT			= 2,
			CMD_SSTOP 			= 3,
			CMD_ESTOP			= 4,
			CMD_RESET			= 5,
			CMD_SERVOON			= 6,
			CMD_SERVOOFF		= 7,
			CMD_SEND_PID		= 8,
			CMD_GRAV_EN			= 9,
			CMD_GRAV_DIS		= 10,
			CMD_SAMP_JOINT		= 11,
			CMD_GET_PID			= 12,
			CMD_CHANGE_MODE		= 13,
			CMD_LOG_ON			= 14,
			CMD_LOG_OFF			= 15,
			CMD_LOG_SAVE		= 16,
			CMD_TORQUE_ENABLE	= 17,
			CMD_TORQUE_DISABLE  = 18,
			CMD_LIMIT_ENABLE	= 19,
			CMD_LIMIT_DISABLE	= 20
		};
		
		enum RescueMode {
			MODE_RESCUE = 0,
			MODE_OBJECT = 1
		};
		enum DataType {
			MESSAGE 	= 1,
			PID_PARAMS	= 2
		};

	public:
		inline KIMMRobotControlSocket()
			: NRMKSocketBase()
			, _commandkey(0), _dataReceiveEvent(true), _hasConnection(false)
		{
			_tarRight.setZero();
			_tarLeft.setZero();

			_tarPosJoint.setZero();
			_tarVelJoint.setZero();
			_tarAccJoint.setZero();
		}

		// sendKey is used to send key input only
		inline void sendKey(char key)
		{
			//_key = key;
			WriteComm((LPBYTE) &key, 1, INFINITE);
		}

		inline int sendJointData(Eigen::Matrix<double, 12, 1> & Joint)
		{
			if (!IsOpen() || !_hasConnection)
				return -1;

			unsigned char _buf[12*sizeof(double) + 4];
			int index = 0;

			memcpy(_buf, NRMK_SOCKET_START_TOKEN, NRMK_SOCKET_TOKEN_SIZE);								//2 bytes, START_token
			index += NRMK_SOCKET_TOKEN_SIZE;

			for (int i=0; i<12; i++)
			{
				memcpy(_buf + index, &Joint[i], sizeof(double));										// Joint[i]
				index += sizeof(double);
			}

			memcpy(_buf + index, NRMK_SOCKET_END_TOKEN, NRMK_SOCKET_TOKEN_SIZE);	//2 bytes, END_token
			index += NRMK_SOCKET_TOKEN_SIZE;

			// Send Data
			WriteComm(_buf, index, INFINITE);

			return index;
		}

		inline int sendCurrentData(double time, Eigen::Matrix<double, ROBOT::JOINT_DOF, 1> & q)
		{
			if (!IsOpen() || !_hasConnection)
				return -1;

			unsigned char _buf[2 + sizeof(double) + sizeof(int) + ROBOT::JOINT_DOF*sizeof(double) + 2];
			int index = 0;

			memcpy(_buf, NRMK_SOCKET_START_TOKEN, NRMK_SOCKET_TOKEN_SIZE);							//2 bytes, START_token
			index += NRMK_SOCKET_TOKEN_SIZE;

			memcpy(_buf + index, &time, sizeof(double));											// 8 byte, time
			index += sizeof(double);

			int _jointNum = ROBOT::JOINT_DOF;
			memcpy(_buf + index, &_jointNum, sizeof(int));											// 4 byte, joint num
			index += sizeof(int);

			double num = 0;
			for (int i=0; i<ROBOT::JOINT_DOF; i++)
			{
				num = q[i]*RADIAN;
				memcpy(_buf + index, &num, sizeof(double));										// Joint[i]
				index += sizeof(double);
			}

			memcpy(_buf + index, NRMK_SOCKET_END_TOKEN, NRMK_SOCKET_TOKEN_SIZE);	//2 bytes, END_token
			index += NRMK_SOCKET_TOKEN_SIZE;

			// Send Data
			WriteComm(_buf, index, INFINITE);

			return index;
		}

		inline int sendPIDParams(Eigen::Matrix<double, 10, 1> & Kp, Eigen::Matrix<double, 10, 1> & Ki, Eigen::Matrix<double, 10, 1> & Kd, Eigen::Matrix<double, 10, 1> & Kl,
								const double * pNeckMax, const double * pNeckMin, const double * pRightMax, const double * pRightMin, const double * pLeftMax, const double * pLeftMin )
		{
			if (!IsOpen() || !_hasConnection)
				return -1;

			unsigned char _buf[1 + 4*10*sizeof(double) + 4 + sizeof(double) * 12 * 2];
			int index = 0;

			memcpy(_buf, NRMK_SOCKET_START_TOKEN, NRMK_SOCKET_TOKEN_SIZE);			//2 bytes, START_token
			index += NRMK_SOCKET_TOKEN_SIZE;

			unsigned char _dataType = PID_PARAMS;
			memcpy(_buf + index, &_dataType, sizeof(unsigned char));								// 1 byte, Message Type
			index += sizeof(unsigned char);

			for (int i=0; i<10; i++)
			{
				memcpy(_buf + index, &Kp[i], sizeof(double));										// Kp[i]
				index += sizeof(double);
			}

			for (int i=0; i<10; i++)
			{
				memcpy(_buf + index, &Ki[i], sizeof(double));										// Ki[i]
				index += sizeof(double);
			}

			for (int i=0; i<10; i++)
			{
				memcpy(_buf + index, &Kd[i], sizeof(double));										// Kd[i]
				index += sizeof(double);
			}

			for (int i=0; i<10; i++)
			{
				memcpy(_buf + index, &Kl[i], sizeof(double));										// Kl[i]
				index += sizeof(double);
			}

			memcpy(_buf + index, pNeckMax, sizeof(double) * 2);
			index += (sizeof(double) * 2);
			memcpy(_buf + index, pRightMax, sizeof(double) * 5);
			index += (sizeof(double) * 5);
			memcpy(_buf + index, pLeftMax, sizeof(double) * 5);
			index += (sizeof(double) * 5);

			memcpy(_buf + index, pNeckMin, sizeof(double) * 2);
			index += (sizeof(double) * 2);
			memcpy(_buf + index, pRightMin, sizeof(double) * 5);
			index += (sizeof(double) * 5);
			memcpy(_buf + index, pLeftMin, sizeof(double) * 5);
			index += (sizeof(double) * 5);

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

		inline char getCommandMode()
		{
			char cmd = _cmdMode;

			_cmdMode = CMD_NONE;
//			if (_cmdMode == CMD_SSTOP)
//				_cmdMode = CMD_NONE;
			return cmd;
		}
		inline void getTaskCommand(char & RescueMode, char & MotionMode, double & TrajTime, Eigen::Matrix<double, 9, 1> & Right, Eigen::Matrix<double, 9, 1> & Left, Eigen::Matrix<double, 9, 1> & Rel)
		{
			RescueMode = _rescueMode;
			MotionMode = _motionMode;
			TrajTime = _trajTime;
			Right << _tarRight;
			Left << _tarLeft;
			Rel << _tarRel;
			_cmdMode = CMD_NONE;
		}
		inline void getModeCommand(char & RescueMode)
		{
			RescueMode = _rescueMode;
			_cmdMode = CMD_NONE;
		}
		inline void getJointCommand(double & TrajTime, Eigen::Matrix<double, 12, 1> & TarPos, Eigen::Matrix<double, 12, 1> & TarVel, Eigen::Matrix<double, 12, 1> & TarAcc)
		{
			TrajTime = _trajTime;
			TarPos << _tarPosJoint;
			TarVel << _tarVelJoint;
			TarAcc << _tarAccJoint;
			_cmdMode = CMD_NONE;
		}
		inline void getPIDSetting(Eigen::Matrix<double, 10, 1> & Kp, Eigen::Matrix<double, 10, 1> & Ki, Eigen::Matrix<double, 10, 1> & Kd, Eigen::Matrix<double, 10, 1> & Kl)
		{
			Kp = _kp;
			Ki = _ki;
			Kd = _kd;
			Kl = _kl;
			_cmdMode = CMD_NONE;
		}

		inline virtual bool hasConnection() const { return _hasConnection; }
		inline void setConnected()
		{
			_hasConnection = true;
			// to initiate the connection
			sendKey('O');
		}

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
			else if (dwCount >= 3)
			{
				if (_hasConnection)
				{
					if ((lpBuffer[0] != 'N') || (lpBuffer[1] != 'S'))  return;

					_parseCommand(lpBuffer+2, dwCount);
				}
			}
			else
			{
				// wrong format packet
			}

			//UnlockList();
		}

		inline void _parseCommand(const LPBYTE lpBuffer, DWORD dwCount)
		{
			int index = 0;
			double tempVal;

			_cmdMode = lpBuffer[index];
			index++;

			if (_cmdMode == CMD_TASK)				// TASK MODE => RESCUE MODE | MOTION MODE | TARGET RIGHT (POS, VEL, ACC) | TARGET LEFT (POS, VEL, ACC) | TARGET RELATIVE (POS, VEL, ACC)
			{
				_rescueMode = lpBuffer[index];
				index++;

				_motionMode = lpBuffer[index];
				index++;

				memcpy(&_trajTime, lpBuffer+index, sizeof(double));
				index += sizeof(double);

				for (int i=0; i<9; i++)
				{
					memcpy(&tempVal, lpBuffer+index, sizeof(double));
					index += sizeof(double);

					_tarRight[i] = tempVal;
				}
				for (int i=0; i<9; i++)
				{
					memcpy(&tempVal, lpBuffer+index, sizeof(double));
					index += sizeof(double);

					_tarLeft[i] = tempVal;
				}
				for (int i=0; i<9; i++)
				{
					memcpy(&tempVal, lpBuffer+index, sizeof(double));
					index += sizeof(double);

					_tarRel[i] = tempVal;
				}
			}
			else if (_cmdMode == CMD_JOINT || _cmdMode == CMD_SAMP_JOINT )			// JOINT MODE => TRAJ TIME | JOINT TAR POS (12) | JOINT TAR VEL (12) | JOINT TAR ACC (12
			{
				memcpy(&_trajTime, lpBuffer+index, sizeof(double));
				index += sizeof(double);

				for (int i=0; i<12; i++)
				{
					memcpy(&tempVal, lpBuffer+index, sizeof(double));
					index += sizeof(double);

					_tarPosJoint[i] = tempVal;
				}

				_tarVelJoint.setZero();
				_tarAccJoint.setZero();
				if (_trajTime >= 0)
				{
					for (int i=0; i<12; i++)
					{
						memcpy(&tempVal, lpBuffer+index, sizeof(double));
						index += sizeof(double);

						_tarVelJoint[i] = tempVal;
					}
					for (int i=0; i<12; i++)
					{
						memcpy(&tempVal, lpBuffer+index, sizeof(double));
						index += sizeof(double);

						_tarAccJoint[i] = tempVal;
					}
				}
			}
			else if (_cmdMode == CMD_SEND_PID)
			{
				for (int i=0; i<10; i++)
				{
					memcpy(&tempVal, lpBuffer+index, sizeof(double));
					index += sizeof(double);

					_kp[i] = tempVal;
				}
				for (int i=0; i<10; i++)
				{
					memcpy(&tempVal, lpBuffer+index, sizeof(double));
					index += sizeof(double);

					_ki[i] = tempVal;
				}
				for (int i=0; i<10; i++)
				{
					memcpy(&tempVal, lpBuffer+index, sizeof(double));
					index += sizeof(double);

					_kd[i] = tempVal;
				}
				for (int i=0; i<10; i++)
				{
					memcpy(&tempVal, lpBuffer+index, sizeof(double));
					index += sizeof(double);

					_kl[i] = tempVal;
				}
			}
			else if (_cmdMode == CMD_CHANGE_MODE)
			{
				_rescueMode = lpBuffer[index];
				index++;
			}
		}

		inline virtual void OnEvent(UINT uEvent, LPVOID lpvData)
		{
			switch( uEvent )
			{
			case EVT_CONSUCCESS:
				std::cout << "One client has connected to the Command Server..." << std::endl;
				_commandkey = 0;
				break;

			case EVT_CONDROP:
				std::cout << "Command Server Connection abandoned " << std::endl;

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
		volatile char _commandkey;
		Poco::Event _dataReceiveEvent;

		//COMMAND
		char _cmdMode;
		char _rescueMode;
		char _motionMode;
		double _trajTime;

		Eigen::Matrix<double, 9, 1> _tarRight;
		Eigen::Matrix<double, 9, 1> _tarLeft;
		Eigen::Matrix<double, 9, 1> _tarRel;

		Eigen::Matrix<double, 12, 1> _tarPosJoint;
		Eigen::Matrix<double, 12, 1> _tarVelJoint;
		Eigen::Matrix<double, 12, 1> _tarAccJoint;

		Eigen::Matrix<double, 10, 1> _kp;
		Eigen::Matrix<double, 10, 1> _ki;
		Eigen::Matrix<double, 10, 1> _kd;
		Eigen::Matrix<double, 10, 1> _kl;

		volatile bool _hasConnection; // moved from TESTSocketBase
		// Do not try to move this to the base class
		// Nor to delete volatile attribute. (Then release mode does not work properly).
};
} // namespace NRMKHelper


#endif /* CONTROLSOCKET_H_ */
