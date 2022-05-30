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

const float FL_DIR = 1; 
const float FR_DIR = -1;
const float BL_DIR = 1;
const float BR_DIR = -1;


typedef struct _EncoderEstimates {
    uint32_t fl_velocity_est;
    uint32_t fr_velocity_est;
    uint32_t bl_velocity_est;
    uint32_t br_velocity_est;
    uint32_t fl_angle_est;
    uint32_t fr_angle_est;
    uint32_t bl_angle_est;
    uint32_t br_angle_est;
} EncoderEstimates;


class MotorCommand {

private:

    RequestMessage motorMessage;
    CANCommon *canCommon;

    bool motorIsIdle[8] = {true, true, true, true, true, true, true, true};

    int motorCANMessages[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
    uint motorPayloads[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int motorCANMessagesSize = 0;

    void _calibrateSteeringMotor(CANCommon *_canCommon, int motorNum);
    void _enableSteeringMotor(CANCommon *_canCommon, int motorNum);

public:
    // constructor
    MotorCommand();

    // must be done on all steering motors before operation
    void steeringMotorStartup(CANCommon *_canCommon);
    void setAngle(float rot, CANCommon *_canCommon, int motorNum);
    void arduinoSetAngle(float rot, CAN *can);

    void newSendMotorMessages(RequestMessage requestMessage, CANCommon *_canCommon, int motorNum);
    void sendSteerMessage(RequestMessage requestMessage, CANCommon *_canCommon, int motorNum);
    void sendMotorMessages(RequestMessage requestMessage, CANCommon *_canCommon);
    void sendMotorMessages1(RequestMessage requestMessage, CANCommon *_canCommon);
    void sendMotorMessages2(RequestMessage requestMessage, CANCommon *_canCommon);
    void sendMotorMessages3(RequestMessage requestMessage, CANCommon *_canCommon);
    void translateMotorMessage(CANCommon *canCommon);
    void printMotorMessages();

    void getEncoderEstimates(ResponseMessageData *responseMessageData, CANCommon *canCommon);
};


#endif //IGVC_FIRMWARE_MOTORCOMMAND_H
