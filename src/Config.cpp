#include <romas/Config.hpp>
#include <romas/tinyxml2.hpp>

romas::Config::Config()
{
	file_name_ = ".\\romas.conf";
	block_library_path_ = ".\\block_library\\";
	vcvarsall_path_ = "C:\\Program Files (x86)\\Microsoft Visual Studio 10.0\\VC\\vcvarsall.bat";
	msbuild_path_ = "C:\\Windows\\Microsoft.NET\\Framework\\v4.0.30319\\MSBuild.exe";
//	neuromeka_path_ = "C:\\Program Files (x86)\\neuromeka";
}

bool romas::Config::load(void)
{
	using namespace tinyxml2;

	XMLDocument doc;

	doc.LoadFile(file_name_.c_str());

	if (doc.Error())
	{
		save();
		return true;
	}

	XMLElement* root_element = doc.FirstChildElement("Romas");

	XMLElement* config_element = root_element->FirstChildElement("Config");

	XMLElement *block_element = config_element->FirstChildElement("Block");

	XMLElement *block_library_element = block_element->FirstChildElement("Library");

	XMLElement *block_library_path_element = block_library_element->FirstChildElement("Path");

	block_library_path_ = romas::convert(block_library_path_element->GetText());

	XMLElement *compile_element = config_element->FirstChildElement("Compile");

	XMLElement *vcvarsall_element = compile_element->FirstChildElement("Vcvarsall");
	XMLElement *vcvarsall_path_element = vcvarsall_element->FirstChildElement("Path");

	vcvarsall_path_ = romas::convert(vcvarsall_path_element->GetText());

	XMLElement *msbuild_element = compile_element->FirstChildElement("Msbuild");
	XMLElement *msbuild_path_element = msbuild_element->FirstChildElement("Path");
	msbuild_path_ = romas::convert(msbuild_path_element->GetText());

	//XMLElement *neuromeka_element = compile_element->FirstChildElement("Neuromeka");
	//XMLElement *neuromeka_path_element = neuromeka_element->FirstChildElement("Path");
	//neuromeka_path_ = romas::convert(neuromeka_path_element->GetText());

	return true;
}


void romas::Config::save(void)
{
	using namespace tinyxml2;

	XMLDocument document;

	XMLNode *root_node = document.NewElement("Romas");
	document.InsertFirstChild(root_node);

	XMLElement *config_element = document.NewElement("Config");
	root_node->InsertFirstChild(config_element);

	XMLElement* block_element = document.NewElement("Block");

	XMLElement* block_library_element = document.NewElement("Library");
	block_element->InsertEndChild(block_library_element);

	XMLElement* block_library_path_element = document.NewElement("Path");
	block_library_element->InsertEndChild(block_library_path_element);

	block_library_path_element->SetText(block_library_path_.c_str());

	XMLElement* compile_element = document.NewElement("Compile");

	XMLElement* vcvarsall_element = document.NewElement("Vcvarsall");
	compile_element->InsertEndChild(vcvarsall_element);
	XMLElement* vcvarsall_path_element = document.NewElement("Path");
	vcvarsall_element->InsertEndChild(vcvarsall_path_element);

	vcvarsall_path_element->SetText(vcvarsall_path_.c_str());

	XMLElement* msbuild_element = document.NewElement("Msbuild");
	compile_element->InsertEndChild(msbuild_element);
	XMLElement* msbuild_path_element = document.NewElement("Path");
	msbuild_element->InsertEndChild(msbuild_path_element);

	msbuild_path_element->SetText(msbuild_path_.c_str());

	//XMLElement* neuromeka_element = document.NewElement("Neuromeka");
	//compile_element->InsertEndChild(neuromeka_element);
	//XMLElement* neuromeka_path_element = document.NewElement("Path");
	//neuromeka_element->InsertEndChild(neuromeka_path_element);

	//neuromeka_path_element->SetText(neuromeka_path_.c_str());

	config_element->InsertEndChild(block_element);
	config_element->InsertEndChild(compile_element);

	//	pElement->SetText(10);
	//	pRoot->InsertEndChild(pElement);
	//	pElement = xmlDoc.NewElement("FloatValue");
	//pElement->SetText(0.5f);

	//pRoot->InsertEndChild(pElement);
	//	pElement = xmlDoc.NewElement("Date");
	//pElement->SetAttribute("day", 26);
	//pElement->SetAttribute("month", "April");
	//pElement->SetAttribute("year", 2014);
	//pElement->SetAttribute("dateFormat", "26/04/2014");

	//pRoot->InsertEndChild(pElement);
	//	pElement = xmlDoc.NewElement("List");

	//	for (const auto & item : vecList)
	//	{
	//		XMLElement * pListElement = xmlDoc.NewElement("Item");
	//		pListElement->SetText(item);

	//		pElement->InsertEndChild(pListElement);
	//	}

	//	pElement->SetAttribute("itemCount", vecGroup.size());
	//	pRoot->InsertEndChild(pElement);


	document.SaveFile(file_name_.c_str());
}

