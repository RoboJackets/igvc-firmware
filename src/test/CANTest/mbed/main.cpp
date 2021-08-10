#include "mbed.h"
#include "rtos.h"
#include <chrono>
 
Ticker ticker;
DigitalOut led1(LED1);
DigitalOut led2(LED2);
CAN can1(p9, p10);
CAN can2(p30, p29);
char counter = 0;
char ready = 0;

int main();
void send();

int main() {
    printf("main()\n");
    ticker.attach(&send, 1.0);
    CANMessage msg;
    while(1) {
        printf("loop()\n");

        if (ready) {
            if (can1.write(CANMessage(1337, &counter, 1))) {
                printf("wloop()\n");
                counter++;
                printf("Message sent: %d\n", counter);
                led1 = !led1;
            }
    
            ready = 0;
        } else if (can2.read(msg)) {
            printf("Message received: %d\n", msg.data[0]);
            led2 = !led2;
        } 

        ThisThread::sleep_for(200ms);
    }
}

void send() {
    ready = 1;
}