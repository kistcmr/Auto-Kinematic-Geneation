#ifndef __ROMAS_GEOMETRY__
#define __ROMAS_GEOMETRY__

//#include <osg/ref_ptr>
//
//#include <osgViewer/GraphicsWindow>
//#include <osgViewer/CompositeViewer>
//#include <osgGA/TrackballManipulator>
//#include <osgText/String>
//#include <osg/Quat>
#include <romas/romas.hpp>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <screw/Lie.hpp>

namespace romas {
	//osg::MatrixTransform* createLine(const osg::Matrixd& matrix, const osg::Vec4& color, double radius, double length);

	// z-axis arrow
	osg::Geode* createArrow(const osg::Vec4& color, double radius, double length);


	osg::Group* createArrowCoordinate(double radius, double length);

	osg::Geode* createLineCoordinate(double length);

	inline osg::Quat rotation2quaternian(screw::SO3d R)
	{
		Eigen::Quaterniond q(R);

		double w = q.w();
		double x = q.x();
		double y = q.y();
		double z = q.z();
		return 	osg::Quat(x, y, z, w);
	}

	screw::SO3d calculateRotation(romas::AXIS parent_axis1, romas::AXIS child_axis1);
	screw::SO3d calculateRotation(romas::AXIS parent_axis1, romas::AXIS child_axis1, romas::AXIS parent_axis2, romas::AXIS child_axis2);

}



#endif