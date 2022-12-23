#include <cstdio>
#include <utility>
#include <iostream>
#include <iomanip>
#include <string>
#include <tuple>
#include <vector>
#include <regex>
#include <readline/readline.h>
#include <readline/history.h>
#include <cmdline.hpp>

Cmdline::Cmdline(std::string nm, std::string pt)
{
	name = nm;
	prompt = pt + " ";
}

std::vector<std::string> Cmdline::tokenize(std::string cmd)
{
	std::vector<std::string> tokens;
	std::string word;
	for (std::string::size_type i = 0; i < cmd.size(); i++)
	{
		if (cmd[i] != ' ')
		{
			word = word + cmd[i];
		}
		else
		{
			tokens.push_back(word);
			word = "";
		}
	}
	tokens.push_back(word);
	return tokens;
}

std::vector<std::string> Cmdline::split(std::string cmd)
{
	std::vector<std::string> cmds;
	std::string expr;
	for (std::string::size_type i = 0; i < cmd.size(); i++)
	{
		if (cmd[i] != ';')
		{
			expr = expr + cmd[i];
		}
		else
		{
			cmds.push_back(expr);
			expr = "";
		}
	}
	cmds.push_back(expr);
	return cmds;
}

void Cmdline::setcmd(std::string op, std::string syntax,
					 std::string helpmsg, int (*_fn)(std::vector<std::string>))
{
	Cmds[op] = _fn;
	Help[op] = std::pair<std::string, std::string>(syntax, helpmsg);
}

void Cmdline::sethelp(std::string op, std::string helpmsg)
{
	help_op = op;
	help_msg = helpmsg;
}

int spacing = 30;

void Cmdline::getusage(std::string util)
{
	std::cout << std::setw(30) << std::left << Help[util].first
			  << std::setw(35) << std::left << Help[util].second << std::endl;
}

void Cmdline::help()
{
	for (auto const &[key, val] : Help)
	{
		std::cout << std::setw(15) << std::left << key
				  << std::setw(spacing) << std::left << val.first
				  << std::setw(spacing) << std::left << val.second << std::endl;
	}
	std::cout << std::setw(15) << std::left << help_op
			  << std::setw(spacing) << std::left << ""
			  << std::setw(spacing) << std::left << help_msg << std::endl;
}

int Cmdline::exec(std::vector<std::string> tokens)
{
	std::string op = tokens[0];
	tokens.erase(tokens.begin());
	typedef std::map<std::string, int (*)(std::vector<std::string>)> MAP;
	MAP::const_iterator it = Cmds.find(op);
	if (it == Cmds.end())
		return -1;
	else
	{
		return Cmds[op](tokens);
	}
}
std::tuple<int, std::string> Cmdline::accept(std::string cmd){
	/*Use directly to accept a command string without having to read from input*/
	for (auto i : split(cmd))
	{
		i = std::regex_replace(i, std::regex("^ +| +$|( ) +"), "$1");
		std::vector<std::string> tokens = tokenize(i);
		if (tokens[0] == "help"){
			help();
			return {0, "help"};
		}
		else {
			return {exec(tokens), tokens[0]};
		}
	}
	return {0, ""};
}

void Cmdline::read()
{
	char *line = NULL;
	while (line = readline(prompt.c_str()))
	{
		if (*line)
			add_history(line);
		std::string cmd{line};
		free(line);
		// remove trailing, leading and extra whitespaces
		auto [s, t] = accept(cmd);
		if ((s) < 0){
			std::cout << t << ": command not found\n";
		}
	}
}
