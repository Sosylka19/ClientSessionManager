#include <iostream>
#include <string>
#include "cli.h"

void print_help()
{
    std::cout << 
    "Usage:\n"
    " --help              - Show this help message\n"
    " new <client>        - Create a new client\n"
    " connect <client>    - Connect to an existing client\n"
    " ping <client>       - Ping a client to check if it's alive\n"
    " list                - List all clients\n"
    " exit                - Exit the CLI\n";
}

void create_client(const std::string &username)
{
    std::cout << "create\n"; 
}

void connect_client(const std::string &username)
{
    std::cout << "connect\n";
}

void ping_client(const std::string &username)
{
    std::cout << "ping\n";
}

void on_clients()
{
    std::cout << "on_clients\n";
}
void cli_exit()
{
    std::cout << "exit\n";
}

int run_cli()
{
    print_help();
    std::string command;
    while (true)
    {
        std::cout << "server>> ";
        getline(std::cin, command);
        if (command == "--help") print_help();
        else if (command.find("new") == 0) 
        {
            std::string str = command.substr(4, command.length() - 1);
            create_client(str);
        }
        else if (command.find("connect") == 0) 
        {
            std::string str = command.substr(8, command.length() - 1);
            connect_client(str);
        }
        else if (command.find("ping") == 0) 
        {
            std::string str = command.substr(5, command.length() - 1);
            ping_client(str);
        }
        else if (command == "list") on_clients();
        else if (command == "exit") exit();
        else std::cerr << "Invalid command: " << command
            << "\n";
    }
    return 0;
}

int main()
{
    run_cli();
    return 0;
}