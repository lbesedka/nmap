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

        void process3_main(const sockaddr_in &addr);

        bool try_connect(const sockaddr_in &addr);

    public:
        UDPscanner(std::string s){
            addr.sin_addr.s_addr = inet_addr(s.c_str());
            addr.sin_family = AF_INET;
            addr.sin_port = htons(0);
        }

        void try_scan(const char* dest , unsigned int d_port);
        bool try_connect(const sockaddr_in &addr, int i);
        void scanPorts();
        void create_ip_header(iphdr *iph, unsigned long source, unsigned long destination, unsigned int length, unsigned short protocol);
        void create_udp_header(udphdr* udp_header, iphdr *ip_header, unsigned short source_port, unsigned short dest_port);
        std::string get_source_ip(sockaddr_in &source);
        void dns_format_convert(unsigned char* dns,unsigned char* host);
        int time_remaining(time_t start_time);

    };
}

#endif //NMAP_UDPTOOLSET_H
