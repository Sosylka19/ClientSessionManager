#include <cassert>
#include <iostream>
#include "../server/server_core.h"

struct talk_to_client;
typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> array;
extern array clients;
extern boost::recursive_mutex cs;
int main() {
    {
        boost::recursive_mutex::scoped_lock lock(cs);
        clients.clear();
    }

    std::string msg;

    msg = create_client("Alice");
    assert(msg.find("Alice") != std::string::npos);

    msg = create_client("Bob");
    assert(msg.find("Bob") != std::string::npos);

    {
        boost::recursive_mutex::scoped_lock lock(cs);
        assert(clients.size() == 2);
        assert(clients[0]->username() != "");
        assert(clients[1]->username() != "");
    }

    msg = list_clients();
    assert(msg.find("Alice") != std::string::npos);
    assert(msg.find("Bob") != std::string::npos);

    msg = ping_client("Alice");
    assert(msg == "Client Alice is offline.");

    msg = ping_client("Charlie");
    assert(msg == "Client Charlie not found.");

    msg = exit("Charlie");
    assert(msg == "Client Charlie not found.");

    msg = exit("Alice");
    assert(msg == "Client is not connected");

    std::cout << "server tests passed\n";
    return 0;
}