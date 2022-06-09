#ifndef ARGUMENTSMANAGER_H_
#define ARGUMENTSMANAGER_H_


#include <string.h>
#include <iostream>
#include <string>
#include <list>


class ArgumentsManager{
private:
    std::list<std::string> ip;
public:
    ArgumentsManager(int argc, char* argv[]);
    int parse_args(int argc, char* argv[]);
    std::string input_low_port();
    std::string input_up_port(std::string low_port);
    void help();
};

#endif /* ARGUMENTSMANAGER_H_ */