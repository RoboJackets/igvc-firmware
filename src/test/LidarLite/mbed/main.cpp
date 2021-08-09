#include <stdio.h>
#include "communication/ParseProtobufMbed.h"
#include "lidar_lite/LidarLiteController.h"

#include "rtos.h"

ParseProtobufMbed eth;

int main() {

    eth = ParseProtobufMbed(); 
    eth.connect();

    LidarLiteController();

    printf("Done! \n");
    
    while (true) {
        ThisThread::sleep_for(1s);

        int *arr = LidarLiteController::getRawDistances();

        printf("lidar1 %d : lidar2 %d : lidar3 %d \n", arr[0], arr[1], arr[2]);
        
        eth.sendMbedMessage();
    }
    
}