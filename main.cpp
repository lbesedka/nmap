#include <iostream>
#include "arpa/inet.h"
#include "TCP.h"
#include <cstring>
//#include "TCP.cpp"

int main(int argc, char **argv) {
    sockaddr_in addr{};
    std::string s;

    switch(argc){
        case 2:
            s = argv[1];
            addr.sin_addr.s_addr = inet_addr (s.c_str());
            if (addr.sin_addr.s_addr != INADDR_NONE)
                break;
        case 1:
            do{
                std::cout << "Enter your IP-address: " << std::endl;
                std::cin >> s;
                addr.sin_addr.s_addr = inet_addr (s.c_str());
            }while (addr.sin_addr.s_addr == INADDR_NONE);
            break;

    }
    if (argc > 2)
        return 1;
    else {
        if (argc == 2) {
            s = argv[1];
            addr.sin_addr.s_addr = inet_addr(s.c_str());
        }
        do {
            if (addr.sin_addr.s_addr != INADDR_NONE)
                break;
            std::cout << "Enter your IP-address: " << std::endl;
            std::cin >> s;
            addr.sin_addr.s_addr = inet_addr(s.c_str());
        } while (true);
    }



    do{
        std::cout << "Enter your IP-address: " << std::endl;
        std::cin >> s;
        addr.sin_addr.s_addr = inet_addr (s.c_str());
    }while (addr.sin_addr.s_addr == INADDR_NONE);

    TCPscannerSpace::TCPscanner scanner(s);
    scanner.scanPorts();

    /*std::cout << geteuid() << std::endl;

    // entered adress
    addr.sin_addr.s_addr = inet_addr(s.c_str());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);

    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_family = AF_INET;
    address.sin_port = htons(0);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(fd, (sockaddr*)&address, sizeof(sockaddr_in));
    listen(fd_list,1);

    int fd_list = socket(AF_INET, SOCK_STREAM, 0);
    int result = connect(fd_list, (sockaddr*)&addr, sizeof(sockaddr_in));
    if (result)
        std::cout << "Port "<< 0 << " is open" << std::endl;
    setuid(getuid());

    std::cout << geteuid() << std::endl;
    for (int i = 2; i < 65535; ++i)
    {
        addr.sin_port = htons(i);
        bool res = try_connect(addr);
        if (res) {
            std::cout << "Port " << i << " is open" << std::endl;
        }
    }*/

    return 0;
}