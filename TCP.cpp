#include <iostream>
#include <vector>
#include <signal.h>
#include <sys/socket.h>
#include "unistd.h"
#include "arpa/inet.h"
#include "sys/wait.h"

using namespace std;

bool isNumber(const string& str)
{
    if (!str.empty() && (str.find_first_not_of("[0123456789]") == string::npos))
        return true;
    else return false;
}

vector<string> split(const string& str, char delim)
{
    auto i = 0;
    vector<string> list;

    auto pos = str.find(delim);

    while (pos != string::npos)
    {
        list.push_back(str.substr(i, pos - i));
        i = ++pos;
        pos = str.find(delim, pos);
    }
    list.push_back(str.substr(i, str.length()));
    return list;
}

bool validateIP(string ip)
{
    vector<string> list = split(ip, '.');
    if (list.size() != 4) {
        return false;
    }
    for (string str : list)
    {
        if (!isNumber(str) || stoi(str) > 255 || stoi(str) < 0) {
            return false;
        }
    }
    return true;
}

void process2_main(const sockaddr_in& addr) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int result = connect(fd, (sockaddr*)&addr, sizeof(sockaddr_in));
    close(fd);
    exit(result);
}

bool try_connect(const sockaddr_in& addr) {
    auto pid = fork();
    if (!pid) {
        process2_main(addr);
    }

    timespec interval{ .tv_sec = 0, .tv_nsec = 1000000 };

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

int main() {
    sockaddr_in addr{};
    sockaddr_in address{};
    std::string s;
    int fd;

    do{
        std::cout << "Enter your IP-address: " << std::endl;
        std::cin >> s;
        addr.sin_addr.s_addr = inet_addr (s.c_str());
    }while (addr.sin_addr.s_addr == INADDR_NONE);
    cout << geteuid() << std::endl;
    addr.sin_addr.s_addr = inet_addr(s.c_str());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_family = AF_INET;
    address.sin_port = htons(0);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    int fd_list = socket(AF_INET, SOCK_STREAM, 0);
    bind(fd, (sockaddr*)&address, sizeof(sockaddr_in));
    listen(fd_list,1);
    int result = connect(fd_list, (sockaddr*)&addr, sizeof(sockaddr_in));

    if (result)
        std::cout << "Port "<< 0 << " is open" << std::endl;
    setuid(getuid());

    cout << geteuid() << std::endl;
    for (int i = 2; i < 65535; ++i)
    {
        addr.sin_port = htons(i);
        bool result = try_connect(addr);
        if (result) {
            std::cout << "Port " << i << " is open" << std::endl;
        }
    }
}
