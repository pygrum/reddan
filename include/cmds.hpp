#include <vector>
#include <string>
#include <nlohmann/json.hpp>

typedef std::vector<std::string> ARGS; 

void usage_err(std::string);

void exitprog(ARGS);

void add_target(ARGS);

void rm_target(ARGS);

void info(ARGS);

void targets(ARGS);

void new_beacon(ARGS);

void r_exec(ARGS);

nlohmann::json getConfig();
void setConfig(nlohmann::json &config);