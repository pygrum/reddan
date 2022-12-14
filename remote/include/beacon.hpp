#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Beacon {
    public:
        Beacon(int id, int port, const char* attached_to);
        bool alive;
        bool persistent;
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

int beacon_id;
int beacon_port;
const char *beacon_ip_addr;



