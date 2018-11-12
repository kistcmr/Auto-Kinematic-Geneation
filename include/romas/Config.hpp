#ifndef __ROMAS_CONFIG__
#define __ROMAS_CONFIG__

#include <string>

namespace romas {


	class Config
	{
	public :
		Config();

		bool load(void);
		void save(void);

		std::string blockLibraryPath(void) { return block_library_path_; }
		std::string vcvarsallPath(void) { return vcvarsall_path_; }
		std::string msbuildPath(void) { return msbuild_path_; }
//		std::string neuromekaPath(void) { return neuromeka_path_; }

		void setBlockLibraryPath(std::string block_library_path) { block_library_path_ = block_library_path; }
		void setVcvarsallPath(std::string vcvarsall_path) { vcvarsall_path_ = vcvarsall_path; }
		void setMsbuildPath(std::string msbuild_path) { msbuild_path_ = msbuild_path; }
//		void setNeuromekaPath(std::string neuromeka_path) { neuromeka_path_ = neuromeka_path; }

	private :
		std::string file_name_;
		/// path to block_library folder
		std::string block_library_path_;

		/// path to vavarsall.bat file
		std::string vcvarsall_path_;
		/// path to MSBuild.exe file
		std::string msbuild_path_;
//		std::string neuromeka_path_;
	};
}

#endif