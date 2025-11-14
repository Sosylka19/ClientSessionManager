#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "connection_card.h"


using namespace boost::asio;
using namespace boost::posix_time;
using namespace boost::placeholders;

struct talk_to_client: boost::enable_shared_from_this<talk_to_client>, public ConnectionCard
{
    //дописать явный интерфейс по моим методам с кли
    talk_to_client();

    std::string username() const;

    // void answer_to_client();
    std::string ping_client();
    void set_client_status();
    ip::tcp::socket& sock();
    std::string stop();
    void login();
    std::string handshake();
    void set_name(const std::string &name);
    bool get_status() const;
    bool operator==(const talk_to_client& obj);
    // bool timed_out() const;
private:
    void read_answer();
    std::string process_answer();
    std::string on_login(const std::string &msg);
    std::string on_ping(const std::string &msg);
    std::string on_username(const std::string &msg);
    void write(const std::string& msg);
private:
    ip::tcp::socket socket_;
    enum { max_msg = 1024};
    char buff_[max_msg];
    bool status_;
    int already_read_;
    std::string username_;
    bool clients_changed_;
    // ptime last_ping;
};

void accept_thread();
void handle_clients_thread();
std::string ping_client(const std::string& username);
std::string list_clients();
std::string exit(const std::string& username);
std::string create_client(const std::string& username);
