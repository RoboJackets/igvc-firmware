#include "mbed.h"
// #include "utils.h"

#include <stdio.h>

#include "lidar_lite.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

#include "nsapi_types.h"

#include "../lidar_lite/LidarLiteController.h"
#include "../utils.h"

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
        int recieveComputerMessage();
        void getMbedIPAddress();
        void getComputerIPAddress();
        bool requestHasFib();
        RequestMessage getRequestMessage();
};

