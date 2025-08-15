#ifndef CHATROOM_H
#define CHATROOM_H
#include <string>
#include <asio.hpp>
#include <vector>
#include <User.h>

using namespace std;
using asio::ip::tcp;

// The struct for the chatroom in the server
struct Chatroom {

    string name;
    string description;

    // A vector of all connected clients
    vector<User> connectedUsers;
};

#endif // CHATROOM_H
