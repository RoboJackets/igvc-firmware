#ifndef UTILS_H
#define UTILS_H

#include "EthernetInterface.h"

/* ethernet setup variables */
constexpr int SERVER_PORT = 5333;
constexpr int BUFFER_SIZE = 256;
constexpr const char *MBED_IP = "192.168.1.20";
constexpr const char *NETMASK = "255.255.255.0";
constexpr const char *COMPUTER_IP = "192.168.1.21";

typedef struct EthernetInfo {
    EthernetInterface *net = nullptr;
                            /* Internet connection between the mbed
                             * and the computer.
                             */
    TCPSocket *serverSocket = nullptr;
                            /* TCP sockets to function as the client.
                              * Binded to the client port.
                              */
    TCPSocket *clientSocket = nullptr; 
                            /* TCP sockets to function as the server.
                              * Binded to the server port.
                              */ 
} EthernetInfo; 


#endif
