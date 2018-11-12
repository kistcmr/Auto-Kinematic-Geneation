#include "CLI/CLI.hpp"
#include <iostream>

int main(int argc, char **argv) 
{
    CLI::App app("romas");

	std::string assembly;
	CLI::Option *opt_assembly = app.add_option("-a,--ass,ass", assembly, "Assembly File Name");

    std::string robot_name;
    CLI::Option *opt_robot_name = app.add_option("-n,--name,name", robot_name, "Robot Name");

	std::string op_env("win");
	// win, linux, both
	CLI::Option *opt_env = app.add_option("-e,--env,env", op_env, "Environment (win|linux|both)");

	std::string op_folder;

	CLI::Option *opt_folder = app.add_option("-f,--fol,fol", op_folder, "Folder to be Saved");

	// control
	std::string op_algorithm;
	// kin, dyn
	CLI::Option *opt_algorithm = app.add_option("-c,--con,con", op_algorithm, "Control Algorithm (kin|dyn)");

	std::string define;
	CLI::Option *opt_define = app.add_option("-d,--def,def", define, "User Define");

    try {
        app.parse(argc, argv);
    } catch(const CLI::Error &e) {
        return app.exit(e);
    }

	std::string env;

	if (op_env == "win")
		env = "windows";
	else
		env = op_env;

	std::string folder;

	if (op_folder.empty())
		folder = std::string("./") + robot_name;

	std::string algorithm;

	if (op_algorithm == std::string("kin"))
		algorithm = "kinematics";
	else
		algorithm = "dynamics";

	std::cout << "----------------------------------" << std::endl;
	std::cout << "Romas (Robot Module ASsembler) " << std::endl;
	std::cout << "----------------------------------" << std::endl;
	std::cout << "Options" << std::endl;
	std::cout << "----------------------------------" << std::endl;
	std::cout << " Assembly: " << assembly << std::endl;
	std::cout << " Robot Name: " << robot_name << std::endl;
	std::cout << " Environment: " << env << std::endl;
	std::cout << " Saved Folder: " << folder << std::endl;
	std::cout << " Algorithm: " << algorithm << std::endl;
	std::cout << " Define: " << define << std::endl;
	std::cout << "----------------------------------" << std::endl;
	std::cout << "Generating Code......" << std::endl;
	std::cout << "----------------------------------" << std::endl;
	std::cout << "Build Code......" << std::endl;
	std::cout << "----------------------------------" << std::endl;
	std::cout << "Completed!" << std::endl;
	std::cout << "----------------------------------" << std::endl;

    return 0;
}