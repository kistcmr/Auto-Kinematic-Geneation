#ifndef _SCREW_HPP_
#define _SCREW_HPP_

#include <list>

namespace screw {

	class System;
	class Link;
	class Joint;
	class FixedJoint;
	class RevoluteJoint;
	class PrismaticJoint;

	typedef std::list<screw::Link *> LinkPointers;
	typedef std::list<screw::Joint *> JointPointers;

	enum class Axis : short
	{
		X = 0,
		Y,
		Z
	};


	enum class Direction : short
	{
		PARENT = 0,
		CHILD
	};

	enum class JointType : short
	{
		FIXED = 0,
		REVOLUTE,
		PRISMATIC,
	};

	enum class RotationType : short
	{
		ROLL_PITCH_YAW = 0
	}; 

	const double PI = 3.141592653589793238462643383279502884197;
	const double PI_2 = 1.570796326794896619231321691639751442099;
	const double PI_4 = 0.7853981633974483096156608458198757210495;
	const double EPS = 2.2204E-16;
	const double INF = 1.0E100;
	const double SMALL = 1.0e-9;
	const double TINY = 1.0E-12;
	const double SVD_EPS = 1.0e-6;
	const double DGEFA_EPS = 1.0E-6;
	const double LCP_EPS = 1.0E-6;

	const double MATRIX_NORM_EPS = 1.0E-20;	
	const double VECTOR_NORM_EPS = 1.0E-20;	
	const double EXP_EPS = 1.0E-12;	
	const double LOG_EPS = 1.0E-12;
	const double INV_EULER_EPS = 1.0E-16;

	//#define DEG2RAD (2*(EigenDefine::PI)/360)


}

#endif