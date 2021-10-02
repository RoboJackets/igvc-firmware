#include "ParseProtobuf.h"

void ParseProtobuf::connect() {

    sock_ =  std::make_unique<EthernetSocket>("192.168.1.20", 5333);
}

void ParseProtobuf::send_message() {
    uint8_t buffer[256];
    size_t message_length;

    /* Create a stream that will write to our buffer. */
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    /* encode the protobuffer */
    pb_encode(&ostream, RequestMessage_fields, &request);
    message_length = ostream.bytes_written;

    /* Send the message strapped to a pigeon's leg! */
    (*sock_).sendMessage(reinterpret_cast<char*>(buffer), message_length);
}

void ParseProtobuf::populate_ack() {
    request = RequestMessage_init_zero;

    request.has_ack = true;
    request.has_axis_id = false;
    request.has_can_id = false;
    request.has_cmd_id = false;
    request.has_float_request = false;
    request.has_signed_int_request = false;
    request.has_unsigned_int_request = false;

    request.ack = 1;
}

void ParseProtobuf::populate_message(int can_id, int axis_id, int cmd_id) {
    request = RequestMessage_init_zero;

    // Fill out request message
    request.has_ack = false;
    request.has_axis_id = true;
    request.has_can_id = true;
    request.has_cmd_id = true;
    request.has_float_request = false;
    request.has_signed_int_request = false;
    request.has_unsigned_int_request = false;

    request.axis_id = static_cast<uint32_t>(axis_id);
    request.can_id = static_cast<uint32_t>(can_id);
    request.cmd_id = static_cast<uint32_t>(cmd_id);

    set_request_message_ready(true);
}

void ParseProtobuf::populate_my_message(int can_id, int axis_id, int cmd_id, uint32_t data) {
    request = RequestMessage_init_zero;

    // Fill out request message
    request.has_ack = false;
    request.has_axis_id = true;
    request.has_can_id = true;
    request.has_cmd_id = true;
    request.has_float_request = false;
    request.has_signed_int_request = false;
    request.has_unsigned_int_request = true;

    request.axis_id = static_cast<uint32_t>(axis_id);
    request.can_id = static_cast<uint32_t>(can_id);
    request.cmd_id = static_cast<uint32_t>(cmd_id);
    request.unsigned_int_request = static_cast<uint32_t>(data);

    set_request_message_ready(true);
}

void ParseProtobuf::populate_message(int can_id, int axis_id, int cmd_id, int data) {
    request = RequestMessage_init_zero;

    // Fill out request message
    request.has_ack = false;
    request.has_axis_id = true;
    request.has_can_id = true;
    request.has_cmd_id = true;
    request.has_float_request = false;
    request.has_signed_int_request = true;
    request.has_unsigned_int_request = false;

    request.axis_id = static_cast<uint32_t>(axis_id);
    request.can_id = static_cast<uint32_t>(can_id);
    request.cmd_id = static_cast<uint32_t>(cmd_id);
    request.unsigned_int_request = static_cast<int>(data);

    set_request_message_ready(true);
}

void ParseProtobuf::populate_message(int can_id, int axis_id, int cmd_id, float data) {
    request = RequestMessage_init_zero;

    // Fill out request message
    request.has_ack = false;

    request.has_axis_id = true;
    request.has_can_id = true;
    request.has_cmd_id = true;
    request.has_float_request = true;
    request.has_signed_int_request = false;
    request.has_unsigned_int_request = false;

    request.axis_id = static_cast<uint32_t>(axis_id);
    request.can_id = static_cast<uint32_t>(can_id);
    request.cmd_id = static_cast<uint32_t>(cmd_id);

    // printf("0x%x\n", ()data);

    request.float_request = static_cast<float>(data);

    set_request_message_ready(true);
}

int ParseProtobuf::recieve_message() {
    /* Read response from the server */
    size_t n;             
    uint8_t buffer[256];  

    memset(buffer, 0, sizeof(buffer));
    /* read from the buffer */

    // blocks until data is read
    n = (*sock_).readMessage(buffer); 

    if (n == 0) {
        printf("Connection Closed!\n");
        return n;
    } 

    /* Allocate space for the decoded message. */
    response = ResponseMessage_init_zero;

    /* Create a stream that reads from the buffer. */
    pb_istream_t istream = pb_istream_from_buffer(buffer, n);

    /* decode the message. */
    pb_decode(&istream, ResponseMessage_fields, &response);

    return n;
}

ResponseMessage ParseProtobuf::getResponseMessage() {
    return response;
}

bool ParseProtobuf::get_request_message_ready() {
    return request_message_ready;
}

void ParseProtobuf::set_request_message_ready(bool val) {
    request_message_ready = val;
}
