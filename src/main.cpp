#include <fstream>
#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <readline/readline.h>
#include <readline/history.h>
#include <cmdline.hpp>

using json = nlohmann::json;

void echo(std::vector<std::string> vec){
	std::string empty = "";
	for (auto i : vec) {
		std::cout << i << "\n";
	}
}

int main(int argc, char *argv[]) {	
	system("clear");
	Cmdline cmdline("reddan","[RDN]∮");
	cmdline.Cmds["echo"] = &echo;
	cmdline.read();
}
