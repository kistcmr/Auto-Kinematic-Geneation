#include <romas/Block.hpp>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/Switch>
#include <romas/ustring.hpp>
#include <romas/ContactPoint.hpp>

//void romas::ContactPointInfoSet::pushBack(romas::ContactPointInfo info)
//{
//	contact_points_info_set_.push_back(info);
//}

romas::ContactPoint::ContactPoint(romas::Block *block, romas::ContactPointInfo info, int id)
	: osg::Group()
	, block_(block)
	, id_(id)
	, x_(0)
	, y_(0)
	, z_(0)
	, roll_(0)
	, pitch_(0)
	, yaw_(0)
{
	marker_node_ = new osg::Switch();
	coordinate_node_ = new osg::Switch();

	osg::ShapeDrawable* sd;
	osg::Geode* geode;
	osg::StateSet* state_set;
	osg::Material* material;
	//		osg::Image* diffuse = osgDB::readImageFile(xml_path_ + diffuse_);

	type_ = (info.type() == std::string("joint")) ? ContactPointType::Joint : ContactPointType::Assembly;

	std::string pose = info.pose();
	std::vector<std::string> position_and_orientation;
	romas::tokenize(pose, ";", position_and_orientation);
	if (position_and_orientation.size() == 3)
	{
		x_ = romas::lexical_cast<double>(position_and_orientation[0]);
		y_ = romas::lexical_cast<double>(position_and_orientation[1]);
		z_ = romas::lexical_cast<double>(position_and_orientation[2]);
	}
	else if (position_and_orientation.size() == 6)
	{
		x_ = romas::lexical_cast<double>(position_and_orientation[0]);
		y_ = romas::lexical_cast<double>(position_and_orientation[1]);
		z_ = romas::lexical_cast<double>(position_and_orientation[2]);
		roll_ = romas::lexical_cast<double>(position_and_orientation[3]) * romas::DTR;
		pitch_ = romas::lexical_cast<double>(position_and_orientation[4]) * romas::DTR;
		yaw_ = romas::lexical_cast<double>(position_and_orientation[5]) * romas::DTR;
	}

	osg::PositionAttitudeTransform *transform = new osg::PositionAttitudeTransform();
	transform->setPosition(osg::Vec3d(x_, y_, z_));
	double x, y, z, w;
	romas::toQuaternion(roll_, pitch_, yaw_, w, x, y, z);
	transform->setAttitude(osg::Quat(x, y, z, w));

	marker_geode_ = new osg::Geode;
	material = new osg::Material;

	// joint= sphere
	if (type_ == ContactPointType::Joint)
	{
		osg::Sphere *sphere = new osg::Sphere(osg::Vec3(), 0.02);
		sd = new osg::ShapeDrawable(sphere);
		sd->setColor(osg::Vec4(0.f, 1.f, 0.f, 1.0f));
	}
	// assembly = box
	else
	{
		osg::Box *box = new osg::Box(osg::Vec3(0, 0, 0), 0.03);
		sd = new osg::ShapeDrawable(box);
		sd->setColor(osg::Vec4(1.f, 1.f, 1.f, 1.f));
	}

	marker_geode_->addDrawable(sd);
	state_set = marker_geode_->getOrCreateStateSet();
	state_set->setAttributeAndModes(material, osg::StateAttribute::ON);
	material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	state_set->setMode(GL_BLEND, osg::StateAttribute::ON);
	state_set->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//	geode->setName(std::string("-J") + std::to_string(ID_));
	//	box_geode_->setName(std::string("1"));


	marker_node_->addChild(marker_geode_);
	coordinate_node_->addChild(romas::createArrowCoordinate(0.002, 0.1));

	transform->addChild(marker_node_);
	transform->addChild(coordinate_node_);

	this->addChild(transform);

	rot_axis_ = romas::string2axis(info.rotAxis());
	zero_axis_ = romas::string2axis(info.zeroAxis());
}

void romas::ContactPoint::setMarkerName(int block_assemble_id)
{
	std::string name = romas::lexical_cast<std::string, int>(block_assemble_id) 
		+ ";" 
		+ romas::lexical_cast<std::string, unsigned int>(id_);

	marker_geode_->setName(name);
}