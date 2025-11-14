#include <iostream>
#include <string>

#include "server_core.h"

extern std::queue<std::string> tasks_pull;
int interval = set_sec_timeout();

class ServerCLI
{
public:
    ServerCLI() {}
    void print_help()
    {
        std::cout <<
        "Usage:\n"
        " --help                    - Show this help message\n"
        " new <client>              - Create a new client\n"
        " ping <client>             - Ping a client to check if it's alive\n"
        " list                      - List all clients\n"
        " load_tasks <file>         - Load task pool from text file\n"
        " tasks                     - Show current task pool\n"
        " run_task                  - Dispatch one task to an active client\n"
        " exit <client>             - Disconnect client\n";
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

    void show_tasks()
    {
        std::cout << "Current task pool:\n";
        std::cout << list_tasks_pull();
    }

    void run_one_task()
    {
        std::string res = dispatch_task_to_active_client();

        if (res == "ok")
            std::cout << "Task executed successfully\n";
        else if (res == "error")
            std::cout << "Task failed, returned to pool\n";
        else if (res == "no_tasks")
            std::cout << "No tasks in pool\n";
        else if (res == "no_active_clients")
            std::cout << "No active clients available for tasks\n";
        else
            std::cout << "Unexpected dispatch result: " << res << "\n";
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
            else if (command.find("load_tasks") == 0)
            {
                std::string path = command.substr(11);
                size_t before = tasks_pull.size();
                load_tasks(path);
                size_t after = tasks_pull.size();
                std::cout << "Loaded " << (after - before)
                        << " tasks from: " << path << std::endl;
            }
            else if (command == "tasks")
            {
                show_tasks();
            }
            else if (command == "run_task")
            {
                run_one_task();
            }
            else std::cerr << "Invalid command: " << command
                << "\n";
        }
    }
};

#ifdef PROD
int main()
{
    initialize_registry("data/clients.txt");
    ServerCLI interface;
    boost::thread_group threads;
    threads.create_thread(accept_thread); 
    threads.create_thread([&]{ ping_loop(interval);});
    threads.create_thread([&]{interface.run_cli();});
    threads.join_all();

    save_clients("data/clients.txt");
    return 0;
}
#endif