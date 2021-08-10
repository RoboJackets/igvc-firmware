#include <stdio.h>
#include "communication/ParseProtobuf.h"

ParseProtobuf eth;

int main() {

    ParseProtobuf eth = ParseProtobuf(); 
    eth.connect();
    
    while (true) {
        eth.recieveTestMessage();

        printf("Lidar 1: %d, Lidar 2: %d \n",
            eth.getResponseMessage().lidar1,
            eth.getResponseMessage().lidar2);
    }
}