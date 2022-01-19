
#include "mbed.h"
#include "rtos.h"

#include <chrono>

#include "can_helper.h"
#include "communication/ParseProtobufMbed.h"
#include "can/CANCommon.h"
#include "motors/MotorCommand.h"

Ticker ticker;
Ticker printTimer;

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalIn sw1(p8);

CAN can(p9, p10);

char counter = 0;

bool ready = false;
bool printVal = false;

int main();
void send();
// void togglePrint();

typedef struct _thread_data {
    ParseProtobufMbed *eth;
    CANCommon *canCommon;
} thread_data;

thread_data data;

void handle_ethernet(ParseProtobufMbed *eth);
void handle_can(ParseProtobufMbed *eth);
void handle_can_new(thread_data *data);
void handle_print(ParseProtobufMbed *eth);

ParseProtobufMbed eth;
CANCommon canCommon(&can);

Mutex request_message_mutex;

bool mbed_idle = true;
bool message_recv = false;

ConditionVariable mbed_idle_cond(request_message_mutex);
ConditionVariable message_recv_cond(request_message_mutex);

//Mutex request_message_ready_mutex;

Thread ethernet_thread;
Thread can_thread;
Thread print_thread;

bool first = true;


int main() {

    data.eth = &eth;
    data.canCommon = &canCommon;

    led1 = 1;
    led2 = 1;

    printf("main()\n");

    eth.connect();

    // Cannot start a print thread
    // Not enough memory
    // print_thread.start(callback(handle_print, &eth));

    ethernet_thread.start(mbed::callback(handle_ethernet, &eth));
    can_thread.start(mbed::callback(handle_can_new, &data));
    
    led1 = 0;
    led2 = 0;

    // handle_print(&eth);

    while (true) {}

    int printCount1 = 0;
    int printCount2 = 0;
    int count = 0;
    
//    while (true) {
//
//        // Print out thread information
//        if (first) {
//            printf("Ethernet Thread: %x\n", ethernet_thread.get_id());
//            printf("Print Thread: %x\n", print_thread.get_id());
//            first = false;
//        }
//
//        request_message_ready_mutex.lock();
//
//        if (eth.get_request_message_ready() == true) {
//            eth.set_request_message_ready(false);
//            request_message_ready_mutex.unlock();
//
//            led1 = 1;
//
//            printf("\n");
//
//            request_message_mutex.lock();
//            RequestMessage message = eth.getRequestMessage();
//
//            printf("axis id: %d, can id: %d, cmd id: %d, ", message.axis_id, message.can_id, message.cmd_id);
//
//            if (message.has_unsigned_int_request ||
//                    message.has_signed_int_request) {
//                printf("data: %d\n", message.unsigned_int_request);
//            } else if (message.has_float_request) {
//                printf("requesting float!\n");
//            }
//            request_message_mutex.unlock();
//        } else {
//            request_message_ready_mutex.unlock();
//        }
//    }
}

//void handle_print(ParseProtobufMbed *eth) {
//
//    int printCount1 = 0;
//    int printCount2 = 0;
//    int count = 0;
//
//    while (true){
//
//        // Print out thread information
//        if (first) {
//            printf("Ethernet Thread: %x\n", ethernet_thread.get_id());
//            printf("Print Thread: %x\n", print_thread.get_id());
//            first = false;
//        }
//
//        request_message_ready_mutex.lock();
//
//        if (eth->get_request_message_ready() == true) {
//            eth->set_request_message_ready(false);
//            request_message_ready_mutex.unlock();
//
//            led1 = 1;
//
//            printf("\n");
//
//            request_message_mutex.lock();
//            RequestMessage message = eth->getRequestMessage();
//
//            printf(
//                "axis id: %x, can id: %x, cmd id: %x, ",
//                message.axis_id,
//                message.can_id,
//                message.cmd_id
//            );
//
//            // if (message.has_unsigned_int_request ||
//            //         message.has_signed_int_request) {
//            //     printf("data: %d\n", message.unsigned_int_request);
//            // } else if (message.has_float_request) {
//            //     printf("requesting float!\n");
//            // }
//
//            if (message.has_unsigned_int_request ||
//                    message.has_signed_int_request) {
//                printf("data: %d\n", message.unsigned_int_request);
//            } else if (message.has_float_request) {
//                printf("data: 0x%x\n", *(unsigned int*)&message.float_request);
//            }
//
//            request_message_mutex.unlock();
//        } else {
//            request_message_ready_mutex.unlock();
//
//            led2 = 0;
//
//            /*
//            if (printCount1 > 100) {
//                printf("%d\r", count);
//            } else {
//                printCount1++;
//            }
//            */
//        }
//
//    }
//}

void handle_ethernet(ParseProtobufMbed *eth) {
    while (true){

        request_message_mutex.lock();
        while (mbed_idle == false) {
            mbed_idle_cond.wait();
        }

        bool no_errors = eth->recieveComputerMessage();

        // this means a valid message was recv
        if (no_errors) {
            // wait until the mbed is finished handling the request

            mbed_idle = false;
            message_recv = true;

            message_recv_cond.notify_all();
            request_message_mutex.unlock();

            continue;
        }

        request_message_mutex.unlock();

        ThisThread::sleep_for(100);
    }
}

void handle_can_new(thread_data *data) {

    ParseProtobufMbed *eth = data->eth;
    CANCommon *canCommon = data->canCommon;

    can.frequency(500000);

    while (true) {

        // wait until we have recv a message
        request_message_mutex.lock();
        while (message_recv == false) {
            message_recv_cond.wait();
        }

        // led indicator
//        led1 = 1;

        // process the message
        MotorCommand motorCommand(eth, canCommon);
        motorCommand.sendMotorMessages();
//        motorCommand.printMotorMessages();

        // we are done processing the message; notify the ethernet thread

        mbed_idle = true;
        message_recv = false;

        mbed_idle_cond.notify_all();
        request_message_mutex.unlock();
    }
}



//void handle_can(ParseProtobufMbed *eth) {
//    // Ticker
//    ticker.attach(&send, 1.0);
//
//    // cmd_id = 0x17 works
//    // cmd_id = 0x0F works
//    // int axis_id = 0x03;
//    // int cmd_id = 0x00D;
//    // int can_id = axis_id << 5 | cmd_id;
//
//    uint payload;
//    uint message_len;
//
//    int id_mask = 0b11111;
//
//    // set baud rate to 500 kbps
//    can.frequency(500000);
//
//    while (true) {
//
//        CANMessage msg;
//
//        // if (sw1) {
//        //     payload = 10;
//        //     printf("10\n");
//        // } else {
//        //     printf("5\n");
//        //     payload = 5;
//        // }
//
//        request_message_mutex.lock();
//
//        if (eth->get_request_message_ready() == true) {
//            eth->set_request_message_ready(false);
//
//            // CANMessage: leaving third argument blank creates a request
//
//            RequestMessage message = eth->getRequestMessage();
//
//            // axis_id = 0x3;
//            // cmd_id = message.cmd_id;
//            int can_id = (message.can_id << 5) | message.cmd_id;
//
//            if (message.has_unsigned_int_request) {
//                payload = message.unsigned_int_request;
//                message_len = -1;
//            } else if (message.has_signed_int_request) {
//                payload = message.signed_int_request;
//                message_len = -1;
//            } else if (message.has_float_request) {
//                payload = *(unsigned int*)&message.float_request;
//                message_len = 32;
//                // payload = 0x40a00000;
//            }
//
//            request_message_mutex.unlock();
//
//            if (message_len == -1) {
//                can1.write(CANMessage(can_id, (char *)(&payload)));
//            } else {
//                can1.write(CANMessage(can_id, (char *)(&payload), message_len));
//            }
//
//            // if (can1.write(CANMessage(can_id, (char *)(&payload)))) {
//            // if (can1.write(CANMessage(can_id))) {
//                // counter++;
//                //printf("Message sent: %d\n", counter);
//            // }
//
//            ready = false;
//        } else {
//
//            request_message_mutex.unlock();
//
//            if (can1.read(msg)) {
//
//            /*
//            printf("Message received! %x, %x, %x, %x\n",
//                msg.data[0], msg.data[1], msg.data[2], msg.data[3]);
//            */
//
//            if (msg.id == 0x69) {
//                // printf("Heartbeat message!\n");
//            } else if ((msg.id & id_mask) == 0x17){
//                // led2 != led2;
//                //printf("Voltage value! ID: %x\n", msg.id & id_mask);
//
//                // Litte endian
//                printf("Data: %x, %x, %x, %x\n",
//                    msg.data[0], msg.data[1], msg.data[2], msg.data[3]);
//                // printf("Data: %2.6f\n", msg.data);
//            }
//        }
//        }
//
//    }
//}
//
//void send() {
//    ready = true;
//}
//
//void togglePrint() {
//    // printVal = true;
//    // led2 != led2;
//}