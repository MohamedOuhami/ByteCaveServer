#ifndef IBYTECAVESERVER_H
#define IBYTECAVESERVER_H
#include <asio.hpp>

// Here, we define the interface for the ByteCave Server with the methods to implement
class IByteCaveServer {
public :

    // Method to start the server and listen for incoming connection requests
    virtual void pollContext() = 0;

    // Method to accept incoming sockets
    virtual void acceptSocketsAsync() = 0;

    // A Method to send messages to clients
    virtual void readMessage() = 0;

    // A method to read messages from clients
    virtual void sendMessage(std::shared_ptr<asio::ip::tcp::socket> socket,std::string message) = 0;

    // Destructor
    virtual ~IByteCaveServer() = default;
};

#endif // IBYTECAVESERVER_H
