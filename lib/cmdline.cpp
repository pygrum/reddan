#include <cmdline.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <readline/readline.h>
#include <readline/history.h>

Cmdline::Cmdline(std::string nm, std::string pt) {
    name = nm;
    prompt = pt + " ";
}

std::vector<std::string> Cmdline::tokenize(std::string cmd) {
	std::vector<std::string> tokens;
	std::string word;
	for (std::string::size_type i = 0; i < cmd.size(); i++) {	
		if (cmd[i] != ' ') {
			word = word + cmd[i];
		}
		else {
			tokens.push_back(word);
			word = "";
		}
	}
	tokens.push_back(word);
	return tokens;
}

void Cmdline::exec(std::vector<std::string> tokens) {
    std::string op = tokens[0];
    tokens.erase(tokens.begin());
    typedef std::map<std::string, void (*)(std::vector<std::string>)> MAP;
    MAP::const_iterator it = Cmds.find(op);
    if (it == Cmds.end())
        std::cout << name << ": " << op << ": Command not found\n";
    else {
        Cmds[op](tokens);
    }
//    Cmds[op](tokens);
}

void Cmdline::read() {
	char* line = NULL;
	while (line = readline(prompt.c_str())){
		if (*line) add_history(line);
		std::string cmd{line};
		free(line);
        //remove trailing, leading and extra whitespaces
		cmd = std::regex_replace(cmd, std::regex("^ +| +$|( ) +"), "$1"); 
        std::vector<std::string> tokens = tokenize(cmd);
        exec(tokens);
    }
}

