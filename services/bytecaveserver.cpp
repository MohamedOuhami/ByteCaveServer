#include "bytecaveserver.h"

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
            cout << "My Client connected from " << socket->remote_endpoint() << std::endl;

            // Wait for the username message
            // We created the shared_ptr for the usernamebuff to stay alive ahead of the lambda
            auto username_buff = make_shared<array<char,64>>();


            // Async_accept is an async function, so we need to make sure that the processing of waiting the username is non-blocking


            socket->async_read_some(asio::buffer(*username_buff),[this,username_buff,socket](error_code ec,size_t len){

                if (!ec){
                    string username(username_buff->data(),len);

                    // Generate the new uuid
                    boost::uuids::random_generator randomGen;
                    boost::uuids::uuid uuid = randomGen();

                    // Create the new User
                    User newUser {uuid,username,socket};

                    cout << "Creating new User with username : " << newUser.username
                         << " and a uuid : "
                         << newUser.uuid
                         << " and socket " << newUser.socketPtr
                         << endl;

                    // Add the new user to the allClients
                    allUsers.push_back(newUser);

                    // Send the new UUID to the client
                    sendMessage(socket,boost::uuids::to_string(uuid));

                    for (auto user : allUsers){
                        readMessage(user);
                    }
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

// Implementation of the sending messages to clients
void ByteCaveServer::sendMessage(std::shared_ptr<asio::ip::tcp::socket> socket,boost::uuids::uuid uuid){
    asio::write(*socket,asio::buffer(uuid));
}

// Implementation of the reading messages coming from clients
void ByteCaveServer::readMessage(User user){
    // Read from the buffer

    auto buffer = make_shared<array<char,1024>>();

    auto &socket = *user.socketPtr;

    socket.async_read_some(
        // The buffer to read to
        asio::buffer(*buffer),
        // The callback to perform when receiving a message
        [this,buffer,user](error_code ec,size_t length){
            if(!ec){

                // If the
                string msg(buffer->data(),length);

                // Find the command end
                auto commandEnd = msg.find("\n");
                if(commandEnd != string::npos){
                    // Stripe the command name from the message
                    string command = msg.substr(0,commandEnd);

                    // Find the size line
                    auto sizeEnd = msg.find("\n",commandEnd+1);
                    if(sizeEnd != string::npos){
                        size_t jsonSize = stoul(msg.substr(commandEnd+1,sizeEnd-commandEnd+1));

                        // Get the paylaod
                        string jsonPayload = msg.substr(sizeEnd+1);

                        // Make sure that we have no missing or incomplete data
                        if (jsonPayload.size() >= jsonSize){

                            // Parse the data
                            auto j = json::parse(jsonPayload.substr(0,jsonSize));

                            // Check the type of command
                            if (command == "CREATE_CHATROOM"){

                                string name = j["name"];
                                string description = j["description"];
                                string owner_uuid = j["chatroomClients"][0];

                                cout << "Chatroom name : " << j["name"] << endl;
                                cout << "Chatroom description : " << j["description"] << endl;
                                cout << "Chatroom owner : " << j["chatroomClients"][0] << endl;

                                // Parse the owner uuid string to uuid
                                boost::uuids::string_generator generator;
                                boost::uuids::uuid id = generator(owner_uuid);

                                // Find the user using the uuid
                                auto it = find_if(allUsers.begin(),allUsers.end(),
                                                  [&id](const User& user){
                                                      return user.uuid == id;
                                });

                                if (it != allUsers.end()){
                                    User& foundUser = *it;
                                    cout << "Found the user " << foundUser.username;

                                    // Instantiate a new Chatroom
                                    Chatroom newChatroom {name,description};
                                    newChatroom.connectedUsers.push_back(foundUser);

                                    allChatrooms.push_back(newChatroom);
                                }
                                else {
                                    cout << "Could not find the user";
                                }


                            }


                        }
                    }
                }

                // Redo the loop
                readMessage(user);
            }
            else {
                // Return that there was an error
            }
        });
}

