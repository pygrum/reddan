#include <vector>
#include <string>
#include <tuple>
#include <nlohmann/json.hpp>

typedef std::vector<std::string> ARGS; 

void usage_err(std::string);

int exitprog(ARGS);

int add_target(ARGS);

int rm_target(ARGS);

int info(ARGS);

int targets(ARGS);

int new_beacon(ARGS);

int r_exec(ARGS);

nlohmann::json *getConfig();
void setConfig(nlohmann::json &config);

int compile(ARGS);

std::string get_binary(std::string cmdline_util);

std::tuple<int, int, const char*> get_beacon_data();