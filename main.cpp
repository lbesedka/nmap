
#include "TCP.h"
#include "UDP.h"
#include "netdb.h"
#include "ArgumentsManager.h"

int main(int argc, char *argv[]) {

    ArgumentsManager a = ArgumentsManager(argc, argv);
    hostent *he = gethostbyname(a.ip.c_str());
    UDPscannerSpace::UDPscanner scannerU(a.ip);
    TCPscannerSpace::TCPscanner scanner(a.ip);
    switch(a.flag)
    {
        case 2: // UDP
            scannerU.scanPorts(he, a);
            break;
        case 3: //TCP
            scanner.scanPorts(a);
            break;
        default:
            break;
    }


    return 0;
}