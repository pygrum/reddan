#include <cmds.hpp>
#include <string>
#include <iostream>
#include <cmdline.hpp>

void exitprog(ARGS) {
    std::cout << "Goodbye\n";
    exit(0);
}


void set_target(ARGS args) {
    if (args.size() != 1)
        usage_err("set-target");
    std::cout << "Target set\n";
}