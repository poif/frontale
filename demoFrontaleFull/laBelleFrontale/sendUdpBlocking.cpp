#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::udp;

enum { max_length = 1024 };

std::string sendUDP(const std::string& message, std::string& host, std::string& port){
    try
  {

    boost::asio::io_service io_service;

    udp::socket s(io_service, udp::endpoint(udp::v4(), 0));

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), host, port);
    udp::resolver::iterator iterator = resolver.resolve(query);
/*
    using namespace std; // For strlen.
    std::cout << "Enter message: ";
    char request[max_length];
    std::cin.getline(request, max_length);
*/

    //size_t request_length = strlen(request);
    s.send_to(boost::asio::buffer(message.data(), message.size()), *iterator);

    char reply[max_length];
    udp::endpoint sender_endpoint;
    size_t reply_length = s.receive_from(
        boost::asio::buffer(reply, max_length), sender_endpoint);
    std::cout << "Reply is: ";
    std::cout.write(reply, reply_length);
    std::cout << "\n";
    std::string replyStr = reply;
    return replyStr;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

}