#include "ParseProtobufMbed.h"

int ParseProtobufMbed::connect() {
    printf("Connecting...\r\n");

    setupMbedIP();
    setupServerSocket();
    setupClientSocket();

    return 0;
}

void ParseProtobufMbed::disconnect() {
    serverSocket->close();
    clientSocket->close();
    net->disconnect();
}

bool ParseProtobufMbed::is_connected() {
    int val = net->get_connection_status();
    printf("Val: %d\n", val);
    return true;
}

void ParseProtobufMbed::sendMbedMessage(ResponseMessageData responseMessageData) {
    /* protocol buffer to hold response message */
    ResponseMessage response = ResponseMessage_init_zero;
    
    /* This is the buffer where we will store the response message. */
    uint8_t responsebuffer[256];
    size_t response_length;
    bool ostatus;
    
    /* Create a stream that will write to our buffer. */
    pb_ostream_t ostream =
        pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));
   
//    // Response Message Test
//    response.has_test = true;
// //    response.test = 16;
//     response.test = (uint32_t)responseMessageData.duration;


    // TODO
    response.has_buffer             = false;
    response.has_fl_velocity_est    = true;
    response.has_fr_velocity_est    = true;
    response.has_bl_velocity_est    = true;
    response.has_br_velocity_est    = true;
    response.has_fl_angle_est       = false;
    response.has_fr_angle_est       = false;
    response.has_bl_angle_est       = false;
    response.has_br_angle_est       = false;
    response.has_duration           = true;

    response.duration = responseMessageData.duration;

    response.fl_velocity_est = responseMessageData.fl_velocity_est;
    response.fr_velocity_est = responseMessageData.fr_velocity_est;
    response.bl_velocity_est = responseMessageData.bl_velocity_est;
    response.br_velocity_est = responseMessageData.br_velocity_est;

    // // DEBUG
    // uint durPrint = *(unsigned int*)&responseMessageData.duration;
    // uint encPrint = *(unsigned int*)&responseMessageData.br_velocity_est;
    // printf("DUR: 0x%x\n", durPrint);
    // printf("ENC: 0x%x\n", encPrint);

    // Encode the protobuffer
    pb_encode(&ostream, ResponseMessage_fields, &response);
    response_length = ostream.bytes_written;

    // Send the message using TCP
    clientSocket->send(reinterpret_cast<char*>(responsebuffer), response_length);
}

RequestMessage ParseProtobufMbed::recieveComputerMessage() {
    
    char buffer[BUFFER_SIZE];

    /* read data into the buffer. This call blocks until data is read */
    // int n = clientSocket->recv(buffer, bufferSize - 1);
    // int n = clientSocket->recv(buffer, sizeof(buffer) - 1);
    int n = clientSocket->recv(buffer, sizeof(buffer) - 1);
//    printf("Buffer Size: %d\n", n);

    /* protobuf message to hold request from client */
    RequestMessage requestMessage = RequestMessage_init_zero;

    if (n < 0) {
        return requestMessage;
    } else if (n == 0) {
        requestMessage.has_buffer = true;
        requestMessage.buffer = -1;
        return requestMessage;
    }


    /* Create a stream that reads from the buffer. */
    pb_istream_t istream =
        // pb_istream_from_buffer(reinterpret_cast<uint8_t *>(buffer), n);
        pb_istream_from_buffer(reinterpret_cast<uint8_t *>(buffer), n);


    /* decode the message */
    bool istatus = pb_decode(&istream, RequestMessage_fields, &requestMessage);
    
//    if (requestMessage.has_axis_id == true) {
        // printf("Command!\n");
//         request_message_ready = true;
//    }

    request_message_ready = true;

    if (!istatus) {
//      printf("Decoding failed: %s\n", PB_GET_ERROR(&istream));
    }

//    return n;
    return requestMessage;
}

//RequestMessage ParseProtobufMbed::getRequestMessage() {
//    return requestMessage;
//}

int ParseProtobufMbed::setupMbedIP() {

    net = new EthernetInterface();

    if (net == nullptr || net->set_network(MBED_IP, NETMASK, COMPUTER_IP)) {
        printf("Error performing set_network().");
        return 1;
    }

    if (net == nullptr || net->connect()) {
        printf("Error performing connect().");
        return 1;
    }

    getMbedIPAddress();

    printf("b\n");

    return 0;
}

int ParseProtobufMbed::setupServerSocket() {

    serverSocket = new TCPSocket();

    if (net == nullptr || serverSocket == nullptr 
            || serverSocket->open(net)) {
        printf("Error opening TCPSocket.");
        return 1;
    }
    
    if (serverSocket == nullptr || 
            serverSocket->bind(SERVER_PORT)) {
        printf("Error binding TCPSocket.");
        return 1;
    }
    
    if (serverSocket == nullptr || serverSocket->listen(1)) {
        printf("Error listening.");
        return 1;
    }

    printf("Waiting for new connection...\r\n");

    return 0;
}

int ParseProtobufMbed::setupClientSocket() {
    clientSocket = serverSocket->accept(); 

    SocketAddress socketAddress;
    clientSocket->getpeername(&socketAddress);
    printf("Accepted client from %s\r\n", socketAddress.get_ip_address());

    return 0;
}

void ParseProtobufMbed::getMbedIPAddress() {
    SocketAddress ip;

    if (net == nullptr) {
        printf("net is null");
    }
    net->get_ip_address(&ip);

    printf("MBED's IP address is: %s\r\n", ip.get_ip_address() 
            ? ip.get_ip_address() : "No IP\r\n");
}

bool ParseProtobufMbed::get_request_message_ready() {
    return request_message_ready;
}

void ParseProtobufMbed::set_request_message_ready(bool val) {
    request_message_ready = val;
}