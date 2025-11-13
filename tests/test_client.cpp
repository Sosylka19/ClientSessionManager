#include <cassert>
#include <iostream>

#define private public
#define protected public
#include "../client/client_core.h"
#undef private
#undef protected

// extern boost::asio::io_context service;

int main() {
    talk_to_server cli("TestUser");
    assert(cli.username() == "TestUser");

    cli.already_read_ = 5;
    cli.buff_[0] = 'p';
    cli.buff_[1] = 'i';
    cli.buff_[2] = 'n';
    cli.buff_[3] = 'g';
    cli.buff_[4] = '\n';

    boost::system::error_code ec;
    size_t r = cli.read_complete(ec, cli.already_read_);
    assert(r == 0);
    assert(cli.already_read_ == 5);

    std::cout << "client tests passed\n";
    return 0;
}