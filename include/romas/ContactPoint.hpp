#ifndef __ROMAS_CONTACT_POINT__
#define __ROMAS_CONTACT_POINT__

#include <romas/romas.hpp>
#include <romas/Geometry.hpp>
#include <vector>
#include <screw/Link.hpp>
#include <romas/tinyxml2.hpp>
#include <osg/Group>
#include <osg/Switch>
#include <osg/MatrixTransform>

namespace romas
{

	class ContactPointInfo
	{
	public:
		ContactPointInfo(std::string pose, std::string type, std::string rot_axis, std::string zero_axis) : pose_(pose), type_(type), rot_axis_(rot_axis), zero_axis_(zero_axis) {};
		std::string pose(void) { return pose_; }
		std::string type(void) { return type_; }
		std::string rotAxis(void) { return rot_axis_; }
		std::string zeroAxis(void) { return zero_axis_; }
	private:
		std::string pose_;
		std::string type_;
		std::string rot_axis_;
		std::string zero_axis_;
	};
	
	typedef std::vector<ContactPointInfo> ContactPointInfoSet;

	class ContactPoint : public osg::Group
	{
	public:
		ContactPoint(romas::Block *block, romas::ContactPointInfo info, int ID);
		int getId(void) { return id_; }
		osg::Geode *markerGeode(void) { return marker_geode_; }
		void setMarkerName(int block_assemble_id);
		void getTransform(double& x, double& y, double& z, double& roll, double& pitch, double& yaw)
		{
			x = x_;
			y = y_;
			z = z_;
			roll = roll_;
			pitch = pitch_;
			yaw = yaw_;
		}

		romas::ContactPointType type(void) { return type_; }

		romas::AXIS rotAxis(void) { return rot_axis_; }
		romas::AXIS zeroAxis(void) { return zero_axis_; }

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:
		romas::Block *block_;
		osg::Switch *marker_node_; // ContactPointType이 joint인 경우 구형, assembly인 경우 상자
		osg::Switch *coordinate_node_;
		osg::Geode *marker_geode_;
		int id_;
		double x_;
		double y_;
		double z_;
		double roll_;
		double pitch_;
		double yaw_;
		ContactPointType type_;
		romas::AXIS rot_axis_;
		romas::AXIS zero_axis_;
	};
}

#endif
