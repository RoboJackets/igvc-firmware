#include <stdio.h>
#include "ParseProtobuf.h"

ParseProtobuf eth;

int main() {

    ParseProtobuf eth = ParseProtobuf(); 
    eth.connect();
    
    while (true) {
        bool valid = false;
        int input;

        while (true) {
            printf("Please enter a number 0 - 9: ");
            scanf("%d", &input);

            if(input >= 0 && input <= 9) {
                break;
            } else {
                printf("Invalid input!\n");
            }
        }

        eth.sendTestMessage(input);
        eth.recieveTestMessage();

        printf("The %dth Fibonacci Number is %d\n", input, eth.getResponseMessage().fibResponse);
    }
    
}