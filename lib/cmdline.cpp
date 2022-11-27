#include <cstdio>
#include <utility>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <regex>
#include <readline/readline.h>
#include <readline/history.h>
#include <cmdline.hpp>

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



void Cmdline::setcmd(std::string op, std::string syntax, 
std::string helpmsg, void (*_fn)(std::vector<std::string>)) {
	Cmds[op] = _fn;
	Help[op] = std::pair<std::string, std::string> (syntax, helpmsg);
}

void Cmdline::sethelp(std::string op, std::string helpmsg) {
	help_op = op;
	help_msg = helpmsg;
}

void Cmdline::help() {
	for (auto const& [key, val] : Help) {
		std::cout << std::setw(15) << std::left << key
		<< std::setw(15) << std::left << val.first
		<< std::setw(15) << std::left << val.second << std::endl;		
	}
	std::cout << std::setw(15) << std::left << help_op
	<< std::setw(15) << std::left << ""
	<< std::setw(15) << std::left << help_msg << std::endl;
}

void Cmdline::exec(std::vector<std::string> tokens) {
    std::string op = tokens[0];
    tokens.erase(tokens.begin());
    typedef std::map<std::string, void (*)(std::vector<std::string>)> MAP;
    MAP::const_iterator it = Cmds.find(op);
    if (it == Cmds.end())
        std::cout << op << ": command not found\n";
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
		if (tokens[0] == "help")
			help();
		else
        	exec(tokens);
    }
}

