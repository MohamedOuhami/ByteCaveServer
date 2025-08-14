#ifndef BYTECAVESERVER_H
#define BYTECAVESERVER_H

#include <IByteCaveServer.h>
#include <asio.hpp>

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

    // Method to read messages from clients
    void readMessage() override;

    unordered_map<string,socketPtr> getConnectedClients();

// Setup the private members of the Server
private:
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
    unsigned short port;

    // Create a vector of shared pointers of sockets to make the sockets live beyond the scope of the function that accepted It
    // Clients is a vector of a hashed map of usernames and sockets
    unordered_map<string,socketPtr> clients;


};

#endif // BYTECAVESERVER_H
