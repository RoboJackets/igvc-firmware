//
// Created by robonav on 12/19/21.
//

#ifndef IGVC_FIRMWARE_MOTORCOMMAND_H
#define IGVC_FIRMWARE_MOTORCOMMAND_H

#include "../can/CANCommon.h"
#include "../communication/ParseProtobufMbed.h"

#include "swerve_commands.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

class MotorCommand {

private:

    RequestMessage motorMessage;
    CANCommon *canCommon;

    int motorCANMessages[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
    uint motorPayloads[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int motorCANMessagesSize = 0;

    void translateMotorMessage(CANCommon *canCommon);

public:

    MotorCommand(ParseProtobufMbed *eth, CANCommon *canCommon);
    void sendMotorMessages();
    void printMotorMessages();
};


#endif //IGVC_FIRMWARE_MOTORCOMMAND_H
