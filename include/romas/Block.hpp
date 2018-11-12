#ifndef __ROMAS_BLOCK__
#define __ROMAS_BLOCK__

#include <romas/romas.hpp>
#include <romas/Geometry.hpp>
#include <vector>
#include <romas/tinyxml2.hpp>
#include <osg/Group>
#include <osg/Switch>
#include <osg/MatrixTransform>
#include <romas/ContactPoint.hpp>

namespace romas
{
	class BlockInfo
	{
	public:
		BlockInfo() {};
		BlockInfo(std::string id, std::string name, bool is_base, bool is_tool, std::string tag = std::string());
		BlockInfo(const BlockInfo& block_info);

		std::string id(void); 
		std::string name(void);
		bool isBase(void); 
		bool isTool(void);
		std::string tag(void);
		std::string shape(void);
		std::string shapePose(void) { return shape_pose_; }
		std::string diffuse(void) { return diffuse_; }
		std::string normal(void) { return normal_; }
		std::string thumbnail(void) { return thumbnail_; }
//		romas::ContactPointInfoSet& ContactPointInfoSet(void) { return contact_points_info_set_; }
		std::string xmlPath(void) { return xml_path_; }
		void setXmlPath(std::string xml_path) { xml_path_ = xml_path; }
		void setTag(std::string tag) { tag_ = tag; }
		void setShape(std::string shape) { shape_ = shape; }
		void setShapePose(std::string shape_pose) { shape_pose_ = shape_pose; }
		void setDiffuse(std::string diffuse) { diffuse_ = diffuse; }
		void setNormal(std::string normal) { normal_ = normal; }
		void setThumbnail(std::string thumbnail) { thumbnail_ = thumbnail; }
		void pushBack(romas::ContactPointInfo contact_points_info_);
		std::vector<romas::ContactPointInfo>::iterator beginContactPointInfo(void) { return contact_points_info_set_.begin(); }
		std::vector<romas::ContactPointInfo>::iterator endContactPointInfo(void) { return contact_points_info_set_.end(); }

	private:
		std::string id_;
		std::string name_;
		bool is_base_;
		bool is_tool_;
		std::string tag_;
		std::string shape_;
		std::string shape_pose_;
		std::string diffuse_;
		std::string normal_;
		std::string thumbnail_;
		romas::ContactPointInfoSet contact_points_info_set_;
		std::string xml_path_; // 경로
	};

	class BlockInfoSet
	{
	public:
		bool load(std::string xml_path, std::string xml_filename);
		bool loadBlockElement(tinyxml2::XMLElement *block_element, std::string xml_path);

		std::vector<romas::BlockInfo>::iterator begin(void) { return block_info_set_.begin(); }
		std::vector<romas::BlockInfo>::iterator end(void) { return block_info_set_.end(); }

	private:
		std::vector<romas::BlockInfo> block_info_set_;
	};

	class Block : public osg::Group
	{
	public :
		Block(const romas::BlockInfo& block_info);
		BlockInfo blockInfo(void) { return block_info_; }

		screw::SE3d frame(void) { return frame_; }
		int assembleId(void) { return assemble_id_; }
		std::string blockId(void) { return block_info_.id(); }
		std::string thumbnail(void) { return block_info_.thumbnail(); }
		std::string blockName(void) { return block_info_.name(); }
		std::string xmlPath(void) { return xml_path_; }
		void setXmlPath(std::string xml_path) { xml_path_ = xml_path; }
		//void setTag(std::string tag) { tag_ = tag; }
		//void setShape(std::string shape) { shape_ = shape; }
		//void setDiffuse(std::string diffuse) { diffuse_ = diffuse; }
		//void setNormal(std::string normal) { normal_ = normal; }
		//void setThumbnail(std::string thumbnail) { thumbnail_ = thumbnail; }

		void setFrame(screw::SE3d frame);
		void setPosition(double x, double y, double z);
		void setQuaternion(double x, double y, double z, double w);

		void setAssembleId(int id);

		std::vector<osg::Geode*> jointMarkerGeodes(void);

		void update(void)
		{
//			if (block_)
//				block_->setTransformation(frame_);
		}



		// functions for contact point
		// cpid : contact point's ID
		// from block to contact point
		void getContactPointLocalFrame(unsigned int cpid, double& x, double& y, double& z, double& roll, double& pitch, double& yaw);
		screw::SE3d getContactPointLocalFrame(unsigned int cpid);
		romas::ContactPointType getContactPointType(unsigned int cpid);
		// from global to joint candiate
		screw::SE3d getContactPointFrame(unsigned int cpid);
		romas::AXIS getContactPointRotAxis(unsigned int cpid);
		romas::AXIS getContactPointZeroAxis(unsigned int cpid);


	private :
		int assemble_id_;
		BlockInfo block_info_;
		std::vector<romas::ContactPoint*> contact_points_;

		std::string xml_path_; // 경로
		osg::MatrixTransform *transform_;
		screw::SE3d frame_; // from global to block
	};
}

#endif
