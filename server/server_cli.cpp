#include <iostream>
#include <string>

#include "server_core.h"


class ServerCLI
{
public:
    ServerCLI() {}
    void print_help()
    {
        std::cout << 
        "Usage:\n"
        " --help              - Show this help message\n"
        " new <client_ip>     - Create a new client\n"
        " ping <client_ip>    - Ping a client to check if it's alive\n"
        " list                - List all clients\n"
        " exit <client_ip>    - Exit the CLI\n";
    }

    void creating_client(const std::string &username)
    {
        std::cout << "Process creating client card...\n"; 
        std::cout << create_client(username) << std::endl;
    }

    void pinging_client(const std::string &username)
    {
        std::cout << "Pinging client...\n";
        std::cout << ping_client(username) << std::endl;
    }

    void on_clients()
    {
        std::cout << "Requesting list of clients...\n";
        std::cout << list_clients() << std::endl;
    }
    void client_exit(const std::string &username)
    {
        std::cout << "Try to exit...\n";
        std::cout << exit(username) << std::endl;
    }

    void run_cli()
    {
        print_help();
        std::string command;
        while (true)
        {

            boost::this_thread::sleep(boost::posix_time::millisec(1000));
            std::cout << "server>> ";
            getline(std::cin, command);
            if (command == "--help") print_help();
            else if (command.find("new") == 0) 
            {
                std::string str = command.substr(4, command.length() - 1);
                creating_client(str);
            }
            else if (command.find("ping") == 0) 
            {
                std::string str = command.substr(5, command.length() - 1);
                pinging_client(str);
            }
            else if (command == "list") on_clients();
            else if (command.find("exit") == 0) 
            {
                std::string str = command.substr(5, command.length() - 1);
                client_exit(str);
            }
            else std::cerr << "Invalid command: " << command
                << "\n";
        }
    }
};

int main()
{
    
    ServerCLI interface;
    boost::thread_group threads;
    threads.create_thread(accept_thread); 
    threads.create_thread([&]{interface.run_cli();});
    threads.join_all();
    return 0;
}