#include "EthernetSocket.h"

#include "swerve_commands.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

#include "stdio.h"

class ParseProtobuf {
    private:
        std::unique_ptr<EthernetSocket> sock_;
        RequestMessage request;
        ResponseMessage response;

        bool request_message_ready = false;

    public:
        void connect();
        void send_message();
        void send_message(RequestMessage *message);
        void populate_ack();
        void populate_message(int can_id, int axis_id, int cmd_id);
        void populate_message(int can_id, int axis_id, int cmd_id, uint32_t data);
        void populate_message(int can_id, int axis_id, int cmd_id, int data);
        void populate_message(int can_id, int axis_id, int cmd_id, float data);
   	    int recieve_message();
        ResponseMessage getResponseMessage();
        bool get_request_message_ready();
        void set_request_message_ready(bool val);
};