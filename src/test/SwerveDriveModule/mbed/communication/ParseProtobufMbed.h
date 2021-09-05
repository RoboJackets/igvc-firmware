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
constexpr const char *MBED_IP = "192.168.1.20";
constexpr const char *NETMASK = "255.255.255.0";
constexpr const char *COMPUTER_IP = "192.168.1.21";

class ParseProtobufMbed {
    private:
        EthernetInterface *net = nullptr;
        TCPSocket *serverSocket = nullptr;
        TCPSocket *clientSocket = nullptr; 

        RequestMessage requestMessage;

        int setupMbedIP();
        int setupServerSocket();
        int setupClientSocket(); 
    public:
        int connect(); 
        void disconnect();
        bool is_connected();
        void sendMbedMessage();
        int recieveComputerMessage(Mutex *request_message_mutex);
        void getMbedIPAddress();
        void getComputerIPAddress();
        bool requestHasFib();
        RequestMessage getRequestMessage();
};

