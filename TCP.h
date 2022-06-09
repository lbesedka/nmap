#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ArgumentsManager.h"
//using namespace std;

namespace TCPscannerSpace {
    class TCPscanner {
    private:
        sockaddr_in addr{};

        //bool isNumber(const std::string &str);

        //std::vector<std::string> split(const std::string &str, char delim);

        //bool validateIP(std::string ip);

        void process2_main(const sockaddr_in &addr);

        bool try_connect(const sockaddr_in &addr);

    public:
        TCPscanner(std::string s){
            addr.sin_addr.s_addr = inet_addr(s.c_str());
            addr.sin_family = AF_INET;
            addr.sin_port = htons(0);
        }

        void scanPorts(ArgumentsManager a);
    };
}
