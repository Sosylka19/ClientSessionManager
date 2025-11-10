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
            write_request();
            read_answer();

            int millisec = rand() % 7000;
            std::cout << username_ << " postpone ping: "
                << millisec << "\n";
            boost::this_thread::sleep(boost::posix_time::millisec(millisec));
        }
    }

    std::string username()
    {
        return username_;
    }
private:
    void write_request()
    {
        write("ping\n");
    }
 
    void read_answer()
    {
        already_read_ = 0;
        read(socket_, boost::asio::buffer(buff_), 
            boost::bind(&talk_to_server::read_complete, this, _1, _2));
        process_msg();
    }

    void process_msg()
    {
        std::string msg(buff_, already_read_);
        if (msg.find("login ") == 0) on_login();
        else if (msg.find("ping") == 0) on_ping(msg);
        else if (msg.find("clients ") == 0) on_clients(msg);
        else std::cerr << "invalid_msg " << msg << std::endl;
    }

    void on_login()
    {
        std::cout << username_ << " logged_in" << std::endl;
        do_ask_clients();
    }
    void on_ping(const std::string& msg)
    {
        std::istringstream is(msg);
        std::string answer;
        is >> answer >> answer;
        std::cout << username_ << " ПИНГАНУЛ " << std::endl;
        if (answer == "client_list_changed")
            do_ask_clients();
    }
    void on_clients(const std::string& msg)
    {
        std::string clients = msg.substr(8);
        std::cout << username_ << ", new clients list:" << clients << std::endl;
    }

    void do_ask_clients()
    {
        write("ask clients\n");
        read_answer();
    }
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

ip::tcp::endpoint ep( ip::make_address("127.0.0.1"), 8001);

void run_client(const std::string& username)
{
    talk_to_server obj(username);
    try
    {
        obj.connect(ep);
        obj.loop();
    }catch (boost::system::system_error &error)
    {
        std::cout << "Client quit unexpectedly: " << obj.username()
        << ": " << error.what() << std::endl;
    }
}

int main(int argv, char* argc[])
{
    boost::thread_group threads;
    std::vector<std::string> names = 
    {
        "Lecler",
        "Hamilton",
        "Verstappen"
    };
    for (auto name: names)
    {
        threads.create_thread(boost::bind(run_client, name));
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
    threads.join_all();
}



