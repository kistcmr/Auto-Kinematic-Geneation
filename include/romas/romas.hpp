#ifndef __ROMAS__
#define __ROMAS__

#include <string>
#include <screw/Lie.hpp>

namespace romas
{
	class BlockInfo;
	class BlockInfoSet;
	class Block;

	class ContactPointInfo;
//	class ContactPointInfoSet;
	class ContactPoint;

	class Joint;
	class FixedJoint;
	class RevoluteJoint;
	class PrismaticJoint;

	class Assembly;

	class Config;

	const double RTD = 57.29577951;
	const double DTR = 0.01745329252;

	enum AXIS
	{
		AXIS_PLUS_X = 0,
		AXIS_PLUS_Y = 1,
		AXIS_PLUS_Z = 2,
		AXIS_MINUS_X = 3,
		AXIS_MINUS_Y = 4,
		AXIS_MINUS_Z = 5,
		AXIS_NUM = 6
	};

	enum class JointType : short
	{
		Fixed = 0,
		Revolute = 1,
		Prismatic = 2
	};

	enum class ContactPointType : short
	{
		Joint,
		Assembly
	};

	enum class Solver : short
	{
		Kinematic,
		Dynamic
	};

	inline screw::vector3d axis2vector(romas::AXIS axis)
	{
		switch (axis)
		{
		case AXIS_PLUS_X :
			return screw::vector3d(1, 0, 0);
			break;
		case AXIS_PLUS_Y :
			return screw::vector3d(0, 1, 0);
			break;
		case AXIS_PLUS_Z :
			return screw::vector3d(0, 0, 1);
			break;
		case AXIS_MINUS_X :
			return screw::vector3d(-1, 0, 0);
			break;
		case AXIS_MINUS_Y :
			return screw::vector3d(0, -1, 0);
			break;
		case AXIS_MINUS_Z :
			return screw::vector3d(0, 0, -1);
			break;
		default:
		case AXIS_NUM :
			return screw::vector3d(1, 0, 0);
			break;
		}
	}

	inline romas::AXIS minusAxis(romas::AXIS axis)
	{
		switch (axis)
		{
		case AXIS_PLUS_X:
			return AXIS_MINUS_X;
			break;
		case AXIS_PLUS_Y:
			return AXIS_MINUS_Y;
			break;
		case AXIS_PLUS_Z:
			return AXIS_MINUS_Z;
			break;
		case AXIS_MINUS_X:
			return AXIS_PLUS_X;
			break;
		case AXIS_MINUS_Y:
			return AXIS_PLUS_Y;
			break;
		case AXIS_MINUS_Z:
			return AXIS_PLUS_Z;
			break;
		default:
		case AXIS_NUM:
			return AXIS_NUM;
			break;
		}
	}

	inline std::string axis2string(romas::AXIS axis)
	{
		switch (axis)
		{
		default :
		case AXIS_PLUS_X:
			return std::string("x");
			break;
		case AXIS_PLUS_Y:
			return std::string("y");
			break;
		case AXIS_PLUS_Z:
			return std::string("z");
			break;
		case AXIS_MINUS_X:
			return std::string("-x");
			break;
		case AXIS_MINUS_Y:
			return std::string("-y");
			break;
		case AXIS_MINUS_Z:
			return std::string("-z");
			break;
		}
	}

	inline romas::AXIS string2axis(std::string str)
	{
		if ((str == std::string("x")) || (str == std::string("X")))
		{
			return romas::AXIS_PLUS_X;
		}
		else if ((str == std::string("y")) || (str == std::string("Y")))
		{
			return romas::AXIS_PLUS_Y;
		}
		else if ((str == std::string("z")) || (str == std::string("Z")))
		{
			return romas::AXIS_PLUS_Z;
		}
		else if ((str == std::string("-x")) || (str == std::string("-X")))
		{
			return romas::AXIS_MINUS_X;
		}
		else if ((str == std::string("-y")) || (str == std::string("-Y")))
		{
			return romas::AXIS_MINUS_Y;
		}
		else if ((str == std::string("-z")) || (str == std::string("-Z")))
		{
			return romas::AXIS_MINUS_Z;
		}
		return romas::AXIS_NUM;
	}

	// return axis1 x axis 2
	inline romas::AXIS cross(romas::AXIS axis1, romas::AXIS axis2)
	{
		screw::vector3d axis1_vector = axis2vector(axis1);
		screw::vector3d axis2_vector = axis2vector(axis2);
		screw::vector3d axis3_vector = axis1_vector.cross(axis2_vector);

		if (axis3_vector(0) > 0.5)
			return romas::AXIS_PLUS_X;
		else if (axis3_vector(0) < -0.5)
			return romas::AXIS_MINUS_X;
		else if (axis3_vector(1) > 0.5)
			return romas::AXIS_PLUS_Y;
		else if (axis3_vector(1) < -0.5)
			return romas::AXIS_MINUS_Y;
		else if (axis3_vector(2) > 0.5)
			return romas::AXIS_PLUS_Z;
		else if (axis3_vector(2) < -0.5)
			return romas::AXIS_MINUS_Z;
		else
			return romas::AXIS_NUM;


		//romas::AXIS ret = romas::AXIS_PLUS_X;

		//switch (axis1)
		//{
		//case AXIS_PLUS_X:
		//{
		//	switch (axis2)
		//	{
		//	case AXIS_PLUS_Y:
		//		ret = romas::AXIS_PLUS_Z;
		//		break;
		//	case AXIS_PLUS_Z:
		//		ret = romas::AXIS_MINUS_Y;
		//		break;
		//	case AXIS_MINUS_Y:
		//		ret = romas::AXIS_MINUS_Z;
		//		break;
		//	case AXIS_MINUS_Z:
		//		ret = romas::AXIS_PLUS_Y;
		//		break;
		//	}
		//	break;
		//}
		//case AXIS_PLUS_Y:
		//{
		//	switch (axis2)
		//	{
		//	case AXIS_PLUS_X:
		//		ret = romas::AXIS_MINUS_Z;
		//		break;
		//	case AXIS_PLUS_Z:
		//		ret = romas::AXIS_PLUS_X;
		//		break;
		//	case AXIS_MINUS_X:
		//		ret = romas::AXIS_PLUS_Z;
		//		break;
		//	case AXIS_MINUS_Z:
		//		ret = romas::AXIS_MINUS_X;
		//		break;
		//	}
		//	break;
		//}
		//case AXIS_PLUS_Z:
		//{
		//	switch (axis2)
		//	{
		//	case AXIS_PLUS_X:
		//		ret = romas::AXIS_PLUS_Y;
		//		break;
		//	case AXIS_PLUS_Y:
		//		ret = romas::AXIS_MINUS_X;
		//		break;
		//	case AXIS_MINUS_X:
		//		ret = romas::AXIS_MINUS_Y;
		//		break;
		//	case AXIS_MINUS_Y:
		//		ret = romas::AXIS_PLUS_X;
		//		break;
		//	}
		//	break;
		//}
		//case AXIS_MINUS_X:
		//{
		//	switch (axis2)
		//	{
		//	case AXIS_PLUS_Y:
		//		ret = romas::AXIS_MINUS_Z;
		//		break;
		//	case AXIS_PLUS_Z:	
		//		ret = romas::AXIS_PLUS_Y;
		//		break;
		//	case AXIS_MINUS_Y:
		//		ret = romas::AXIS_PLUS_Z;
		//		break;
		//	case AXIS_MINUS_Z:
		//		ret = romas::AXIS_MINUS_Y;
		//		break;
		//	}
		//	break;
		//}
		//case AXIS_MINUS_Y:
		//{
		//	switch (axis2)
		//	{
		//	case AXIS_PLUS_X:
		//		ret = romas::AXIS_PLUS_Z;
		//		break;
		//	case AXIS_PLUS_Z:
		//		ret = romas::AXIS_MINUS_X;
		//		break;
		//	case AXIS_MINUS_X:
		//		ret = romas::AXIS_MINUS_Z;
		//		break;
		//	case AXIS_MINUS_Z:
		//		ret = romas::AXIS_PLUS_X;
		//		break;
		//	}
		//	break;
		//}
		//case AXIS_MINUS_Z:
		//{
		//	switch (axis2)
		//	{
		//	case AXIS_PLUS_X:
		//		ret = romas::AXIS_MINUS_Y;
		//		break;
		//	case AXIS_PLUS_Y:
		//		ret = romas::AXIS_PLUS_X;
		//		break;
		//	case AXIS_MINUS_X:
		//		ret = romas::AXIS_PLUS_Y;
		//		break;
		//	case AXIS_MINUS_Y:
		//		ret = romas::AXIS_MINUS_X;
		//		break;
		//	}
		//	break;
		//}
		//}
		//return ret;
	}

	inline std::string jointtype2string(romas::JointType type)
	{
		switch (type)
		{
		default:
		case romas::JointType::Fixed :
			return std::string("fixed");
		case romas::JointType::Revolute:
			return std::string("revolute");
		case romas::JointType::Prismatic:
			return std::string("prismatic");
		}
	}

	inline romas::JointType string2jointtype(std::string str)
	{
		if (str == std::string("revolute"))
		{
			return romas::JointType::Revolute;
		}
		else if (str == std::string("prismatic"))
		{
			return romas::JointType::Prismatic;
		}
		else //if (str == std::string("fixed"))
		{
			return romas::JointType::Fixed;
		}
	}
}


#endif
