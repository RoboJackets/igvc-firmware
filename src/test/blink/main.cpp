#include "mbed.h"
#include <stdio.h>

DigitalOut myled(LED1);

int main() {

    printf("Hello World!\n");

    while(1) {
        myled = 1;
        ThisThread::sleep_for(50);
        myled = 0;
        ThisThread::sleep_for(50);
    }
}
