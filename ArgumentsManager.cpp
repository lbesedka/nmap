#include "ArgumentsManager.h"

ArgumentsManager::ArgumentsManager(int argc, char* argv[]){
    parse_args(argc, argv);

}

int ArgumentsManager::parse_args(int argc, char* argv[]) {
    std::string port;
    for (int i = 1; i < argc; i++) {
        if(strcmp(argv[i],"-h") == 0){
            help();
            return 1;
        }else if(strcmp(argv[i],"-ip") == 0){
            ip.push_back(argv[i+1]);
            std::cout<<ip.back()<<std::endl;
            if(input_up_port(input_low_port())=="0")
            return 0;
            break;

        }else{
            std::cout << "Error: wrong arguments\n";
            help();
            return 1;
        }
    }
    return 0;
}

std::string ArgumentsManager::input_low_port(){
    std::string arg;
    std::cout<<"Enter lower border of port range: ";
    std::cin>>arg;
    if(stoi(arg) > 65535 || stoi(arg) < 0) {
        std::cout << "Check -h" << std::endl;
        return"0";
    }
    return arg;
}
std::string ArgumentsManager::input_up_port(std::string low_port){
    std::string arg;
    std::cout<<"Enter upper border of port range: ";
    std::cin>>arg;
    if(stoi(arg) > 65535 || stoi(arg) < 0 || stoi(arg) < stoi(low_port)) {
        std::cout << "Check -h" << std::endl;
        return"0";
    }
    return arg;
}

void ArgumentsManager::help() {
    std::cout << "help: \n"
                 "-ip:<IP address to scan>. Example: -ip 127.0.0.1\n"
                 "-p <ports to scan>. Example: -p {0<lower border<65535 and 0<upper border<65535 and lower border must be smaller than upper}";
}


