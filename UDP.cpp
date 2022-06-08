//
// Created by kerek on 07.06.2022.
//
#define _BSD_SOURCE
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <netinet/udp.h>
#include <sys/time.h>
#include <queue>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <iterator>
#include <arpa/inet.h>
#include <fstream>
#include <netdb.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdexcept>
#include <stdio.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <bits/ioctls.h>
#include <netinet/ether.h>
#include <poll.h>
#include <pthread.h>
#include <iomanip>
#include <ifaddrs.h>
#include <math.h>
#include <cmath>
#include "UDP.h"
#include "DNS.h"


sockaddr_in ipv4_address;
string ipv4_string;
unsigned char *qname;
vector<Report>report;
Report *status_report=new Report;
PortScanner p1;
//using namespace std;

using namespace UDPscannerSpace;

void UDPscanner::dns_format_convert(unsigned char* dns,unsigned char* host) {
    int lock = 0 , i;
    strcat((char*)host,".");

    for(i = 0 ; i < strlen((char*)host) ; i++) {
        if(host[i]=='.') {
            *dns++ = i-lock;
            for(;lock<i;lock++) {
                *dns++=host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++='\0';
}

void UDPscanner::create_ip_header(iphdr *iph, unsigned long source, unsigned long destination, unsigned int length, unsigned short protocol)
{


    iph->frag_off = 0;         /* fragmentation not required */
    iph->ttl = 255;            /* maximum limit on number of hops */
    iph->protocol = protocol;  /* Next layer protocol. Can be 1 (ICMP), 6 (TCP) or 17 (UDP) */
    iph->check = 0;            /* Kernel always sets the IPv4 checksum */
    iph->saddr = source;       /* self explanatory :P */
    iph->daddr = destination;
    iph->ihl = 5;              /* Header length. 4 times this is 20 bytes. Mac value is 15 */
    iph->version = 4;          /* IPv4 used */
    iph->tos = 0;              /* Type of service. 0=> Routine. Kernel might replace this with DSCP/ECN :(.  */
    iph->tot_len = length;     /* Total length of the datagram */
    iph->id = htons (14118);   /* Identification. We dont want to fragment. But kernel will fragment if we go above
                                      MTU. Not clear about this*/

}

void UDPscanner::create_udp_header(udphdr* udp_header, iphdr *ip_header, unsigned short source_port, unsigned short dest_port)
{

    //Populate the UDP header here manually. Used for UDP scans
    udp_header->source=source_port;
    udp_header->dest=dest_port;
    udp_header->len = htons(sizeof(udphdr));

    //I believe UDP checksum is not required. But still added , not sure about it.
    udp_header->check = 0;

    //udp_header->check = generate_udp_checksum(ip_header, udp_header, (ip_header->tot_len-(ip_header->ihl*4)));

}

string UDPscanner::get_source_ip(sockaddr_in &source)
{

    char buffer[INET_ADDRSTRLEN]; //Store the IP address
    memset(&source, 0, sizeof(source));
    void * temp_ipv4_address = NULL;
    //implementation as per getifaddr() man page.

    ifaddrs *interface_addr = NULL, *ifa= NULL;

    if(getifaddrs(&interface_addr) == -1 )
    {
        perror("\n Error in getifaddr function call \n");
    }

    /*Iterate over the linked list */

    for ( ifa = interface_addr; ifa != NULL; ifa=ifa->ifa_next)
    {
        if ((ifa->ifa_addr->sa_family == AF_INET) && (strcmp(ifa->ifa_name,"eth0")==0))
        {

            temp_ipv4_address = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            //Convert network-to-presentation
            inet_ntop(AF_INET, temp_ipv4_address, buffer, INET_ADDRSTRLEN);

            ipv4_string = buffer;  //Buffer has the "presentation" address

        }

    }
    //cleanup
    freeifaddrs(interface_addr);

    //Populate source. c_str to convert to c style pointer
    //Populate source. c_str to convert to c style pointer
    if(inet_aton(ipv4_string.c_str(), &source.sin_addr) == 0)
    {
        perror("\n inet_aton function call failed \n");
    }


    return ipv4_string;
}

int UDPscanner::time_remaining(time_t start_time)  /*remaining time waiting for a response */
{
    /*get current time and subtract */

    time_t current_time = 0, remain;
    current_time = time(NULL);
    remain = current_time - start_time;
    return (int)remain;

}

void UDPscanner::try_scan(const char* dest , unsigned int d_port)
{
    /*UDP scan starts here. IPPROTO_UDP. Check ICMP responses */

    int status=0;
    addrinfo *list_pointer = NULL,hints;
    sockaddr_in source,destination;
    std::string source_host;
    timeval time_val;
    time_val.tv_sec = 5;
    time_val.tv_usec =0;
    char read_buffer[4096] = {0};
    char buf[65536];
    char dns_buffer[65536];
    iphdr *iph;

    /* DNS query variables */
    /*--------------------------------------*/
    struct dns_header *dns = {nullptr};
    struct question *qinfo = NULL;
    dns = (struct dns_header *)&dns_buffer[sizeof(struct ip) + sizeof(struct udphdr)];
    unsigned char dns_host[] = "pdd.yandex.ru";


    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    //point to the query portion
    qname = (unsigned char*)&dns_buffer[sizeof(struct dns_header) + sizeof(struct iphdr) + sizeof(struct udphdr)];
    dns_format_convert(qname , dns_host);
    qinfo = (struct question*) &dns_buffer[sizeof(struct dns_header) + (strlen((const char*)qname) + 1) + sizeof(struct iphdr) + sizeof(struct udphdr)]; //fill it
    qinfo->qtype = htons(1); //type of the query , A , MX , CNAME , NS etc
    qinfo->qclass = htons(1); //its internet (lol)

    /*-------------------------*/

    status_report->destination_port = d_port;
    status_report->destination_host = dest;
    status_report->scan_type = "UDP";



    memset(&hints,0, sizeof(addrinfo));
    hints.ai_family=AF_INET;  //Only IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = hints.ai_flags | AI_CANONNAME ; //First attribute in list is the host name


    //Main task. Populate dest.
    if ((status = getaddrinfo (dest, NULL, &hints, &list_pointer)) != 0) {
        perror("\n getaddrinfo() call failed. Check options set \n");
    }


    in_port_t source_port = getpid() % 65536;   //A little buggy
    status_report->source_port = source_port;

    memcpy(&destination, (sockaddr_in *) list_pointer->ai_addr, list_pointer->ai_addrlen);

    destination.sin_port = htons(d_port);
    source_host = "127.0.0.1";

    // cout<<"Source is :"<<source_host<<endl;

    int write_sock, read_sock;
    int buffer_size = 60*1024;
    //=======================
    char final_header[4096];
    //========================

    if((write_sock = socket(AF_INET,SOCK_RAW, IPPROTO_UDP)) < 0 )
    {
        perror("\n\t Socket creation failed \n");
        exit(1);
    }

    if((read_sock = socket(AF_INET,SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        perror("n\t ICMP socket creation failed \n");
        exit(1);
    }


    int option = 1;
    const int *temp_val = &option;


    if (setsockopt (write_sock, IPPROTO_IP, IP_HDRINCL, temp_val, sizeof (option)) < 0)
    {
        cout<<"Error setting IP_HDRINCL";
        exit(0);

    }


    if(setsockopt(read_sock, SOL_SOCKET, SO_RCVTIMEO, (timeval *)&time_val, sizeof(timeval)) != 0)
    {
        perror("\n Setting socket options failed \n");
    }

    /* ----- DNS query part -------*/


    if ( d_port == 53 )
    {

        iph = (iphdr *) dns_buffer;
        create_ip_header(iph,source.sin_addr.s_addr , destination.sin_addr.s_addr, sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct dns_header) + (strlen((const char*)qname)+1) + sizeof(struct question), IPPROTO_UDP);

        udphdr *udp_header = (udphdr*)(dns_buffer + sizeof(iphdr));
        create_udp_header(udp_header, iph, source_port, destination.sin_port);
    }

    else
    {
        iph = (iphdr *) final_header;
        create_ip_header(iph,source.sin_addr.s_addr , destination.sin_addr.s_addr, sizeof(iphdr)+ sizeof(udphdr), IPPROTO_UDP);

        udphdr *udp_header = (udphdr*)(final_header + sizeof(iphdr));
        create_udp_header(udp_header, iph, source_port, destination.sin_port);
    }

    ssize_t sent_bytes;
    time_t timer;
    int t, count=0;


    do{

        if( d_port == 53 )
            sent_bytes = sendto(write_sock, dns_buffer, iph->tot_len, 0, (sockaddr *)&destination, sizeof(destination));
        else
            sent_bytes = sendto(write_sock, final_header, sizeof(udphdr) + sizeof(iphdr), 0, (sockaddr *)&destination, sizeof(destination));


        time(&timer);
        count++;

        if(sent_bytes < 0)
        {
            perror("\n sendto() function call failed, Check arguments \n");
            exit(1);
        }

        ssize_t data_read ;
        sleep(2);

        ssize_t received_bytes=recv(read_sock, read_buffer , sizeof(read_buffer), 0);
        if(received_bytes < 0 )
        {
            status_report->protocol = IPPROTO_UDP;
            status_report->status = "OPEN|FILTERED";
        }
        else
        {
            break;
        }

        t=time_remaining(timer);

    }
    while(t<5);

    iphdr* read_iphdr = (iphdr*) read_buffer;
    int result = 2;
    if ( read_iphdr->protocol==IPPROTO_ICMP )
    {
        /* Got ICMP desitnation unreachable response */

        icmphdr* read_icmphdr = (icmphdr*)(read_buffer + (int)read_iphdr->ihl*4);
        /* Check if type matches */
        if((read_icmphdr->type == ICMP_UNREACH) && (read_icmphdr->code == ICMP_UNREACH_PORT))
        {
            result = true;
            status_report->protocol = IPPROTO_UDP;
            status_report->status = "CLOSED";

        }
        exit(result);

    }
    if ( read_iphdr->protocol==IPPROTO_UDP)
    {
        result = false;
        cout<<"Got a udp packet "<<endl;
    }
    close(write_sock);
    close(read_sock);
    exit(result);
}

bool UDPscanner::try_connect(const sockaddr_in &addr, int i) {
    auto pid = fork();
    if (!pid) {
        try_scan("127.0.0.1", i);
    }

    timespec interval{.tv_sec = 0, .tv_nsec = 1000000};

    int p = 0, stat = 0;

    for (int i = 0; i < 20; ++i) {
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

void UDPscanner::scanPorts() {
    std::cout << geteuid() << std::endl;
    for (int i = 2; i < 65535; ++i) {
        addr.sin_port = htons(i);
        bool res = try_connect(addr, i);
        if (res) {
            std::cout << "UDP Port " << i << " is closed" << std::endl;
        }
        else if (!res)
        {
            std::cout << "UDP port " << i << " is (probably) open" << std::endl;
        }
    }
}