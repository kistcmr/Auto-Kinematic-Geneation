#ifndef _ROMAS_ASSEMBLY_
#define _ROMAS_ASSEMBLY_

#include <romas/romas.hpp>

#include <vector>
#include <romas/Block.hpp>
#include <osg/Group>
#include <screw/Lie.hpp>
#include <romas/tinyxml2.hpp>

namespace romas
{
	struct AssembleInfo
	{
		AssembleInfo();

		int parent_contact_point_id;
		int child_contact_point_id;
		romas::ContactPointType parent_contact_point_type;
		romas::ContactPointType child_contact_point_type;
		romas::AXIS parent_rot_axis;
		romas::AXIS child_rot_axis;
		romas::AXIS parent_matched_axis;
		romas::AXIS child_matched_axis;
		romas::AXIS parent_zero_axis;
		romas::AXIS child_zero_axis;
		romas::JointType joint_type;
		double offset;
		screw::SO3d R;
	};

	class Assembly
	{
	public :
		Assembly();
		Assembly(osg::Group* scene_root);
		~Assembly();

		void save(std::string filename);
		bool load(std::string file_name);
		bool loadPartElement(tinyxml2::XMLElement* part_element);
		void addBaseBlock(romas::Block* block);
		void addJointAndBlock(romas::AssembleInfo assemble_info, romas::Block *block);

		void generateHeaderFile(std::string directory, std::string robot_name, romas::Solver solver, std::string define);

		void generateNewSlnFile(std::string in_file, std::string out_file, std::string robot_name);
		void generateNewVcxprojFile(std::string in_file, std::string out_file, std::string robot_name);
		void generateNewVcxprojUserFile(std::string in_file, std::string out_file);
		void generateNewDefineRobotFile(std::string in_file, std::string out_file, std::string robot_name);
		void generateNewNRMKFrameworkCppFile(std::string in_file, std::string out_file, std::string robot_name);
		void generateNewFile(std::string in_file, std::string out_file, std::string robot_name);
		std::string replace(std::string& in, std::string& robot_name);

		romas::Block* findBlock(int id);

		void forwardKinematics(void);

		void setBlockLibraryPath(std::string block_library_path) { block_library_path_ = block_library_path; }
		void setVcvarsPath(std::string vcvarsall_path) { vcvarsall_path_ = vcvarsall_path; }
		void setMsbuildPath(std::string msbuild_path) { msbuild_path_ = msbuild_path; }

	private :
		std::vector<romas::Block*> blocks_;
		std::vector<romas::Joint*> joints_;
		osg::Group* scene_root_;
		std::string block_library_path_;
		std::string vcvarsall_path_;
		std::string msbuild_path_;
	};
}


#endif








