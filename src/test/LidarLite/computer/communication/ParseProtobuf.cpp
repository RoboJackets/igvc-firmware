#include "ParseProtobuf.h"

void ParseProtobuf::connect() {

    sock_ =  std::make_unique<EthernetSocket>("192.168.1.20", 5333);
}

void ParseProtobuf::sendTestMessage(int num) {
    uint8_t buffer[256];
    size_t message_length;
    
    RequestMessage request = RequestMessage_init_zero;

    /* Create a stream that will write to our buffer. */
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    
    // Fill out request message
    request.has_fibRequest = true;
    request.fibRequest = static_cast<uint32_t>(num);

    /* encode the protobuffer */
    pb_encode(&ostream, RequestMessage_fields, &request);
    message_length = ostream.bytes_written;

    /* Send the message strapped to a pigeon's leg! */
    (*sock_).sendMessage(reinterpret_cast<char*>(buffer), message_length);
}

int ParseProtobuf::recieveTestMessage() {
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

