#include <vector>
#include <map>
#include <string>
#include <utility>

typedef std::vector<std::string> ARGS;

class Cmdline {
    public:
        Cmdline(std::string, std::string);
        void read();
        void sethelp(std::string, std::string);
        void setcmd(std::string, std::string, std::string, void (*_)(ARGS));
        void exec(ARGS);
        ARGS tokenize(std::string);
    private:
        void help();

        std::map<std::string, void (*)(ARGS)> Cmds;
        std::map<std::string, std::pair<std::string, std::string>> Help;
        std::string help_op;
        std::string help_msg;
        std::string name;
        std::string prompt;
};

