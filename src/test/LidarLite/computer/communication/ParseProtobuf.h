#include "EthernetSocket.h"

#include "lidar_lite.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

#include "stdio.h"

class ParseProtobuf {
    private:
        std::unique_ptr<EthernetSocket> sock_;
        ResponseMessage response;

    public:
        void connect();
        void sendTestMessage(int num);
   	    int recieveTestMessage();
        ResponseMessage getResponseMessage();
};