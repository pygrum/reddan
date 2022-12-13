#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <cmdline.hpp>
#include <cmds.hpp>
#include <nlohmann/json.hpp>


using json = nlohmann::json;
namespace fs = std::filesystem;

Cmdline cmdline("reddan","[RDN]$");
std::string projectname;

void usage_err(std::string util) {
    std::cout << util << ": invalid usage / parameters\n";
    cmdline.getusage(util);
}

void init(std::string project) {
    for (const auto & entry : fs::directory_iterator("config")){
		std::string conf{entry.path()};
		conf.erase(conf.length()-5);
		if (project == conf.substr(7,-1)){
			std::cerr << "error: a project with this name already exists\n";
			exit(1);
		}
	}
	json proj = json({});
	proj["name"] = project;
	proj["targets"] = json::array();
	std::string fname = "config/" + project + ".json";
	std::ofstream config(fname);
	config << std::setw(4) << proj << std::endl;
	projectname = project; 
}

void usage(const char *util){
	std::cout << "create a new project\n";
	std::cout << "	" << util << " new <projectname>\n\n";
	std::cout << "list all existing projects\n";
	std::cout << "	" << util << " ls\n\n";
	std::cout << "load an existing project\n";
	std::cout << "	" << util << " <projectname>\n\n";
	exit(0);
}

void ls(){
    for (const auto & entry : fs::directory_iterator("config")){
		std::string conf{entry.path()};
		conf.erase(conf.length()-5);
        std::cout << conf.substr(7,-1) << std::endl;
	}
	exit(0);
}

void load(std::string project){
	fs::create_directories("config");
    for (const auto & entry : fs::directory_iterator("config")){
		std::string conf{entry.path()};
		conf.erase(conf.length()-5);
        if (project == conf.substr(7,-1)) {
			projectname = project;
			return;
		}
	}
	std::cerr << "error: '" << project << "' does not match the name of any existing projects\n";
	exit(1);
}

int main(int argc, char *argv[]) {
	if (argc == 1){
		usage(argv[0]);
	}
	std::string cmd{argv[1]};
	if(argc == 3){
		if (cmd == "new"){
			init(std::string{argv[2]});
		}
		else{
			usage(argv[0]);
		}
	}
	else if (argc == 2){
		if (cmd == "ls"){
			ls();
		}
		else{
			load(argv[1]);
		}
	}
	else {
		usage(argv[0]);
	}
	std::ofstream o(".runtime");
	o << projectname;
	o.close();
	//system("clear");
	cmdline.setcmd("target-info","target-info <id>",
	"display information about specified target",info);
	cmdline.setcmd("targets","","list all targets in project",targets);
	cmdline.setcmd("add-target","add-target <name> <ip>",
	"add a target to project",add_target);
	cmdline.setcmd("rm-target","rm-target <id>",
	"remove a target from project",rm_target);
	cmdline.setcmd("exit", "", "exit the program", exitprog);
	cmdline.sethelp("help", "display this help message");
	cmdline.read();
}
