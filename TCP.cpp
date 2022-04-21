#include <vector>
#include <signal.h>
#include <sys/socket.h>
#include "unistd.h"
#include "arpa/inet.h"
#include "sys/wait.h"
#include "iostream"
#include "TCP.h"

//using namespace std;

using namespace TCPscannerSpace;

/*bool TCPscanner::isNumber(const std::string &str) {
    if (!str.empty() && (str.find_first_not_of("[0123456789]") == std::string::npos))
        return true;
    else return false;
}

std::vector<std::string> split(const std::string &str, char delim) {
    auto i = 0;
    std::vector<std::string> list;

    auto pos = str.find(delim);

    while (pos != std::string::npos) {
        list.push_back(str.substr(i, pos - i));
        i = ++pos;
        pos = str.find(delim, pos);
    }
    list.push_back(str.substr(i, str.length()));
    return list;
}

bool TCPscanner::validateIP(std::string ip) {
    std::vector<std::string> list = split(ip, '.');
    if (list.size() != 4) {
        return false;
    }
    for (std::string str: list) {
        if (!isNumber(str) || stoi(str) > 255 || stoi(str) < 0) {
            return false;
        }
    }
    return true;
}*/

void TCPscanner::process2_main(const sockaddr_in &addr) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int result = connect(fd, (sockaddr *) &addr, sizeof(sockaddr_in));
    close(fd);
    exit(result);
}

bool TCPscanner::try_connect(const sockaddr_in &addr) {
    auto pid = fork();
    if (!pid) {
        process2_main(addr);
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

void TCPscanner::scanPorts() {
    std::cout << geteuid() << std::endl;

    int fd_list = socket(AF_INET, SOCK_STREAM, 0);
    int result = connect(fd_list, (sockaddr *) &addr, sizeof(sockaddr_in));
    if (result)
        std::cout << "Port " << 0 << " is open" << std::endl;
    setuid(getuid());

    std::cout << geteuid() << std::endl;
    for (int i = 2; i < 65535; ++i) {
        addr.sin_port = htons(i);
        bool res = try_connect(addr);
        if (res) {
            std::cout << "Port " << i << " is open" << std::endl;
        }
    }
}

