#include <romas/Assembly.hpp>
#include <romas/Joint.hpp>
#include <romas/tinyxml2.hpp>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <fstream>
#include <Poco/Process.h>
#include <stdlib.h>
#include <Poco/FileStream.h>
#include <Poco/File.h>
#include <Poco/TemporaryFile.h>
#include <Poco/String.h>
#include <windows.h>
#include <stdio.h>


romas::AssembleInfo::AssembleInfo()
{
	parent_rot_axis = romas::AXIS_PLUS_Z;
	child_rot_axis = romas::AXIS_PLUS_Z;
	parent_zero_axis = romas::AXIS_PLUS_X;
	child_zero_axis = romas::AXIS_PLUS_X;

	parent_matched_axis = romas::AXIS_PLUS_X;
	child_matched_axis = romas::AXIS_PLUS_X;
	joint_type = romas::JointType::Fixed;
	offset = 0;
	parent_contact_point_id = -1;
	child_contact_point_id = -1;
	parent_contact_point_type = romas::ContactPointType::Assembly;
	child_contact_point_type = romas::ContactPointType::Assembly;
}

romas::Assembly::Assembly()
	: scene_root_(nullptr) 
{


};

romas::Assembly::Assembly(osg::Group* scene_root) 
{ 
	scene_root_ = scene_root; 
}

romas::Assembly::~Assembly()
{
	if (scene_root_)
	{
		for (auto itr = blocks_.begin(); itr != blocks_.end(); itr++)
		{
			scene_root_->removeChild(*itr);
		}
	}
	else
	{
		for (auto itr = blocks_.begin(); itr != blocks_.end(); itr++)
		{
			delete *itr;
		}
		for (auto itr = joints_.begin(); itr != joints_.end(); itr++)
		{
			delete (romas::Joint*)*itr;
		}
	}
}

void romas::Assembly::save(std::string filename)
{
	using namespace tinyxml2;

	XMLDocument document;

	XMLNode *root_node = document.NewElement("Romas");
	document.InsertFirstChild(root_node);

	XMLElement *assembly_element = document.NewElement("Assembly");

	for (int i = 0; i < blocks_.size(); i++)
	{
		romas::Block *block = blocks_[i];
		romas::Joint *joint = joints_[i];

		romas::AssembleInfo assemble_info = joint->assembleInfo();

		XMLElement * part_element = document.NewElement("Part");

		part_element->SetAttribute("id", block->assembleId());
		part_element->SetAttribute("parentId", block->assembleId() - 1);
		part_element->SetAttribute("blockId", block->blockId().c_str());
		part_element->SetAttribute("parentContactPointIndex", assemble_info.parent_contact_point_id);
		part_element->SetAttribute("childContactPointIndex", assemble_info.child_contact_point_id);
		part_element->SetAttribute("parentMatchedAxis", romas::axis2string(assemble_info.parent_matched_axis).c_str());
		part_element->SetAttribute("childMatchedAxis", romas::axis2string(assemble_info.child_matched_axis).c_str());

		part_element->SetAttribute("jointType", romas::jointtype2string(assemble_info.joint_type).c_str());

		if (assemble_info.joint_type != romas::JointType::Fixed)
		{
			part_element->SetAttribute("offset", assemble_info.offset);
		}

		assembly_element->InsertEndChild(part_element);
	}

	root_node->InsertEndChild(assembly_element);

	document.SaveFile(filename.c_str());
}

bool romas::Assembly::load(std::string file_name)
{
	// 기존 삭제.

	tinyxml2::XMLDocument doc;

	doc.LoadFile(file_name.c_str());

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

	tinyxml2::XMLElement* assembly_element = romas_element->FirstChildElement("Assembly");

	if (!assembly_element)
	{
		//error
		return false;
	}

	tinyxml2::XMLElement* part_element;

	for (
		part_element = assembly_element->FirstChildElement("Part");
		part_element != nullptr;
		part_element = part_element->NextSiblingElement()
		)
	{
		loadPartElement(part_element);
	}

	return true;
}

bool romas::Assembly::loadPartElement(tinyxml2::XMLElement* part_element)
{
	if (!part_element)
		return false;

	int id = part_element->IntAttribute("id");
	int parent_id = part_element->IntAttribute("parentId");
	std::string blockId = romas::convert(part_element->Attribute("blockId"));

	//romas::BlockInfo block_info(id, name, is_base, is_tool);

	//tinyxml2::XMLElement *shapeElement = blockElement->FirstChildElement("shape");
	//block_info.setShape(romas::convert(shapeElement->Attribute("value")));

	//tinyxml2::XMLElement *diffuseElement = blockElement->FirstChildElement("diffuse");
	//block_info.setDiffuse(romas::convert(diffuseElement->Attribute("value")));

	//tinyxml2::XMLElement *normalElement = blockElement->FirstChildElement("normal");
	//block_info.setNormal(romas::convert(normalElement->Attribute("value")));

	return true;
}

void romas::Assembly::addBaseBlock(romas::Block* block)
{
	int assemble_id = (int)blocks_.size();
	block->setAssembleId(assemble_id);

	blocks_.push_back(block);
	if (scene_root_)
	{
		scene_root_->addChild(block);
	}

	romas::AssembleInfo assemble_info;

	romas::FixedJoint *joint = new romas::FixedJoint(assemble_info);

	joints_.push_back(joint);
}

void romas::Assembly::addJointAndBlock(romas::AssembleInfo assemble_info, romas::Block *block)
{
	if (blocks_.size() < 1)
		return;

	romas::Block *parent_block = blocks_.back();

	double x, y, z, roll, pitch, yaw;

	parent_block->getContactPointLocalFrame(assemble_info.parent_contact_point_id, x, y, z, roll, pitch, yaw);

	screw::SO3d R1 = screw::SO3d(screw::RotationType::ROLL_PITCH_YAW, roll, pitch, yaw);
	screw::SE3d parent_frame = screw::SE3d(R1, screw::vector3d(x, y, z));

	block->getContactPointLocalFrame(assemble_info.child_contact_point_id, x, y, z, roll, pitch, yaw);

	screw::SO3d R2 = screw::SO3d(screw::RotationType::ROLL_PITCH_YAW, roll, pitch, yaw);
	screw::SE3d child_frame = screw::SE3d(R2, screw::vector3d(x, y, z));

	screw::SE3d M(assemble_info.R);

	if (assemble_info.joint_type == romas::JointType::Revolute)
	{
		romas::RevoluteJoint* joint = new romas::RevoluteJoint(assemble_info);
		joint->setFrame(parent_frame, child_frame);

		if (assemble_info.parent_contact_point_type == romas::ContactPointType::Joint)
		{
			joint->setM(1, M);
		}
		else
		{
			joint->setM(0, M);
		}

		joints_.push_back(joint);
	}
	else if (assemble_info.joint_type == romas::JointType::Prismatic)
	{
		romas::PrismaticJoint* joint = new romas::PrismaticJoint(assemble_info);
		joint->setFrame(parent_frame, child_frame);

		if (assemble_info.parent_contact_point_type == romas::ContactPointType::Joint)
		{
			joint->setM(1, M);
		}
		else
		{
			joint->setM(0, M);
		}

		joints_.push_back(joint);

	}
	// Fixed joint
	else {
		romas::FixedJoint* joint = new romas::FixedJoint(assemble_info);
		joint->setFrame(parent_frame, child_frame);
		joint->setM(0, M);
		joints_.push_back(joint);
	}

	int assemble_id = (int)blocks_.size();
	block->setAssembleId(assemble_id);

	blocks_.push_back(block);
	if (scene_root_)
	{
		scene_root_->addChild(block);
	}
}

romas::Block* romas::Assembly::findBlock(int id)
{
	if ((id < 0) || id >= (int)blocks_.size())
	{
		return nullptr;
	}
	else
	{
		return blocks_[id];
	}
}

void romas::Assembly::generateHeaderFile(std::string directory, std::string robot_name, romas::Solver solver, std::string define)
{
//-------------pc version -----//
	std::string path1_str = directory + std::string("/") + robot_name + std::string("/");
	Poco::Path path1(path1_str);
	Poco::File file1(path1);
	file1.createDirectories();

//	msBuild.bat
	std::ofstream fout;
	fout.open(path1_str + "msBuild.bat");

	fout << "@echo off" << std::endl;
	//+std::string("\"set VisualStudioVersion=\" && ")
	fout << "set VisualStudioVersion=" << std::endl;
//	fout << "%comspec% /c \"" << vcvarsall_path_ << "\" x86 && \"" << msbuild_path_ << "\" \"" 
	fout << "\"" << vcvarsall_path_ << "\" x86 && \"" << msbuild_path_ << "\" \""
		<< directory << "\\" << robot_name << "\\" << robot_name << "\\" << robot_name << ".sln\"" << " /p:Configuration=Release\"" << std::endl;
//	fout << "copy \"" << release << "\\*.dll\" \"" << release << \"

	std::string path1_1_str = path1_str + robot_name + std::string("/");
	Poco::Path path1_1(path1_1_str);
	Poco::File file1_1(path1_1);
	file1_1.createDirectories();


	std::string path1_2_str = path1_str + std::string("/src/");
	Poco::Path path1_2(path1_2_str);
	Poco::File file1_2(path1_2);
	file1_2.createDirectories();

//	Poco::File robot_sln("./embeds/Simulator/Robot.sln");
//	robot_sln.copyTo(path1_1_str + robot_name + std::string(".sln"));

	//Poco::FileInputStream robot_sln_fis("./embeds/Simulator/Robot.sln");
	//std::string read;
	//robot_sln_fis >> read;
	//Poco::FileOutputStream robot_sln_fos(path1_1_str + robot_name + std::string(".sln"));
	//robot_sln_fos << read;
	//robot_sln_fos.close();

	generateNewSlnFile("./embeds/Simulator/Robot.sln", path1_1_str + robot_name + std::string(".sln"), robot_name);	

	std::string path1_1_1_str = path1_1_str + robot_name + std::string("/");
	Poco::Path path1_1_1(path1_1_1_str);
	Poco::File file1_1_1(path1_1_1);
	file1_1_1.createDirectories();

	generateNewVcxprojFile("./embeds/Simulator/Robot.vcxproj", path1_1_1_str + robot_name + std::string(".vcxproj"), robot_name);
	generateNewVcxprojUserFile("./embeds/Simulator/Robot.vcxproj.user", path1_1_1_str + robot_name + std::string(".vcxproj.user"));
	generateNewFile("./embeds/Simulator/SimRobot.cpp", path1_1_1_str + std::string("Sim") + robot_name + std::string(".cpp"), robot_name);

	std::string path1_1_2_str = path1_1_str + std::string("/Release/");
	Poco::Path path1_1_2(path1_1_2_str);
	Poco::File file1_1_2(path1_1_2);
	file1_1_2.createDirectories();

	// copyRelaeseDll
	Poco::File nrmk_helper_dll("./embeds/Release/NRMKHelper.dll");
	nrmk_helper_dll.copyTo(path1_1_2_str);

	Poco::File nrmk_helper_64_dll("./embeds/Release/NRMKHelper64.dll");
	nrmk_helper_64_dll.copyTo(path1_1_2_str);

	Poco::File poco_foundation_dll("./embeds/Release/PocoFoundation.dll");
	poco_foundation_dll.copyTo(path1_1_2_str);

	Poco::File poco_foundation_64_dll("./embeds/Release/PocoFoundation64.dll");
	poco_foundation_64_dll.copyTo(path1_1_2_str);

	Poco::File poco_net_dll("./embeds/Release/PocoNet.dll");
	poco_net_dll.copyTo(path1_1_2_str);

	Poco::File poco_net_64_dll("./embeds/Release/PocoNet64.dll");
	poco_net_64_dll.copyTo(path1_1_2_str);

	generateNewFile("./embeds/Simulator/DataSocketRobot.h", path1_2_str + std::string("DataSocket") + robot_name + std::string(".h"), robot_name);



	generateNewDefineRobotFile("./embeds/Simulator/DefineRobot.h", path1_2_str + std::string("Define") + robot_name + std::string(".h"), robot_name);



	generateNewFile("./embeds/Simulator/KIMMRobotControlSocket.h", path1_2_str + std::string("KIMMRobotControlSocket.h"), robot_name);
	generateNewNRMKFrameworkCppFile("./embeds/Simulator/NRMKFrameworkRobot.cpp", path1_2_str + std::string("NRMKFramework") + robot_name + std::string(".cpp"), robot_name);

	generateNewFile("./embeds/Simulator/NRMKFrameworkRobot.h", path1_2_str + std::string("NRMKFramework") + robot_name + std::string(".h"), robot_name);
	generateNewFile("./embeds/Simulator/SimulationSocketRobot.h", path1_2_str + std::string("SimulationSocket") + robot_name + std::string(".h"), robot_name);

	std::string path1_2_1_str = path1_2_str + std::string("/Control") + robot_name + std::string("/");
	Poco::Path path1_2_1(path1_2_1_str);
	Poco::File file1_2_1(path1_2_1);
	file1_2_1.createDirectories();

	generateNewFile("./embeds/Simulator/ControlRobot.cpp", path1_2_1_str + std::string("Control") + robot_name + std::string(".cpp"), robot_name);
	generateNewFile("./embeds/Simulator/ControlRobot.h", path1_2_1_str + std::string("Control") + robot_name + std::string(".h"), robot_name);
	generateNewFile("./embeds/Simulator/ControlRobotCmd.h", path1_2_1_str + std::string("Control") + robot_name + std::string("Cmd.h"), robot_name);

	std::vector<std::string> args;
//	args.push_back("/k");
//	args.push_back(path1_str + "msBuild.bat");
//	args.push_back("notepad.exe");

//	Poco::ProcessHandle ph = Poco::Process::launch("msBuild.bat", args, path1_str);
//	Poco::ProcessHandle ph = Poco::Process::launch("cmd.exe", args);
//	Poco::ProcessHandle ph = Poco::Process::launch("notepad.exe", args);
//	int a = ph.wait();

//	+std::string("\"set VisualStudioVersion=\" && ")
	std::string cmd = std::string("cmd.exe /k ") + path1_str + "msBuild.bat";
//	std::string cmd = path1_str + "msBuild.bat";
	system(cmd.c_str());


//	char szBuff[256];
//	DWORD dwRead = 0, dwOut = 0, dwErr = 0;
//	HANDLE hStdOutWrite = NULL, hStdOutRead = NULL;
//	HANDLE hStdErrWrite = NULL, hStdErrRead = NULL;
//	STARTUPINFO si;
//	SECURITY_ATTRIBUTES sa;
//	PROCESS_INFORMATION pi;
//
//	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//	sa.lpSecurityDescriptor = NULL;
//	sa.bInheritHandle = TRUE;
//
//	CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0);  /* 실행될 콘솔 프로그램에 넘길 stdout */
//	CreatePipe(&hStdErrRead, &hStdErrWrite, &sa, 0);  /* 실행될 콘솔 프로그램에 넘길 stderr */
//
//	ZeroMemory(&si, sizeof(STARTUPINFO));
//	si.cb = sizeof(STARTUPINFO);
//	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
//	si.hStdOutput = hStdOutWrite;
//	si.hStdInput = NULL;
//	si.hStdError = hStdErrWrite;
//	si.wShowWindow = SW_HIDE;       /* 눈에 보이지 않는 상태로 프로세스 시작 */
//
//#define APP "C:\\Windows\\system32\\ipconfig.exe"
//
//	CreateProcess(NULL, APP, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
//	CloseHandle(pi.hThread);
//
//	while (PeekNamedPipe(hStdOutRead, NULL, 0, NULL, &dwOut, NULL) ||
//		PeekNamedPipe(hStdErrRead, NULL, 0, NULL, &dwErr, NULL))  /* 읽어들일 데이터가 있는가? */
//	{
//		if (dwOut <= 0 && dwErr <= 0 && WaitForSingleObject(pi.hProcess, 0) != WAIT_TIMEOUT)
//			break;  /* 실행되어진 콘솔 응용프로그램이 종료된 경우 */
//
//		while (PeekNamedPipe(hStdOutRead, NULL, 0, NULL, &dwOut, NULL) && dwOut > 0)
//		{
//			ReadFile(hStdOutRead, szBuff, sizeof(szBuff), &dwRead, NULL);
//			szBuff[dwRead] = 0;
//			printf("%s", szBuff);
//		}
//
//		while (PeekNamedPipe(hStdErrRead, NULL, 0, NULL, &dwErr, NULL) && dwErr > 0)
//		{
//			ReadFile(hStdErrRead, szBuff, sizeof(szBuff), &dwRead, NULL);
//			szBuff[dwRead] = 0;
//			printf("%s", szBuff);
//		}
//	}
//
//	CloseHandle(pi.hProcess);
//	CloseHandle(hStdOutRead);
//	CloseHandle(hStdOutWrite);
//	CloseHandle(hStdErrRead);
//	CloseHandle(hStdErrWrite);
}

void romas::Assembly::generateNewFile(std::string in_file, std::string out_file, std::string robot_name)
{
	std::ifstream robot_sln_if(in_file);
	std::stringstream buffer;
	buffer << robot_sln_if.rdbuf();
	std::string read = buffer.str();
	std::string write = replace(read, robot_name);
	std::ofstream robot_sln_of(out_file);
	robot_sln_of << write;
	robot_sln_if.close();
	robot_sln_of.close();
}

void romas::Assembly::generateNewSlnFile(std::string in_file, std::string out_file, std::string robot_name)
{
	std::ifstream robot_sln_if(in_file);
	std::stringstream buffer;
	buffer << robot_sln_if.rdbuf();
	std::string read = buffer.str();

	std::string temp1 = Poco::replace(read, "NRMK_VS_FORMAT_VER", "11.00");
	std::string temp2 = Poco::replace(temp1, "NRMK_ROBOT_NAME", robot_name.c_str());
	std::string write = Poco::replace(temp2, "NRMK_VS_VER", "2010");

	std::ofstream robot_sln_of(out_file);
	robot_sln_of << write;
	robot_sln_if.close();
	robot_sln_of.close();
}

void romas::Assembly::generateNewVcxprojFile(std::string in_file, std::string out_file, std::string robot_name)
{
	std::ifstream robot_sln_if(in_file);
	std::stringstream buffer;
	buffer << robot_sln_if.rdbuf();
	std::string read = buffer.str();

//	std::string temp1 = Poco::replace(read, "NRMK_VS_FORMAT_VER", "11.00");
	std::string temp2 = Poco::replace(read, "NRMK_ROBOT_NAME", robot_name.c_str());
	std::string temp3 = Poco::replace(temp2, "NRMK_VS_VER", "v100");
	std::string temp4 = Poco::replace(temp3, "NRMK_TARGET_FRAMEWORK_VER", "v4.0");
	std::string write = Poco::replace(temp4, "	NRMK_ROBOT_KIN_CONTROL", "KIN_CONTROL");

	std::ofstream robot_sln_of(out_file);
	robot_sln_of << write;
	robot_sln_if.close();
	robot_sln_of.close();
}

void romas::Assembly::generateNewVcxprojUserFile(std::string in_file, std::string out_file)
{
	std::ifstream robot_sln_if(in_file);
	std::stringstream buffer;
	buffer << robot_sln_if.rdbuf();
	std::string read = buffer.str();

//	std::string temp1 = Poco::replace(read, "NRMK_VS_FORMAT_VER", "11.00");
//	std::string temp2 = Poco::replace(temp1, "NRMK_ROBOT_NAME", robot_name.c_str());
	std::string write = Poco::replace(read, "NRMK_VS_VER", "vs2010");
//	std::string write = Poco::replace(temp3, "NRMK_TARGET_FRAMEWORK_VER", "v4.0");

	std::ofstream robot_sln_of(out_file);
	robot_sln_of << write;
	robot_sln_if.close();
	robot_sln_of.close();
}

void romas::Assembly::generateNewDefineRobotFile(std::string in_file, std::string out_file, std::string robot_name)
{
	std::ifstream robot_sln_if(in_file);
	std::stringstream buffer;
	buffer << robot_sln_if.rdbuf();
	std::string read = buffer.str();

	// NRMK_NUMBER_OF_BODIES
	std::string number_of_bodies = std::to_string(blocks_.size() + 1);
	std::string temp1 = Poco::replace(read, "NRMK_NUMBER_OF_BODIES", std::to_string(6 + 1).c_str());

	// NRMK_ROBOT_NAME
	std::string temp2 = Poco::replace(temp1, "NRMK_ROBOT_NAME", robot_name.c_str());

	// NRMK_REF_INDEX
	std::string temp3 = Poco::replace(temp2, "NRMK_REF_INDEX", std::to_string(-1).c_str());

	// NRMK_TARGET_INDEX
	std::string target_index = std::to_string(blocks_.size());
	std::string temp4 = Poco::replace(temp3, "NRMK_TARGET_INDEX", std::to_string(6).c_str());

	// NRMK_TARGET_ROTATION
	std::string target_rotation = "1,0,0,0,1,0,0,0,1";
	std::string temp5 = Poco::replace(temp4, "NRMK_TARGET_ROTATION", target_rotation.c_str());

	// NRMK_TARGET_POS
	std::string target_pos = "0,0,0";
	std::string temp6 = Poco::replace(temp5, "NRMK_TARGET_POS", target_pos.c_str());

	// NRMK_REF_ROTATION
	std::string ref_rotation = "1,0,0,0,1,0,0,0,1";
	std::string temp7 = Poco::replace(temp6, "NRMK_REF_ROTATION", ref_rotation.c_str());

	// NRMK_REF_POS
	std::string ref_pos = "0,0,0";
	std::string temp8 = Poco::replace(temp7, "NRMK_REF_POS", ref_pos.c_str());

	// NRMK_ADD_OFFSET
	std::string add_offset = "Offset[0].set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.1000));\nOffset[1].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.5000, 0.0000, 0.0000));\nOffset[2].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.3000, 0.0000, -0.2500));\nOffset[3].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.5000, 0.0000, 0.0000));\nOffset[4].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.5000, 0.0000, 0.0000));\nOffset[5].set(Rotation(1, 0, 0, 0, 1, 0, 0, 0, 1), Displacement(-0.3000, 0.0000, -0.2500));";
	std::string temp9 = Poco::replace(temp8, "NRMK_ADD_OFFSET", add_offset.c_str());

	// NRMK_SET_JOINT
	std::string set_joint = "_joint_0.setJointFrame(Offset[0]);\n_joint_1.setJointFrame(Offset[1]);\n_joint_2.setJointFrame(Offset[2]);\n_joint_3.setJointFrame(Offset[3]);\n_joint_4.setJointFrame(Offset[4]);\n_joint_5.setJointFrame(Offset[5]);";
	std::string temp10 = Poco::replace(temp9, "NRMK_SET_JOINT", add_offset.c_str());

	// NRMK_ROBOT_DAMPING
	std::string temp11 = Poco::replace(temp10, "NRMK_ROBOT_DAMPING", "");

	// NRMK_CONNECT
	std::string connect = "connect(0, 1, _joint_0);\nconnect(1, 2, _joint_1);\nconnect(2, 3, _joint_2);\nconnect(3, 4, _joint_3);\nconnect(4, 5, _joint_4);\nconnect(5, 6, _joint_5);";
	std::string temp12 = Poco::replace(temp11, "NRMK_CONNECT", connect.c_str());

	// NRMK_INERTIA
	std::string inertia = "inertia[0].set(6.95, 0, 0, 0.04, 0.02, 0.02, 0.03, 0, 0, 0);\ninertiaTemp.set(21.99, 0, 0, 0.25, 0.49, 0.49, 0.06, 0, 0, 0);\nmodelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));\ninertia[1].addInertia(inertiaTemp, modelingFrame);\ninertiaTemp.set(24.663, -0.051, 0, 0.223, 0.298, 0.449, 0.216, 0, 0, -0.097);\nmodelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));\ninertia[2].addInertia(inertiaTemp, modelingFrame);\ninertiaTemp.set(21.99, 0, 0, 0.25, 0.49, 0.49, 0.06, 0, 0, 0);\nmodelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));\ninertia[3].addInertia(inertiaTemp, modelingFrame);\ninertiaTemp.set(21.99, 0, 0, 0.25, 0.49, 0.49, 0.06, 0, 0, 0);\nmodelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));\ninertia[4].addInertia(inertiaTemp, modelingFrame);\ninertiaTemp.set(24.663, -0.051, 0, 0.223, 0.298, 0.449, 0.216, 0, 0, -0.097);\nmodelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));\ninertia[5].addInertia(inertiaTemp, modelingFrame);\ninertiaTemp.set(2.12, 0, 0, 0.03, 0, 0, 0.01, 0, 0, 0);\nmodelingFrame.set(Rotation(0, 0, 1, 0, 1, 0, -1, 0, 0), Displacement(0.0000, 0.0000, 0.0000));\ninertia[6].addInertia(inertiaTemp, modelingFrame);";
	std::string temp13 = Poco::replace(temp12, "NRMK_INERTIA", inertia.c_str());

	// NRMK_KINEMATIC_JOINT=blank
	std::string temp14 = Poco::replace(temp13, "NRMK_KINEMATIC_JOINT", "");

	// NRMK_INIT_JOINT_VAR

	std::string init_joint_var = "NRMKFoundation::RevoluteJoint _joint_0;\nNRMKFoundation::RevoluteJoint _joint_1;\nNRMKFoundation::RevoluteJoint _joint_2;\nNRMKFoundation::RevoluteJoint _joint_3;\nNRMKFoundation::RevoluteJoint _joint_4;\nNRMKFoundation::RevoluteJoint _joint_5;";
	std::string write = Poco::replace(temp14, "NRMK_INIT_JOINT_VAR", init_joint_var.c_str());

	std::ofstream robot_sln_of(out_file);
	robot_sln_of << write;
	robot_sln_if.close();
	robot_sln_of.close();
}

void romas::Assembly::generateNewNRMKFrameworkCppFile(std::string in_file, std::string out_file, std::string robot_name)
{
	std::ifstream robot_sln_if(in_file);
	std::stringstream buffer;
	buffer << robot_sln_if.rdbuf();
	std::string read = buffer.str();

	// NRMK_BODY_INDEX
	std::string body_index = "0,1,2,3,4,5,6";
	std::string temp1 = Poco::replace(read, "NRMK_BODY_INDEX", body_index.c_str());

	// NRMK_DATA_INDEX0
	std::string data_index0 = "0,1,2,3,4,5,";
	std::string temp2 = Poco::replace(temp1, "NRMK_DATA_INDEX0", data_index0.c_str());

	// NRMK_DATA_INDEX1
	std::string data_index1 = "6,7,8,9,10,11,";
	std::string temp3 = Poco::replace(temp2, "NRMK_DATA_INDEX1", data_index1.c_str());

	// NRMK_DATA_INDEX2
	std::string data_index2 = "12,13,14,15,16,17,";
	std::string temp4 = Poco::replace(temp3, "NRMK_DATA_INDEX2", data_index2.c_str());

	// NRMK_ROBOT_NAME
	std::string write = Poco::replace(temp4, "NRMK_ROBOT_NAME", robot_name.c_str());

	std::ofstream robot_sln_of(out_file);

	robot_sln_of << write;
	robot_sln_if.close();
	robot_sln_of.close();
}

void romas::Assembly::forwardKinematics(void)
{
	if (blocks_.size() < 1)
		return;

	screw::SE3d T = blocks_.front()->frame();
	screw::SE3d T1;

	for (int i = 1; i < blocks_.size(); i++)
	{
		T1 = T * joints_[i]->transition();
		blocks_[i]->setFrame(T1);
		T = T1;
	}
}

std::string romas::Assembly::replace(std::string& in, std::string& robot_name)
{
	std::string temp1 = Poco::replace(in, "NRMK_VS_FORMAT_VER", "11.00");
	std::string temp2 = Poco::replace(temp1, "NRMK_ROBOT_NAME", robot_name.c_str());
	std::string temp3 = Poco::replace(temp2, "NRMK_VS_VER", "2010");
	std::string temp4 = Poco::replace(temp3, "NRMK_TARGET_FRAMEWORK_VER", "v4.0");

	return temp4;
}