#ifndef ARGUMENTSMANAGER_H_
#define ARGUMENTSMANAGER_H_


#include <string.h>
#include <iostream>
#include <string>
#include "arpa/inet.h"

class ArgumentsManager{
private:
public:
    std::string ip = "";
    int flag;
    std::string lw_brd, upr_brd;
    ArgumentsManager(int argc, char* argv[]);
    int parse_args(int argc, char* argv[]);
    std::string input_low_port();
    std::string input_up_port(std::string low_port);
    std::string input_IP();
    void help();
};

#endif /* ARGUMENTSMANAGER_H_ */