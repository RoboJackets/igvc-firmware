#ifndef IGVC_FIRMWARE_PARSEPROTOBUFMBED_H
#define IGVC_FIRMWARE_PARSEPROTOBUFMBED_H

#include "mbed.h"
#include "rtos.h"

#include <stdio.h>

#include "swerve_commands.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

#include "nsapi_types.h"
#include "EthernetInterface.h"

constexpr int SERVER_PORT = 5333;
constexpr int BUFFER_SIZE = 256;
constexpr const char *MBED_IP = "192.168.8.20";
constexpr const char *NETMASK = "255.255.255.0";
constexpr const char *COMPUTER_IP = "192.168.8.21";

class ParseProtobufMbed {
    private:

        EthernetInterface *net;
        TCPSocket *serverSocket;
        TCPSocket *clientSocket; 

//        RequestMessage requestMessage;
        bool request_message_ready = false;

        int setupMbedIP();
        int setupServerSocket();
        int setupClientSocket(); 
    public:
        int connect(); 
        void disconnect();
        bool is_connected();
        void sendMbedMessage();
        RequestMessage recieveComputerMessage();
        void getMbedIPAddress();
        void getComputerIPAddress();
        bool requestHasFib();
        RequestMessage getRequestMessage();
        bool get_request_message_ready();
        void set_request_message_ready(bool val);
};

#endif

