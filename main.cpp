#include <QCoreApplication>
#include <QTimer>
#include "bytecaveserver.h"
#include <iostream>
#include <User.h>

// Declare functions used in the main script
void getUserInput(ByteCaveServer &byteCaveServer);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Instantiating the ByteCaveServer
    ByteCaveServer byteCaveServer(8080);

    // Wait for a connection, but It is not blocking, It has an unready handle for the poll
    byteCaveServer.acceptSocketsAsync();

    // Use QTimer to periodically poll ASIO ( Process the ready handles )
    QTimer timer;

    // The connect function is responsible of linking a signal ( An event that happens ) to a slt ( a function to call )
    // It takes for arguments : A sender, a signal and a function
    // The sender here is the timer object of type QTimer that we instantiated
    // The signal that it is going to send is a timeout signal
    // And the function that It will run, the [&] captures, means to capture all local variables by reference


    QObject::connect(&timer,&QTimer::timeout,[&](){
        byteCaveServer.pollContext();
    });

    // Set the timer every 0 seconds, So it sends a timeout as long as the loop goes on and on
    timer.start(0);

    // We create another thread to get the user's input
    std::thread(getUserInput,std::ref(byteCaveServer)).detach();


    return a.exec();
}



void getUserInput(ByteCaveServer& byteCaveServer) {
    while(true){
        std::cout << "Please, enter an operation to do: " << std::flush;
        std::string operation;

        if (std::getline(std::cin, operation)) {
            if (operation == "1"){

                // Send message through all the sockets
                for(User user : byteCaveServer.allUsers){
                    if (user.socketPtr && user.socketPtr->is_open()){
                        // Send a message to all the open sockets
                        byteCaveServer.sendMessage(user.socketPtr,"Hello from the server\n");
                    }
                }

            }
            else if (operation == "2"){

                size_t openClients = 0;
                for(auto user : byteCaveServer.allUsers){
                    std::cout << "In the vector, you have the following sockets\n";
                    if (user.socketPtr && user.socketPtr->is_open()){
                        std::cout << user.socketPtr << ", its open status :" << user.socketPtr->is_open();
                        openClients++;
                    }
                }
                std::cout << "You have " << openClients << " clients connected\n";
            }

        }
        else {
            break; // EOF or error
        }
    }
}
