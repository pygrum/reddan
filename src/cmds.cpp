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
    json beacon = {
        {"id",config["targets"].size()},
        {"alive",false},
        {"persistent",false},
        {"port",-1}
    };
    target["beacon"] = beacon;
    config["targets"].push_back(target);
    setConfig(config);
    log("success");
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
    int id{};
    try {
        id = std::stoi(args[0]);
    }
    catch (...) {
        std::cerr << "error: invalid target id\n";
        return;
    }
    json config = getConfig();
    if (id+1 > config["targets"].size()){
        std::cerr << "error: target does not exist\n";
        return;
    }
    std::vector properties = { "id", "name", "ip", "uri" };
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
    int id{};
    try {
        id = std::stoi(args[0]);
    }
    catch (...) {
        std::cerr << "error: invalid target id\n";
        return;
    }
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
