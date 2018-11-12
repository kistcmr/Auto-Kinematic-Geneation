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

romas::BlockInfo::BlockInfo(std::string id, std::string name, bool is_base, bool is_tool, std::string tag)
	: id_(id)
	, name_(name)
	, is_base_(is_base)
	, is_tool_(is_tool)
	, tag_(tag)
{

}

romas::BlockInfo::BlockInfo(const BlockInfo& block_info)
{
	id_ = block_info.id_;
	name_ = block_info.name_;
	is_base_ = block_info.is_base_;
	is_tool_ = block_info.is_tool_;
	tag_ = block_info.tag_;
	shape_ = block_info.shape_;
	shape_pose_ = block_info.shape_pose_;
	diffuse_ = block_info.diffuse_;
	normal_ = block_info.normal_;
	thumbnail_ = block_info.thumbnail_;
	contact_points_info_set_ = block_info.contact_points_info_set_;
	xml_path_ = block_info.xml_path_; // °æ·Î
}

std::string romas::BlockInfo::id(void) 
{ 
	return id_; 
}

std::string romas::BlockInfo::name(void) 
{ 
	return name_; 
}

bool romas::BlockInfo::isBase(void) 
{ 
	return is_base_; 
}

bool romas::BlockInfo::isTool(void) 
{ 
	return is_tool_; 
}

std::string romas::BlockInfo::tag(void) 
{ 
	return tag_; 
}

std::string romas::BlockInfo::shape(void) 
{
	return shape_; 
}

//std::string romas::BlockInfo::diffuse(void) { return diffuse_; }
//std::string romas::BlockInfo::normal(void) { return normal_; }
//std::string romas::BlockInfo::thumbnail(void) { return thumbnail_; }
//romas::ContactPointInfoSet& romas::BlockInfo::ContactPointInfoSet(void) { return contact_points_info_set_; }
//std::string romas::BlockInfo::xmlPath(void) { return xml_path_; }
//void romas::BlockInfo::setXmlPath(std::string xml_path) { xml_path_ = xml_path; }
//void romas::BlockInfo::setTag(std::string tag) { tag_ = tag; }
//void romas::BlockInfo::setShape(std::string shape) { shape_ = shape; }
//void romas::BlockInfo::setDiffuse(std::string diffuse) { diffuse_ = diffuse; }
//void romas::BlockInfo::setNormal(std::string normal) { normal_ = normal; }
//void romas::BlockInfo::setThumbnail(std::string thumbnail) { thumbnail_ = thumbnail; }

bool romas::BlockInfoSet::load(std::string xml_path, std::string xml_filename)
{
	tinyxml2::XMLDocument doc;
//	xml_path += "/";

	doc.LoadFile((xml_path+xml_filename).c_str());

	if (doc.Error())
	{
		std::string error1, error2;
		if (!doc.GetErrorStr1())
			error1 = std::string(doc.GetErrorStr1());
		if (!doc.GetErrorStr2())
			error2 = std::string(doc.GetErrorStr2());

		return false;
	}

	tinyxml2::XMLElement* romas_element = doc.FirstChildElement("Romas");

	if (!romas_element)
	{
		//error
		return false;
	}

	tinyxml2::XMLElement* blocks_element = romas_element->FirstChildElement("Blocks");

	if (!blocks_element)
	{
		//error
		return false;
	}

	tinyxml2::XMLElement* block_element;
	
	for (
		block_element = blocks_element->FirstChildElement("Block"); 
		block_element != nullptr; 
		block_element = block_element->NextSiblingElement()
		)
	{
		loadBlockElement(block_element, xml_path);
	}

	return true;
}

void romas::BlockInfo::pushBack(romas::ContactPointInfo contact_points_info)
{
	contact_points_info_set_.push_back(contact_points_info);
}

bool romas::BlockInfoSet::loadBlockElement(tinyxml2::XMLElement *block_element, std::string xml_path)
{
	if (!block_element)
		return false;

	std::string id = romas::convert(block_element->Attribute("id"));
	std::string name = romas::convert(block_element->Attribute("name"));
	bool is_base = block_element->BoolAttribute("isBaseBlock");
	bool is_tool = block_element->BoolAttribute("isToolBlock");
	std::string tag = romas::convert(block_element->Attribute("tag"));

	romas::BlockInfo block_info(id, name, is_base, is_tool);

	tinyxml2::XMLElement *shapeElement = block_element->FirstChildElement("shape");
	block_info.setShape(romas::convert(shapeElement->Attribute("value")));
	if (const char* shape_pose = shapeElement->Attribute("pose"))
	{
		block_info.setShapePose(romas::convert(shape_pose));
	}

	tinyxml2::XMLElement *diffuseElement = block_element->FirstChildElement("diffuse");
	block_info.setDiffuse(romas::convert(diffuseElement->Attribute("value")));

	tinyxml2::XMLElement *normalElement = block_element->FirstChildElement("normal");
	block_info.setNormal(romas::convert(normalElement->Attribute("value")));

	tinyxml2::XMLElement *thumbnailElement = block_element->FirstChildElement("thumbnail");
	block_info.setThumbnail(romas::convert(thumbnailElement->Attribute("value")));

	tinyxml2::XMLElement *contact_points_element = block_element->FirstChildElement("contactPoints");

	for (tinyxml2::XMLElement *contact_element = contact_points_element->FirstChildElement("contact");
		contact_element != NULL;
		contact_element = contact_element->NextSiblingElement())
	{
		std::string pose = romas::convert(contact_element->Attribute("pose"));
		std::string type = romas::convert(contact_element->Attribute("type"));
		std::string rot_axis = romas::convert(contact_element->Attribute("rotAxis"));
		std::string zero_axis = romas::convert(contact_element->Attribute("zeroAxis"));

		block_info.pushBack(romas::ContactPointInfo(pose, type, rot_axis, zero_axis));
	}

	block_info.setXmlPath(xml_path);

	block_info_set_.push_back(block_info);
	return true;
}

romas::Block::Block(const romas::BlockInfo& block_info)
	: osg::Group()
	, block_info_(block_info)
{
//	transform_ = new osg::PositionAttitudeTransform();
	transform_ = new osg::MatrixTransform();

	osg::PositionAttitudeTransform *shape_transform = new osg::PositionAttitudeTransform();

	xml_path_ = block_info_.xmlPath();

	osg::PositionAttitudeTransform *scale = new osg::PositionAttitudeTransform();
	scale->setScale(osg::Vec3d(0.001, 0.001, 0.001));
	osg::Node *obj = osgDB::readNodeFile(xml_path_ + block_info_.shape());

	std::string name;
//	name = obj->getName();
//	obj->setName("obj");
//	name = obj->getName();

	osg::Image *im = osgDB::readImageFile(xml_path_ + block_info_.diffuse());


	osg::StateSet* ss = obj->getOrCreateStateSet();
//	osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D(osgDB::readImageFile(xml_path_ + diffuse_));

//	ss->setTextureAttributeAndModes(0, tex.get());

	//osg::StateSet* ss = new osg::StateSet();

	//osg::Material *mat = new osg::Material();
	//	mat->setAlpha(osg::Material::FRONT_AND_BACK, 0.1);
	//	mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.3, 0, 0, 0.5));
	//	mat->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	//	mat->setTransparency(osg::Material::FRONT_AND_BACK, 0.5);
	//	ss->setAttributeAndModes(mat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	ss->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);


	std::string shape_pose = block_info_.shapePose();
	if (!shape_pose.empty())
	{
		double x, y, z, roll, pitch, yaw;

		std::vector<std::string> position_and_orientation;
		romas::tokenize(shape_pose, ";", position_and_orientation);
		if (position_and_orientation.size() == 3)
		{
			x = romas::lexical_cast<double>(position_and_orientation[0]);
			y = romas::lexical_cast<double>(position_and_orientation[1]);
			z = romas::lexical_cast<double>(position_and_orientation[2]);

			shape_transform->setPosition(osg::Vec3d(x, y, z));
		}
		else if (position_and_orientation.size() == 6)
		{
			x = romas::lexical_cast<double>(position_and_orientation[0]);
			y = romas::lexical_cast<double>(position_and_orientation[1]);
			z = romas::lexical_cast<double>(position_and_orientation[2]);
			roll = romas::lexical_cast<double>(position_and_orientation[3]) * romas::DTR;
			pitch = romas::lexical_cast<double>(position_and_orientation[4]) * romas::DTR;
			yaw = romas::lexical_cast<double>(position_and_orientation[5]) * romas::DTR;

			shape_transform->setPosition(osg::Vec3d(x, y, z));
			double qx, qy, qz, qw;
			romas::toQuaternion(roll, pitch, yaw, qw, qx, qy, qz);
			shape_transform->setAttitude(osg::Quat(qx, qy, qz, qw));
		}
	}

	int i = 0;
	for (auto itr = block_info_.beginContactPointInfo(); itr != block_info_.endContactPointInfo(); itr++)
	{
		romas::ContactPoint *joint = new romas::ContactPoint(this, *itr, i);
		transform_->addChild(joint);
		contact_points_.push_back(joint);
		i++;
	}

	scale->addChild(obj);
	shape_transform->addChild(scale);
	transform_->addChild(shape_transform);
	this->addChild(transform_);

	name = obj->getName();

	setAssembleId(-1);
}

void romas::Block::setFrame(screw::SE3d frame)
{
	frame_ = frame;

	osg::Matrix m;
	m = osg::Matrix::identity();
	screw::vector3d p = frame.position();

	m.setTrans(osg::Vec3(p[0], p[1], p[2]));
	m.setRotate(romas::rotation2quaternian(frame.rotation()));

	transform_->setMatrix(m);
}

void romas::Block::setQuaternion(double x, double y, double z, double w)
{
	osg::Matrix m = transform_->getMatrix();
	m.setRotate(osg::Quat(x, y, z, w));

	transform_->setMatrix(m);
}

void romas::Block::setPosition(double x, double y, double z)
{
	frame_.setPosition(screw::vector3d(x, y, z));

	osg::Matrix m = transform_->getMatrix();
	m.setTrans(osg::Vec3(x, y, z));
	transform_->setMatrix(m);
}

void romas::Block::setAssembleId(int id)
{
	assemble_id_ = id;
	for (auto itr = contact_points_.begin(); itr != contact_points_.end(); itr++)
	{
		(*itr)->setMarkerName(assemble_id_);
	}
}

//void romas::Block::setOrientation(double roll, double pitch, double yaw)
//{
//	roll_ = roll;
//	pitch_ = pitch;
//	yaw_ = yaw;
//
//	double w, x, y, z;
//	romas::toQuaternion(roll, pitch, yaw, w, x, y, z);
//
//	transform_->setAttitude(osg::Quat(x, y, z, w));
//}


std::vector<osg::Geode*> romas::Block::jointMarkerGeodes(void)
{
	std::vector<osg::Geode*> geodes;

	for (std::vector<romas::ContactPoint*>::iterator itr = contact_points_.begin();
		itr != contact_points_.end(); itr++)
	{
		geodes.push_back((*itr)->markerGeode());
	}

	return geodes;
}

void romas::Block::getContactPointLocalFrame(unsigned int cpid, double& x, double& y, double& z, double& roll, double& pitch, double& yaw)
{
	contact_points_[cpid]->getTransform(x, y, z, roll, pitch, yaw);
}

romas::ContactPointType romas::Block::getContactPointType(unsigned int cpid)
{
	return contact_points_[cpid]->type();
}

screw::SE3d romas::Block::getContactPointFrame(unsigned int cpid)
{
	double x, y, z, roll, pitch, yaw;
	contact_points_[cpid]->getTransform(x, y, z, roll, pitch, yaw);

	screw::SE3d T(screw::SO3d(screw::RotationType::ROLL_PITCH_YAW, roll, pitch, yaw), screw::vector3d(x, y, z));

	return frame_ * T;
}

screw::SE3d romas::Block::getContactPointLocalFrame(unsigned int cpid)
{
	double x, y, z, roll, pitch, yaw;
	contact_points_[cpid]->getTransform(x, y, z, roll, pitch, yaw);

	screw::SE3d T(screw::SO3d(screw::RotationType::ROLL_PITCH_YAW, roll, pitch, yaw), screw::vector3d(x, y, z));

	return T;
}

romas::AXIS romas::Block::getContactPointRotAxis(unsigned int cpid)
{
	return contact_points_[cpid]->rotAxis();
}

romas::AXIS romas::Block::getContactPointZeroAxis(unsigned int cpid)
{
	return contact_points_[cpid]->zeroAxis();
}
