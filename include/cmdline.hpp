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
        void setcmd(std::string, std::string, std::string, int (*_)(ARGS));
        int exec(ARGS);
        void getusage(std::string);
        int get_exit_code();
        void accept(std::string, bool);
    private:
        void handle(int status, std::string operand);
        void help();
        int exit_code;
        ARGS tokenize(std::string);
        ARGS split(std::string);
        std::map<std::string, int (*)(ARGS)> Cmds;
        std::map<std::string, std::pair<std::string, std::string>> Help;
        std::string help_op;
        std::string help_msg;
        std::string name;
        std::string prompt;
};

