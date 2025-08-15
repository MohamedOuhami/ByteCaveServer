#ifndef USER_H
#define USER_H

#include <string>
#include <asio.hpp>
#include <boost/uuid.hpp>

using namespace std;
using asio::ip::tcp;
// The struct that represents the users connected to the server
struct User {

    boost::uuids::uuid uuid;
    string username;

    // A pointer to the socket created with the client
    shared_ptr<tcp::socket> socketPtr;
};

#endif // USER_H
