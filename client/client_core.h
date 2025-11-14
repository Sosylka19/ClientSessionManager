#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;
using namespace boost::placeholders;

struct talk_to_server
{
public:
    talk_to_server(const std::string &username);
    void connect(ip::tcp::endpoint &ep);
    void loop();
    const std::string &username() const;
    void on_task(const std::string& task);
private:
    void read_answer();
    void process_request();
    void on_handshake();
    void on_login();
    void on_ping(const std::string& msg);
    void write(const std::string& msg);
    size_t read_complete(const boost::system::error_code &error, size_t bytes);
private:
    ip::tcp::socket socket_;
    bool status_;
    enum { max_msg = 1024 };
    char buff_[max_msg];
    std::string username_;
    int already_read_;
};

void run_client(const std::string& username, const std::string& ip, uint16_t port);
bool cmd(const std::string& cmd);