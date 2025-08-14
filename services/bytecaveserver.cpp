#include "bytecaveserver.h"

#include <asio.hpp>
#include <iostream>
#include <string>

using namespace std;

// This is the constructor for the ByteCaveServer, which takes the port as an argument
// After that, we got what's called "Initializers list" which run before the constructor body
// This is done because the acceptor should be initialized when creating the object,not after
// We give the port member, the argument port
// For the acceptor, its constructor needs 2 arguments , an io_context which was initiated in the header and an endpoint
// An endpoiny itself, needs an ip, hence the v4 which takes the ip address of the machine, and  the 2ns argument is a port
ByteCaveServer::ByteCaveServer(unsigned short port) : port(port),acceptor(io_context,asio::ip::tcp::endpoint(asio::ip::tcp::v4(),port))
{
}

// Make the io_context run only the ready handlers without blocking the whole app
// The poll method runs all the ready-to-run handlers right now, without waiting for new events
// A handler is basically a lambda function or a callback that will perform when an event occurs like a client connecting, receiving a message etc ..
// But, what is a ready handler ???
// You see when you called async_accept, you gave It a callback function
// When nobody is connecting yet, that method is unready. It is idle and add to ASIO's internal queue
// The moment a client wants to connect, ASIO puts that lambda into a ready position with the parameters it needs, and waits for you to call poll again
void ByteCaveServer::pollContext(){
    io_context.poll();
}

// The internal accept
void ByteCaveServer::acceptSocketsAsync(){

    // Create a shared socket with a shared_ptr
    auto socket = std::make_shared<asio::ip::tcp::socket>(io_context);

    // So the async_accept function takes 2 argument
    // 1- The address of the socket that we will assign the connection to
    // 2- A completion handler
    // A completion handler is a lambda function, and it represents the function that the async_accept func will launch when It finishes its async operation
    // A lambda function in Cpp goes in the following form :
    // [captures](parameters) {body}
    // The captures tells the lambda which variables in the scope that It can use, If we don't specify, It will not be able to reach It
    // The parameters, is well the parameters of the function
    // In our case, the captures are this object to use access its member "acceptSocketsAsync", and the socket itself
    // As paremeters, we have the ec, which is inserted implicitely by the async_accept

    acceptor.async_accept(*socket,[this,socket](std::error_code ec){

        // If there is no ec, means that the connection was successful
        if (!ec){

            // Here, we need to get the username
            // Can I send the username with the socket ??
            std::cout << "My Client connected from " << socket->remote_endpoint() << std::endl;

            // Wait for the username message
            // We created the shared_ptr for the usernamebuff to stay alive ahead of the lambda
            auto username_buff = make_shared<array<char,64>>();


            // Async_accept is an async function, so we need to make sure that the processing of waiting the username is non-blocking

            cout << "Getting the username" << endl;
            socket->async_read_some(asio::buffer(*username_buff),[this,username_buff,socket](error_code ec,size_t len){

                if (!ec){
                    string username(username_buff->data(),len);

                    cout << "Got the username " << username << endl;

                    // Added the new user to the vector of connected users
                    clients[username] = socket;

                    sendMessage(socket,"Hello from ByteCaveServer!\n");
                }

                else {
                    cout << "There was an error " << ec.message();
                }
            });

        }

        // Rewait for another connection
        acceptSocketsAsync();
    });
}

// Implementation of the sending messages to clients
void ByteCaveServer::sendMessage(std::shared_ptr<asio::ip::tcp::socket> socket,std::string message){
    asio::write(*socket,asio::buffer(message));
}

// Implementation of the reading messages coming from clients
void ByteCaveServer::readMessage(){

}

// Get the number of clients
unordered_map<string,socketPtr> ByteCaveServer::getConnectedClients(){
    return clients;
}
