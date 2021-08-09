#include "ParseProtobufMbed.h"
#include "utils.h"

bool sendResponse(TCPSocket &client);

ParseProtobufMbed parseProtobufMbed;

int main() {

    while (1) {
        parseProtobufMbed = ParseProtobufMbed();

        parseProtobufMbed.connect(); 

        printf("Done\r\n");

        while (1) {

            if (parseProtobufMbed.recieveComputerMessage() != 0 && 
                    parseProtobufMbed.requestHasFib()) {

                printf("Success! Buffer contains: %d\n", 
                    parseProtobufMbed.getRequestMessage().fibRequest);
                parseProtobufMbed.sendMbedMessage(
                    parseProtobufMbed.getRequestMessage().fibRequest);
            } else {
                printf("Failure!\r\n");
            }
        }
    }
}
