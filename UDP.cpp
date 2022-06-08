#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <poll.h>
#include <pthread.h>
#include "UDP.h"
#include "DNS.h"
sockaddr_in server_socket_addr;


using namespace UDPscannerSpace;
/*
int UDPscanner::try_scan(const sockaddr_in &addr,int port)
{
    int wrfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (wrfd < 0)
    {
        perror("Failed to open send socket");
        exit(-1);
    }
    fd_set sockets;
    FD_SET (wrfd, &sockets);
    socklen_t socklen = sizeof(server_socket_addr);
    std::string sendString = "scanning";
    if (sendto(wrfd, sendString.c_str(), sendString.size() + 1, 0, (sockaddr *)&addr, socklen) < 0)
    {
        std::cout << "send in port scanner failed" << std::endl;
    }
    int responseSize = 1024;
    char response[responseSize];
    memset(response, 0, responseSize); // zero initialize char array

    struct timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;
    if (select(wrfd + 1, &sockets, NULL, NULL, &timeout) > 0) {
        int byteCount = recvfrom(wrfd, response, responseSize, 0, (sockaddr *) &addr, &socklen);
        if (byteCount < 0) {
            std::cout << "error receiving output from server" << std::endl;
            close(wrfd);
            exit(-1);
        } else {
            response[byteCount] = '\0';
            std::cout << "-----------------------" << std::endl;
            std::cout << response << std::endl;
            std::cout << "Byte count received: " << byteCount << ", "
                      << "on port: " << port << std::endl;
            std::cout << "-----------------------" << std::endl;
            exit(0);
        }
        close(wrfd);
    }
    exit(-1);
}
*/

bool rx_packet(int fd)
{
    int responseSize = 1024;
    char buf[responseSize];
    memset(buf, 0, responseSize);
    struct timeval poll{};
    poll.tv_sec = 1;
    poll.tv_usec = 0;
    fd_set fds;
    while(1)
    {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        if(select(fd + 1, &fds, NULL, NULL, &poll) > 0)
        {
            recvfrom(fd, &buf, sizeof(buf), 0x0, NULL, NULL);
        }
        else if(!FD_ISSET(fd, &fds))
            return 1;
        else
            return 0;
    }
}

void tx_packet(int fd, int port, hostent* he)
{
    int responseSize = 1024;
    char response[responseSize];
    memset(response, 0, responseSize);
    sockaddr_in servaddr{};
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr = *((struct in_addr *)he->h_addr);

    if(sendto(fd, response, sizeof(response), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("*** sendto() failed ***");
    }
}

int UDPscanner::try_scan(const sockaddr_in &addr,int port, hostent *he)
{

    int sendfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int recvfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    auto srvport = getservbyport((htons(port)), "udp");
    // open send UDP socket
    if(sendfd < 0)
    {
        perror("*** socket(,,IPPROTO_UDP) failed ***n");
        exit(-1);
    }
    // open receive ICMP socket
    if(recvfd < 0)
    {
        perror("*** socket(,,IPPROTO_ICMP) failed ***n");
        exit(-1);
    }
        tx_packet(sendfd, port, he);

        if(rx_packet(recvfd) == 1)
        {
            srvport = getservbyport(htons(port), "udp");

            if (srvport != NULL)
            {
                close(sendfd);
                close(recvfd);
                std::cout << "Host: " << srvport->s_name << std::endl;
                std::cout << "UDP Port " << port << " is (probably) open. Host: " << srvport->s_name << std::endl;
                exit(0);
            }

            fflush(stdout);
        }
    close(sendfd);
    close(recvfd);
    exit(-1);
}


bool UDPscanner::try_connect(const sockaddr_in &addr, int port, hostent *he) {
    auto pid = fork();
    if (!pid) {
        try_scan(addr, port, he);
    }

    timespec interval{.tv_sec = 0, .tv_nsec = 100000000};

    int p = 0, stat = 0;

    for (int i = 0; i < 10; ++i) {
        p = waitpid(pid, &stat, WNOHANG);
        if (p == 0)
            nanosleep(&interval, nullptr);
        else
            break;
    }
    if (p == 0) {
        kill(pid, SIGTERM);
        return false;
    }
    return stat == 0;
}


void UDPscanner::scanPorts(hostent* he) {
    std::cout << geteuid() << std::endl;
    for (int i = 1; i < 65535; ++i) {
        addr.sin_port = htons(i);
        bool res = try_connect(addr, i, he);
    }
}
