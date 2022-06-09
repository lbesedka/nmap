#include <vector>
#include <signal.h>
#include <sys/socket.h>
#include "unistd.h"
#include "arpa/inet.h"
#include "sys/wait.h"
#include "iostream"
#include "TCP.h"
#include "ArgumentsManager.h"

//using namespace std;

using namespace TCPscannerSpace;

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

void TCPscanner::scanPorts(ArgumentsManager a) {
    int fd_list = socket(AF_INET, SOCK_STREAM, 0);
    int result = connect(fd_list, (sockaddr *) &addr, sizeof(sockaddr_in));
    if (result)
        std::cout << "Port " << 0 << " is open" << std::endl;
    setuid(getuid());
    for (int i = stoi(a.lw_brd); i < stoi(a.upr_brd); ++i) {
        addr.sin_port = htons(i);
        bool res = try_connect(addr);
        if (res) {
            std::cout << "Port " << i << " is open" << std::endl;
        }
    }
}

