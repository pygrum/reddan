#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <cmdline.hpp>
#include <cmds.hpp>

using json = nlohmann::json;


int main(int argc, char *argv[]) {	
	system("clear");
	Cmdline cmdline("reddan","[RDN]∮");
	cmdline.setcmd("exit", "", "exit the program", exitprog);
	cmdline.sethelp("help", "display this help message");
	//cmdline.Cmds["exit"] = &exitprog;
	cmdline.read();
}
