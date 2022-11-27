#include <vector>
#include <map>
#include <string>

class Cmdline {
    public:
        Cmdline(std::string, std::string);
        void read();
        void exec(std::vector<std::string>);
        std::map<std::string, void (*)(std::vector<std::string>)> Cmds;
        std::vector<std::string> tokenize(std::string);
    private:
        std::string name;
        std::string prompt;
};

