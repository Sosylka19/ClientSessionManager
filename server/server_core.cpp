#include "server_core.h"


struct talk_to_client;
typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> array;
array clients;
boost::recursive_mutex cs;
boost::asio::io_context service;


talk_to_client::talk_to_client(): socket_(service), status_(false), already_read_(0)
{
    // last_ping = microsec_clock::local_time();
}
std::string talk_to_client::username() const 
{
    return username_;
}

// std::string talk_to_client::create_client_card(const std::string &username)
// {
//     client_cards_.emplace_back(username);
//     return "Client card for " + username + " created.";
// }

// void talk_to_client::answer_to_client()
// {
//     try
//     {
//         read_answer();
//         process_answer();
//     } catch (boost::system::system_error &)
//     {
//         stop();
//     }
//     if (timed_out())
//     {
//         stop();
//         std::cout << "stopping " << username_ << " - no ping in time" << std::endl;
//     }
// }

std::string talk_to_client::ping_client()
{

    //пока единственная команда
    try
    {
        write("ping\n");
    } catch(boost::system::system_error &err)
    {
        return "Socket error: " +  static_cast<std::string>(err.what());
    }
    read_answer();
    return process_answer();
}

void talk_to_client::login()
{
    read_answer();
    std::cout << process_answer();
}
    
void talk_to_client::set_client_status()
{
    status_ = true;
}

ip::tcp::socket& talk_to_client::sock()
{
    return socket_;
}

// bool talk_to_client::timed_out() const
// {
//     ptime now = microsec_clock::local_time();
//     long long time = (now - last_ping).total_milliseconds();
//     return time > 7000; 
// }

std::string talk_to_client::stop()
{
    boost::system::error_code error;
    if (!status_) return "Client is not connected";
    status_ = false;
    socket_.close(error);
    return "Client " + username_ + " disconnected\n";
}

void talk_to_client::read_answer()
{
    ///
    /// Read data from socket
    ///
    boost::system::error_code error;
    while (true)
    {
        size_t len = socket_.read_some(
            buffer(buff_ + already_read_, max_msg - already_read_), error);
        if (error == boost::asio::error::eof)
        {
            break;
        }
        else if (error)
        {
            throw boost::system::system_error(error);
        }
        already_read_ += len;
        if (std::find(buff_, buff_ + already_read_, '\n') < buff_ + already_read_)
            break;
    }
}

std::string talk_to_client::handshake()
{
    write("login_check\n");
    read_answer();
    return process_answer();
}

std::string talk_to_client::process_answer()
{
    bool found_enter = std::find(buff_, buff_ + already_read_, '\n') 
        < buff_ + already_read_;
    if (!found_enter)
        return "Incomplete message\n";
    
    // last_ping = microsec_clock::local_time();
    size_t pos = std::find(buff_, buff_ + already_read_, '\n') - buff_;
    std::string msg(buff_, pos);
    std::copy(buff_ + already_read_, buff_ + max_msg, buff_);
    already_read_ -= pos + 1;

    if (msg.find("login ") == 0) 
    {
        return on_login(msg);
        
    }
    else if(msg.find("pong ") == 0)
    {
        return on_ping(msg);
    }
    else if(msg.find("name") == 0) 
    {
        return on_username(msg);
    }
    // else if (msg.find("ask clients") == 0) on_clients();
    else return "invalid msg " + msg + "\n";
}

std::string talk_to_client::on_login(const std::string &msg)
{
    std::istringstream is(msg);
    is >> username_ >> username_;
    write("logging ok\n");
    set_client_status();

    return username_ + " logged in\n";
}

std::string talk_to_client::on_username(const std::string &msg)
{
    std::istringstream is(msg);
    is >> username_ >> username_;
    return username_;
}
std::string talk_to_client::on_ping(const std::string &msg)
{   
    // write(clients_changed_ ? "ping client_list_changed\n": "ping ok\n");
    clients_changed_ = false;

    std::istringstream is(msg);
    is >> username_ >> username_;
    return username_ + " pong\n";
}

void talk_to_client::write(const std::string& msg)
{
    socket_.write_some(buffer(msg));
}

void talk_to_client::set_name(const std::string &name)
{
    username_ = name;
}

bool talk_to_client::get_status() const
{
    return status_;
}


void accept_thread()
{
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
    while(true)
    {
        client_ptr new_(new talk_to_client());
        acceptor.accept(new_->sock());
        new_->login();

        std::string name = new_->handshake();
        boost::recursive_mutex::scoped_lock lk(cs);
        for (array::iterator b = clients.begin(), e = clients.end(); b!=e; ++b)
        {
            if ((*b)->username() == name)
            {
                new_->set_name(new_->username());
                new_->set_client_status();
                *b = new_;
                break;
            }
        }   
    }
}

// void handle_clients_thread()
// {
//     //переписать, потому что уже не клиент кидает запрос, а сервер сам дергает клиентов
//     while (true)
//     {
//         boost::this_thread::sleep(millisec(1));
//         boost::recursive_mutex::scoped_lock lk(cs);
//         for (array::iterator b = clients.begin(), e = clients.end(); b!=e; ++b)
//         (*b)->answer_to_client();
        
//         // clients.erase(std::remove_if(clients.begin(), clients.end(),
//         // boost::bind(&talk_to_client::timed_out, _1)), clients.end());
//     }
// }

std::string create_client(const std::string &username)
{
    boost::recursive_mutex::scoped_lock lock(cs);
    client_ptr new_(new talk_to_client());
    new_->set_name(username);
    clients.push_back(new_);
    return "Client card for " + username + " created\n";
}

std::string list_clients()
{
    std::string msg;
    boost::recursive_mutex::scoped_lock lock(cs);
    for (array::const_iterator b = clients.begin(), e = clients.end(); b!=e; ++b)
    {
        msg += (*b)->username() + " ";
    }

    return "clients " + msg + "\n";
}

std::string ping_client(const std::string &username)
{
    for (const auto &card : clients)
    {
        if (card -> username() == username)
        {
            if (!card -> get_status())
                return "Client " + username + " is offline.";
            return card -> ping_client();
        }
    }
    return "Client " + username + " not found.";
}

std::string exit(const std::string &username)
{
    for (const auto &card : clients)
    {
        if (card -> username() == username)
        {
            return card->stop();
        }
    }
    return "Client " + username + " not found.";
}

// int main(int argv, char* argc[])
// {
//     boost::thread_group threads;
//     threads.create_thread(accept_thread);
//     threads.create_thread(handle_clients_thread);
//     threads.join_all();
// }

