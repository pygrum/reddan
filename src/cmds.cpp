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

void exitprog(ARGS) {
    std::cout << "removing .runtime\n";
    const int result = remove(".runtime");
    if (result == 0){
        std::cout << "success\n";
    }
    else {
        std::cout << "failed to remove .runtime, quitting anyway\n";
    }
    std::cout << "Goodbye\n";
    exit(0);
}

bool validIPAddress(const std::string &IP) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, IP.c_str(), &(sa.sin_addr));
    return result != 0;
}

std::string getconfigname(){
    std::ifstream runtime(".runtime");
    std::string projectname{};
    std::string fname;
    while (runtime >> projectname){
        fname = "config/" + projectname + ".json";
    }
    runtime.close();
    return fname;
}

json getConfig() {
    std::string fname = getconfigname();
    json config;
    std::ifstream configfile(fname);
    configfile >> config;
    configfile.close();
    return config;
}

void setConfig(json &config){
    std::ofstream o(getconfigname());
    o << std::setw(4) << config << std::endl;
    o.close();
}

void add_target(ARGS args) {
    if (args.size() != 2) {
        usage_err("add-target");
        return;
    }
    log("Validating parameters...");
    if (!validIPAddress(args[1])){
        std::cerr << "error: IP address is invalid";
        return;
    }
    json config = getConfig();
    json target = json({});
    log("creating entry...");
    target["id"] = config["targets"].size();
    target["name"] = args[0];
    target["ip"] = args[1];
    target["uris"] = json::array();
    config["targets"].push_back(target);
    setConfig(config);
    log("success");
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

void new_beacon(ARGS args){
    if (args.size() != 2) {
        usage_err("new-beacon");
        return;
    }
    int port{};
    try {
        port = std::stoi(args[1]);
    }
    catch (...) {
        std::cerr << "error: invalid port number\n";
        return;
    }
    json config = getConfig();
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
}

void targets(ARGS args){
    if (args.size() != 0){
        usage_err("targets");
        return;   
    }
    json config = getConfig();
    for (auto &i : config["targets"]){
        std::stringstream ss;
        ss << "target " << i["id"] << ":";
        std::string tgnum = ss.str();
        std::cout << std::setw(15) << std::left
        << tgnum << std::setw(15) << std::left << std::string{i["name"]};
        std::cout << std::setw(15) << std::left << std::string{i["ip"]} << std::endl;
    }
}

void info(ARGS args){
    if (args.size() != 1){
        usage_err("target-info");
        return;
    }
    json config = getConfig();
    if (!validTarget(args[0], config["targets"].size())){
        usage_err("target-info");
        return;
    }
    int id = std::stoi(args[0]);
    if (id+1 > config["targets"].size()){
        std::cerr << "error: target does not exist\n";
        return;
    }
    std::vector properties = { "id", "name", "ip", "uris", "beacon" };
    for ( auto &pt : properties) {
        std::string prop = pt;
        std::string propUpper = pt;
        std::transform(propUpper.begin(), propUpper.end(),propUpper.begin(), ::toupper);
        std::cout << std::setw(15) << std::left << propUpper << ": "
        << std::setw(15) << std::left << config["targets"][id][prop] << std::endl;
    }
}

void rm_target(ARGS args){
    if (args.size() != 1){
        usage_err("rm-target");
        return;
    }
    json config = getConfig();
    if (!validTarget(args[0], config["targets"].size())){
        usage_err("rm-target");
        return;
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
}

void compile(ARGS args){
    if (args.size() != 2){
        usage_err("compile");
        return;
    }
    json config = getConfig();
    if (!validTarget(args[0], config["targets"].size())){
        usage_err("compile");
        return;
    }
    std::string id = args[0];
    int id_int = std::stoi(id);
    int port;
    std::string ip_addr;

    port = config["targets"][id_int]["beacon"]["port"];
    ip_addr = config["targets"][id_int]["ip"];

    std::string port_str = std::to_string(port);

    std::string id_st = "int beacon_id = " + id + ';';
    std::string port_st = "int beacon_port = " + port_str + ';';
    std::string ip_addr_st = "const char *beacon_ip_addr = \"" + ip_addr + "\";";
    std::ofstream varfile("remote/beacon.vars");

    varfile << id_st << std::endl;
    varfile << port_st << std::endl;
    varfile << ip_addr_st << std::endl;

    system("echo \"binary to be saved to $PWD/beacon\"");
    std::string compiler = args[1];
    std::string cmdexec = compiler + " remote/*.cpp libs/*.cpp -I include/ -I remote/ -o $PWD/beacon -lreadline";
    system(cmdexec.c_str());
    const int result = remove("remote/beacon.vars");
    if (result != 0)
        std::cout << "Failed to remove variable file (remote/beacon.vars)\n";
}

void r_exec(ARGS args){
    if (args.size() != 1){
        usage_err("r-exec");
        return;
    }
    json config = getConfig();
    if (!validTarget(args[0], config["targets"].size())){
        usage_err("r-exec");
        return;
    }
    json upd;
    std::string cmd;
    std::cout << "enter 'exit' to stop\n";
    std::string exit{"exit"};
    while (true){
        std::cout << "[" << args[0] << " $] ";
        std::getline(std::cin, cmd);
        if (cmd == "exit"){
            return;
        }
        std::string k;
        upd["cmd"] = cmd;
        std::stringstream ss;
        ss << upd;
        k = ss.str();
        communicate(std::stoi(args[0]), k);
    }
}