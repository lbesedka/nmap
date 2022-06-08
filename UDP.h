//
// Created by kerek on 03.06.2022.
//

#ifndef NMAP_UDPTOOLSET_H
#define NMAP_UDPTOOLSET_H
#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

//using namespace std;

namespace UDPscannerSpace {
    class UDPscanner {
    private:
        sockaddr_in addr{};

        //bool isNumber(const std::string &str);

        //std::vector<std::string> split(const std::string &str, char delim);

        //bool validateIP(std::string ip);

    public:
        UDPscanner(std::string s){
            addr.sin_addr.s_addr = inet_addr(s.c_str());
            addr.sin_family = AF_INET;
            addr.sin_port = htons(0);
        }

        int try_scan(const sockaddr_in &addr, int port, hostent *he);
        bool try_connect(const sockaddr_in &addr, int i, hostent *he);
        void scanPorts(hostent *he);

    };
}

#endif //NMAP_UDPTOOLSET_H
