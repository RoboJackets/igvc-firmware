#include "EthernetSocket.h"

#include "swerve_commands.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

#include "stdio.h"

class ParseProtobuf {
    private:
        std::unique_ptr<EthernetSocket> sock_;
        ResponseMessage response;

    public:
        void connect();
        void send_message(int can_id, int axis_id, int cmd_id);
        void send_message(int can_id, int axis_id, int cmd_id, uint32_t data);
        void send_message(int can_id, int axis_id, int cmd_id, int data);
        void send_message(int can_id, int axis_id, int cmd_id, float data);
   	    int recieve_message();
        ResponseMessage getResponseMessage();
};