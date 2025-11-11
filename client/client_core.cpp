#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
using namespace boost::asio;
using namespace boost::placeholders;
boost::asio::io_context service;

struct talk_to_server
{
    talk_to_server(const std::string &username): 
    socket_(service), status_(true), username_(username) {}

    void connect(ip::tcp::endpoint &ep)
    {
        socket_.connect(ep);
    }

    void loop()
    {
        write("login " + username_ + "\n");
        read_answer();
        while(status_)
        {
            // write_request();
            read_answer();

            int millisec = rand() % 2000;
            // std::cout << username_ << " postpone ping: "
            //     << millisec << "\n";
            boost::this_thread::sleep(boost::posix_time::millisec(millisec));
        }
    }

    std::string username()
    {
        return username_;
    }
private:
 
    void read_answer()
    {
        //Read requst/answer from server and process it
        already_read_ = 0;
        read(socket_, boost::asio::buffer(buff_), 
            boost::bind(&talk_to_server::read_complete, this, _1, _2));
        process_request();
    }

    void process_request()
    {
        std::string msg(buff_, already_read_);
        if (msg.find("login_check") == 0) on_handshake();
        else if (msg.find("logging ok") == 0) on_login();
        else if (msg.find("ping") == 0) on_ping(msg);
        // else if (msg.find("clients ") == 0) on_clients(msg);
        else std::cerr << "invalid_msg " << msg << std::endl;
    }

    void on_handshake()
    {
        write("name " + username_ + "\n");
    }

    void on_login()
    {
        std::cout << username_ << " logged_in" << std::endl;
        // do_ask_clients();
    }
    void on_ping(const std::string& msg)
    {
        std::cout << "ping\n";
        write("pong " + username_ + "\n");
        // std::istringstream is(msg);
        // std::string answer;
        // is >> answer >> answer;
        // std::cout << username_ << " ПИНГАНУЛ " << std::endl;
        // if (answer == "client_list_changed")
        //     do_ask_clients();
    }
    // void on_clients(const std::string& msg)
    // {
    //     std::string clients = msg.substr(8);
    //     std::cout << username_ << ", new clients list:" << clients << std::endl;
    // }

    // void do_ask_clients()
    // {
    //     write("ask clients\n");
    //     read_answer();
    // }
    void write(const std::string& msg)
    {
        socket_.write_some(buffer(msg));
    }
    size_t read_complete(const boost::system::error_code &error, size_t bytes)
    {
        if (error) return 0;
        already_read_ = bytes;
        bool find = std::find(buff_, buff_ + bytes, '\n') < buff_ + bytes;
        return find ? 0: 1;
    }
private:
    ip::tcp::socket socket_;
    bool status_;
    enum { max_msg = 1024 };
    char buff_[max_msg];
    std::string username_;
    int already_read_;
};

//сделать проверку на порт

void run_client(const std::string& username, const std::string& ip, uint16_t port)
{
    ip::tcp::endpoint ep( ip::make_address(ip), port);
    talk_to_server obj(username);
    try
    {
        try
        {
            obj.connect(ep);
        } catch(boost::system::system_error &error)
        {
            std::cerr << "Client have not connected: " << error.what() << std::endl;
            return;
        }
        obj.loop();
    }catch (boost::system::system_error &error)
    {
        std::cout << "Client quit unexpectedly: " << obj.username()
        << ": " << error.what() << std::endl;
    }
}

int main(int argv, char* argc[])
{
    if (argv < 4)
    {
        std::cerr << "Too few parameters, supposed to be: <username_ip> <ip> <port>";
        return 1;
    }
    std::string name = static_cast<std::string>(argc[1]);
    std::string ip = static_cast<std::string>(argc[2]);
    uint16_t port = std::stoi(static_cast<std::string>(argc[3]));
    run_client(name, ip, port);
    return 0;
}



