#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <cmdline.hpp>
#include <cmds.hpp>

using json = nlohmann::json;
Cmdline cmdline("reddan","[RDN]∮");

void usage_err(std::string util) {
    std::cout << util << ": invalid usage";
    cmdline.getusage(util);
}

int main(int argc, char *argv[]) {	
	system("clear");
	cmdline.setcmd("set-target","set-target <host>","add a target to workspace",set_target);
	cmdline.setcmd("exit", "", "exit the program", exitprog);
	cmdline.sethelp("help", "display this help message");
	//cmdline.Cmds["exit"] = &exitprog;
	cmdline.read();
}
