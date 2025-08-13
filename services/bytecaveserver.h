#ifndef BYTECAVESERVER_H
#define BYTECAVESERVER_H

#include <IByteCaveServer.h>
#include <asio.hpp>

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

    std::vector<std::shared_ptr<asio::ip::tcp::socket>> getConnectedClients();

// Setup the private members of the Server
private:
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor;
    unsigned short port;

    // Create a vector of shared pointers of sockets to make the sockets live beyond the scope of the function that accepted It
    std::vector<std::shared_ptr<asio::ip::tcp::socket>> clients;


};

#endif // BYTECAVESERVER_H
