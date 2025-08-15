#ifndef BYTECAVESERVER_H
#define BYTECAVESERVER_H

#include <IByteCaveServer.h>
#include <asio.hpp>
#include <vector>
#include <boost/uuid.hpp>
#include <nlohmann/json.hpp>
#include <Chatroom.h>
#include <iostream>

using json = nlohmann::json;
using namespace std;
using asio::ip::tcp;

// Create an alias for sharedPtr for sockets
using socketPtr = shared_ptr<tcp::socket>;

// The class that will implement the ByteCaveServer interface
class ByteCaveServer : public IByteCaveServer
{
public:
    ByteCaveServer(unsigned short port);

    // Start listening method
    void pollContext() override;

    // Accept sockets internally
    void acceptSocketsAsync() override;

    // Method to send Messages to clients
    void sendMessage(std::shared_ptr<asio::ip::tcp::socket> socket,std::string message) override;

    // Method to send Messages to clients
    void sendMessage(std::shared_ptr<asio::ip::tcp::socket> socket,boost::uuids::uuid uuid) override;

    // Method to read messages from clients
    void readMessage(User user) override;

    // Clients is a vector of Users that has a uuid, username and a shared_ptr to the socket
    vector<User> allUsers;

    vector<Chatroom> allChatrooms;

    // A vector of chatrooms

// Setup the private members of the Server
private:
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
    unsigned short port;




};

#endif // BYTECAVESERVER_H
