
#include <romas/Geometry.hpp>
#include <romas/romas.hpp>

#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Geometry>

osg::Geode* romas::createArrow(const osg::Vec4& color, double radius, double length)
{
	double cylinder_radius = radius;
	double cylinder_height = length * 0.7;
	double cone_radius = radius * 1.7;
	double cone_height = length - cylinder_height;

	if (cone_height > cone_radius * 3)
	{
		cone_height = cone_radius * 3;
		cylinder_height = length - cone_height;
	}

	osg::Cylinder* cylinder = new osg::Cylinder(osg::Vec3(0, 0, cylinder_height * 0.5), cylinder_radius, cylinder_height);
	osg::Cone *cone = new osg::Cone(osg::Vec3(0, 0, cylinder_height), cone_radius, cone_height);

	osg::Geode *geode = new osg::Geode();
	osg::ShapeDrawable* sd[2];
	sd[0] = new osg::ShapeDrawable(cylinder);
	sd[1] = new osg::ShapeDrawable(cone);

	osg::Material *material = new osg::Material();

	sd[0]->setColor(color);
	sd[1]->setColor(color);

	geode->addDrawable(sd[0]);
	geode->addDrawable(sd[1]);

	osg::StateSet *state_set = geode->getOrCreateStateSet();
	material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

	state_set->setAttributeAndModes(material, osg::StateAttribute::ON);
	state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	return geode;
}

//osg::MatrixTransform* createLine(const osg::Matrixd& matrix, const osg::Vec4& color, double radius, double length)
//{

//	osg::Vec3 sp(0, -180, 120);
//	osg::Vec3 ep(0, 480, 120);
//	osg::ref_ptr<osg::Geometry> beam(new osg::Geometry);
//	osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
//	points->push_back(sp);
//	points->push_back(ep);
////	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;
////	color->set(1.0, 0.0, 0.0, 1.0);
//	beam->setVertexArray(points.get());
////	beam->setColorArray(color);
//	beam->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
//	beam->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));
//	return 0;
//}

osg::Group* romas::createArrowCoordinate(double radius, double length)
{
	osg::Group *coordinate = new osg::Group();

	// x axis
	osg::Matrixd m_x = osg::Matrix::rotate(osg::inDegrees(90.0f), 0.0f, 1.0f, 0.0f);
	osg::MatrixTransform* transform_x = new osg::MatrixTransform(m_x);
	osg::Vec4 color_red(1.f, 0.f, 0.f, 0.1f);
	osg::Geode* geode_x = createArrow(color_red, radius, length);
	transform_x->addChild(geode_x);

	// y axis
	osg::Matrixd m_y = osg::Matrix::rotate(osg::inDegrees(-90.0f), 1.0f, 0.0f, 0.0f);
	osg::MatrixTransform* transform_y = new osg::MatrixTransform(m_y);
	osg::Vec4 color_green(0.f, 1.0f, 0.f, 1.0f);
	osg::Geode* geode_y= createArrow(color_green, radius, length);
	transform_y->addChild(geode_y);

	// z axis
	osg::Matrixd m_z = osg::Matrix::identity();
	osg::MatrixTransform* transform_z = new osg::MatrixTransform(m_z);
	osg::Vec4 color_blue(0.f, 0.f, 1.f, 1.0f);
	osg::Geode* geode_z = createArrow(color_blue, radius, length);
	transform_z->addChild(geode_z);

	coordinate->addChild(transform_x);
	coordinate->addChild(transform_y);
	coordinate->addChild(transform_z);

	return coordinate;
}

osg::Geode* romas::createLineCoordinate(double length)
{
	osg::Geode* geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geometry(new osg::Geometry());

	osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array());
	vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
	vertices->push_back(osg::Vec3(length, 0.0, 0.0));
	vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
	vertices->push_back(osg::Vec3(0.0, length, 0.0));
	vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
	vertices->push_back(osg::Vec3(0.0, 0.0, length));
	geometry->setVertexArray(vertices.get());

	osg::ref_ptr<osg::Vec4Array> colors(new osg::Vec4Array());
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	geometry->setColorArray(colors.get(), osg::Array::BIND_PER_VERTEX);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));

	geode->addDrawable(geometry.get());
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
	return geode;
}

screw::SO3d romas::calculateRotation(romas::AXIS parent_axis1, romas::AXIS child_axis1)
{
	romas::AXIS parent_axis2, child_axis2;
//	child_axis1 = romas::minusAxis(child_axis1);

	switch (parent_axis1)
	{
	case romas::AXIS_PLUS_X:
		parent_axis2 = romas::AXIS_PLUS_Y;
		break;
	case romas::AXIS_PLUS_Y:
		parent_axis2 = romas::AXIS_PLUS_Z;
		break;
	case romas::AXIS_PLUS_Z:
		parent_axis2 = romas::AXIS_PLUS_X;
		break;
	case romas::AXIS_MINUS_X:
		parent_axis2 = romas::AXIS_MINUS_Y;
		break;
	case romas::AXIS_MINUS_Y:
		parent_axis2 = romas::AXIS_MINUS_Z;
		break;
	default:
	case romas::AXIS_MINUS_Z:
		parent_axis2 = romas::AXIS_MINUS_X;
		break;
	}

	switch (child_axis1)
	{
	case romas::AXIS_PLUS_X:
		child_axis2 = romas::AXIS_PLUS_Y;
		break;
	case romas::AXIS_PLUS_Y:
		child_axis2 = romas::AXIS_PLUS_Z;
		break;
	case romas::AXIS_PLUS_Z:
		child_axis2 = romas::AXIS_PLUS_X;
		break;
	case romas::AXIS_MINUS_X:
		child_axis2 = romas::AXIS_MINUS_Y;
		break;
	case romas::AXIS_MINUS_Y:
		child_axis2 = romas::AXIS_MINUS_Z;
		break;
	default:
	case romas::AXIS_MINUS_Z:
		child_axis2 = romas::AXIS_MINUS_X;
		break;
	}

	return romas::calculateRotation(parent_axis1, child_axis1, parent_axis2, child_axis2);
}

screw::SO3d romas::calculateRotation(romas::AXIS parent_axis1, romas::AXIS child_axis1, romas::AXIS parent_axis2, romas::AXIS child_axis2)
{
	romas::AXIS parent_axis[3];
	romas::AXIS child_axis[3];

	parent_axis[0] = parent_axis1;
	parent_axis[1] = parent_axis2;

	child_axis1 = romas::minusAxis(child_axis1);

	child_axis[0] = child_axis1;
	child_axis[1] = child_axis2;

	parent_axis[2] = romas::cross(parent_axis[0], parent_axis[1]);
	child_axis[2] = romas::cross(child_axis[0], child_axis[1]);

	if ((parent_axis[2] == romas::AXIS_NUM) || (child_axis[2] == romas::AXIS_NUM))
		return screw::SO3d();


	screw::vector3d v[3];

	for (int i = 0; i < 3; i++)
	{
		switch (child_axis[i])
		{
		case romas::AXIS_PLUS_X:
			v[0] = romas::axis2vector(parent_axis[i]);
			break;
		case romas::AXIS_PLUS_Y:
			v[1] = romas::axis2vector(parent_axis[i]);
			break;
		case romas::AXIS_PLUS_Z:
			v[2] = romas::axis2vector(parent_axis[i]);
			break;
		case romas::AXIS_MINUS_X:
			v[0] = -romas::axis2vector(parent_axis[i]);
			break;
		case romas::AXIS_MINUS_Y:
			v[1] = -romas::axis2vector(parent_axis[i]);
			break;
		case romas::AXIS_MINUS_Z:
			v[2] = -romas::axis2vector(parent_axis[i]);
			break;
		}
	}

	return screw::SO3d(v[0], v[1], v[2]);
}
