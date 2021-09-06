#include "mbed.h"
#include "rtos.h"

#include <chrono>

#include "can_helper.h"
#include "communication/ParseProtobufMbed.h"
 
Ticker ticker;
Ticker printTimer;

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalIn sw1(p8);

CAN can1(p9, p10);

char counter = 0;

bool ready = false;
bool printVal = false;

int main();
void send();
// void togglePrint();
void handle_ethernet();
void handle_can();
void handle_print();

ParseProtobufMbed eth;

Mutex request_message_mutex;
Mutex request_message_ready_mutex;

Thread ethernet_thread;
Thread can_thread;
Thread print_thread;

bool first = true;

int main() {

    led1 = 1;
    led2 = 1;

    printf("main()\n");

    // Print timervoid togglePrint() {
    // printVal = true;

    eth.connect();

    ethernet_thread.start(handle_ethernet);
    // print_thread.start(handle_print);
    //can_thread->start(handle_can);
    
    while (true) {}
}

void handle_print() {

    int printCount1 = 0;
    int printCount2 = 0;
    int count = 0;

    while (true){ 

        // Print out thread information
        if (first) {
            printf("Ethernet Thread: %x\n", ethernet_thread.get_id());
            printf("Print Thread: %x\n", print_thread.get_id());
            first = false;
        }

        request_message_ready_mutex.lock();

        if (eth.get_request_message_ready() == true) {
            eth.set_request_message_ready(false);
            request_message_ready_mutex.unlock();

            led1 = 1;

            printf("\n");

            request_message_mutex.lock();
            RequestMessage message = eth.getRequestMessage();
            
            printf(
                "axis id: %x, can id: %x, cmd id: %x, ",
                message.axis_id,
                message.can_id,
                message.cmd_id
            );

            if (message.has_unsigned_int_request ||
                    message.has_signed_int_request) {
                printf("data: %d\n", message.unsigned_int_request);
            } else if (message.has_float_request) {
                printf("requesting float!\n");
            }
            request_message_mutex.unlock();
        } else {
            request_message_ready_mutex.unlock();

            led2 = 0;

            /*
            if (printCount1 > 100) {
                printf("%d\r", count);
            } else {
                printCount1++;
            }
            */
        } 
        
    }
}

void handle_ethernet() {
    while (true){

        led1 = 0;

        request_message_mutex.lock();
        request_message_ready_mutex.lock();

        eth.recieveComputerMessage();

        // led2 = 0;
        
        if (eth.getRequestMessage().has_ack == false) 
            eth.set_request_message_ready(true);

        request_message_ready_mutex.unlock();
        request_message_mutex.unlock();

        ThisThread::sleep_for(5ms);
    } 
}

void handle_can() {
    // Ticker
    ticker.attach(&send, 1.0);

    // cmd_id = 0x17 works
    // cmd_id = 0x0F works
    int axis_id = 0x03;
    int cmd_id = 0x00D;
    int can_id = axis_id << 5 | cmd_id;

    int id_mask = 0b11111;

    float payload = 5;

    // set baud rate to 500 kbps
    can1.frequency(500000);

    while (true) {

        CANMessage msg;

        if (sw1) {
            payload = 10;
            printf("10\n");
        } else {
            printf("5\n");
            payload = 5;
        }
    
        if (ready) {

            // CANMessage: leaving third argument blank creates a request 
            
            if (can1.write(CANMessage(can_id, (char *)(&payload)))) {
            // if (can1.write(CANMessage(can_id))) {
                counter++;
                // printf("Message sent: %d\n", counter);
            }
            

            ready = false;
        } else if (can1.read(msg)) {
            
            /*
            printf("Message received! %x, %x, %x, %x\n", 
                msg.data[0], msg.data[1], msg.data[2], msg.data[3]);
            */    

            led1 = !led1;

            if (msg.id == 0x69) {
                // printf("Heartbeat message!\n");
            } else if ((msg.id & id_mask) == 0x17){
                led2 != led2;
                //printf("Voltage value! ID: %x\n", msg.id & id_mask);

                // Litte endian
                printf("Data: %x, %x, %x, %x\n", 
                    msg.data[0], msg.data[1], msg.data[2], msg.data[3]);
                // printf("Data: %2.6f\n", msg.data);
            }
        } 
    }
}

void send() {
    ready = true;
}

void togglePrint() {
    // printVal = true;
    led2 != led2;
}