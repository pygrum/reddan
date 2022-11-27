#include <cmds.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <cmdline.hpp>
#include <nlohmann/json.hpp>
#include <regex>
#include <arpa/inet.h>

using json = nlohmann::json;


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

void add_target(ARGS args) {
    if (args.size() != 2) {
        usage_err("add-target");
        return;
    }
    if (!validIPAddress(args[1])){
        usage_err("add-target");
        return;
    }
    std::ifstream runtime(".runtime");
    std::string projectname{};
    std::string fname;
    while (runtime >> projectname){
        fname = "config/" + projectname + ".json";
    }
    runtime.close();
    json config;
    std::ifstream configfile(fname);
    configfile >> config;
    json target = json({});
    target["id"] = config["targets"].size();
    target["name"] = args[0];
    target["ip"] = args[1];
    config["targets"].push_back(target);
    configfile.close();
    std::ofstream o(fname);
    o << std::setw(4) << config << std::endl;
    o.close();
}

void rm_target(ARGS args){
    if (args.size() != 1){
        usage_err("rm-target");
        return;
    }
}