//
// Created by robonav on 12/19/21.
//

#include "MotorCommand.h"
#include <cstdlib>
#include <math.h>

MotorCommand::MotorCommand() {

////    canCommon = _canCommon;
//
//    motorMessage = eth->getRequestMessage();
////    translateMotorMessage(_canCommon);

}


void MotorCommand::enableSteeringMotors(CANCommon *_canCommon) {
    for (int i = 1; i <= 4; i++) {
        _enableSteeringMotor(_canCommon, i);
    }
}

void MotorCommand::_enableSteeringMotor(CANCommon *_canCommon, int motorNum) {

    int steerCanId = 0;

    if (motorNum == 1) {
        steerCanId = FL_ANGLE_CAN_ID;
    } else if (motorNum == 2) {
        steerCanId = BL_ANGLE_CAN_ID;
    } else if (motorNum == 3) {
        steerCanId = FR_ANGLE_CAN_ID;
    } else {
        steerCanId = BR_ANGLE_CAN_ID;
    }

    // send command to steering motor
    int controlId = _canCommon->generateCANMessage(steerCanId, CAN_CMD_SET_AXIS_STATE);
    uint controlPayload = AXIS_STATE_CONTROL;
    _canCommon->sendCANMessage(controlId, (char*)&controlPayload, 32);
    motorIsIdle[2 * (motorNum - 1) + 1] = false;
}

void MotorCommand::sendSteerMessage(RequestMessage requestMessage, CANCommon *_canCommon, int motorNum) {

    float rot = 0;
    int steerCanId = 0;

    if (motorNum == 1) {
        rot = requestMessage.fl_angle;
        steerCanId = FL_ANGLE_CAN_ID;
    } else if (motorNum == 2) {
        rot = requestMessage.bl_angle;
        steerCanId = BL_ANGLE_CAN_ID;
    } else if (motorNum == 3) {
        rot = requestMessage.fr_angle;
        steerCanId = FR_ANGLE_CAN_ID;
    } else {
        rot = requestMessage.br_angle;
        steerCanId = BR_ANGLE_CAN_ID;
    }

    // convert rot to turns, multiply by 100 (the gear ratio)
    rot *= (100) * (1.0 / (2.0 * M_PI));

//    // send command to steering motor
//    if (motorIsIdle[2 * (motorNum - 1) + 1]) {
//        int controlId = _canCommon->generateCANMessage(steerCanId, CAN_CMD_SET_AXIS_STATE);
//        uint controlPayload = AXIS_STATE_CONTROL;
//        _canCommon->sendCANMessage(controlId, (char*)&controlPayload, 32);
//        motorIsIdle[2 * (motorNum - 1) + 1] = false;
//    } else {
//        int rotId = _canCommon->generateCANMessage(steerCanId, CAN_CMD_SET_POS);
//        uint rotPayload = *(unsigned int*)&rot;
//        _canCommon->sendCANMessage(rotId, (char*)&rotPayload, 32);
//    }

    int rotId = _canCommon->generateCANMessage(steerCanId, CAN_CMD_SET_POS);
    uint rotPayload = *(unsigned int*)&rot;
    _canCommon->sendCANMessage(rotId, (char*)&rotPayload, 32);

}

void MotorCommand::newSendMotorMessages(RequestMessage requestMessage, CANCommon *_canCommon, int motorNum) {

    float vel = 0;
    int driveCanId = 0;

    if (motorNum == 1) {
        vel = requestMessage.fl_velocity;
        driveCanId = FL_VEL_CAN_ID;
    } else if (motorNum == 2) {
        vel = requestMessage.bl_velocity;
        driveCanId = BL_VEL_CAN_ID;
    } else if (motorNum == 3) {
        vel = -1 * requestMessage.fr_velocity;
        driveCanId = FR_VEL_CAN_ID;
    } else {
        vel = requestMessage.br_velocity;
        driveCanId = BR_VEL_CAN_ID;
    }

    // send command to drive motor
    if (std::abs(vel) < 0.1) {
        if (!motorIsIdle[2 * (motorNum - 1)]) {
            int idleId = _canCommon->generateCANMessage(driveCanId, CAN_CMD_SET_AXIS_STATE);
            uint idlePayload = AXIS_STATE_IDLE;
            _canCommon->sendCANMessage(idleId, (char *) &idlePayload, 32);
            motorIsIdle[2 * (motorNum - 1)] = true;
        }
    } else {
        if (motorIsIdle[2 * (motorNum - 1)]) {
            int controlId = _canCommon->generateCANMessage(driveCanId, CAN_CMD_SET_AXIS_STATE);
            uint controlPayload = AXIS_STATE_CONTROL;
            _canCommon->sendCANMessage(controlId, (char*)&controlPayload, 32);
            motorIsIdle[2 * (motorNum - 1)] = false;
        }

        int velId = _canCommon->generateCANMessage(driveCanId, CAN_CMD_SET_VEL);
        uint velPayload = *(unsigned int*)&vel;
        _canCommon->sendCANMessage(velId, (char*)&velPayload, 32);
    }

}

void MotorCommand::sendMotorMessages1(RequestMessage requestMessage, CANCommon *_canCommon) {

    float vel1 = requestMessage.fl_velocity;

    if (vel1 < 0.1) {
        if (!motorIsIdle[0]) {
            int idleId1 = _canCommon->generateCANMessage(FL_VEL_CAN_ID, CAN_CMD_SET_AXIS_STATE);
            uint idlePayload1 = AXIS_STATE_IDLE;
            _canCommon->sendCANMessage(idleId1, (char *) &idlePayload1, 32);
            motorIsIdle[0] = true;
        }
    } else {
        if (motorIsIdle[0]) {
            int controlId1 = _canCommon->generateCANMessage(FL_VEL_CAN_ID, CAN_CMD_SET_AXIS_STATE);
            uint controlPayload1 = AXIS_STATE_CONTROL;
            _canCommon->sendCANMessage(controlId1, (char*)&controlPayload1, 32);
            motorIsIdle[0] = false;
        }

        int velId1 = _canCommon->generateCANMessage(FL_VEL_CAN_ID, CAN_CMD_SET_VEL);
        uint velPayload1 = *(unsigned int*)&vel1;
        _canCommon->sendCANMessage(velId1, (char*)&velPayload1, 32);
    }

}

void MotorCommand::sendMotorMessages2(RequestMessage requestMessage, CANCommon *_canCommon) {
    int velId2 = _canCommon->generateCANMessage(BL_VEL_CAN_ID, CAN_CMD_SET_VEL);
    float vel2 = requestMessage.bl_velocity;
    uint velPayload2 = *(unsigned int*)&vel2;
    _canCommon->sendCANMessage(velId2, (char*)&velPayload2, 32);
}

void MotorCommand::sendMotorMessages3(RequestMessage requestMessage, CANCommon *_canCommon) {
    int velId3 = _canCommon->generateCANMessage(FR_VEL_CAN_ID, CAN_CMD_SET_VEL);
    float vel3 = -1 * requestMessage.fr_velocity;
    uint velPayload3 = *(unsigned int*)&vel3;
    _canCommon->sendCANMessage(velId3, (char*)&velPayload3, 32);
}

void MotorCommand::sendMotorMessages(RequestMessage requestMessage, CANCommon *_canCommon) {

    int message1 = _canCommon->generateCANMessage(FL_VEL_CAN_ID, CAN_CMD_SET_VEL);
//    int message2 = _canCommon->generateCANMessage(BL_VEL_CAN_ID, CAN_CMD_SET_VEL);
//    int message3 = _canCommon->generateCANMessage(FR_VEL_CAN_ID, CAN_CMD_SET_VEL);

    float float1 = requestMessage.fl_velocity;
//    float float2 = requestMessage.bl_velocity;
//    float float3 = requestMessage.fr_velocity;

//    float float1 = 0xBFD9999A;
//    float float2 = 0xBFD9999A;
//    float float3 = 0;

//    float float1 = 0x3F000000;
//    float float2 = 0x3F000000;
//    float float3 = 0;

//    float float1 = 0;
//    float float2 = 0;
//    float float3 = 0;

    printf("1: 0x%x", float1);
//    printf("2: 0x%x\n", float2);
//    printf("3: 0x%x\n", float3);

    uint payload1 = *(unsigned int*)&float1;
//    uint payload2 = *(unsigned int*)&float2;
//    uint payload3 = *(unsigned int*)&float3;

//    _canCommon->sendCANMessage(message3, (char*)&payload3, 32);

    // OLD
//    _canCommon->sendCANMessage(message2, (char*)&payload2, 32);
//    _canCommon->sendCANMessage(message1, (char*)&payload1, 32);

    //NEW
    _canCommon->sendCANMessage(message1, (char*)&payload1, 32);
//    printf("test\n");
//    _canCommon->sendCANMessage(message2, (char*)&payload2, 32);

//    for (int i = 0; i < motorCANMessagesSize; i++) {
//        _canCommon->sendCANMessage(motorCANMessages[i], (char *) &motorPayloads[i], 32);
//    }

}

void MotorCommand::printMotorMessages() {

    for (int i = 0; i < motorCANMessagesSize; i++) {

        int mask = 0b11111;

        if (motorPayloads[i] != 0x80000000 && motorPayloads[i] != 0x0) {
            printf("can id: 0x%x\n", motorCANMessages[i] >> 5);
            printf("data: 0x%x\n", motorCANMessages[i] & mask);
            printf("payload: 0x%x\n", motorPayloads[i]);
        }
    }
}

void MotorCommand::translateMotorMessage(CANCommon *_canCommon) {

    int message1 = _canCommon->generateCANMessage(FL_VEL_CAN_ID, CAN_CMD_SET_VEL);
    int message2 = _canCommon->generateCANMessage(BL_VEL_CAN_ID, CAN_CMD_SET_VEL);
    int message3 = _canCommon->generateCANMessage(FR_VEL_CAN_ID, CAN_CMD_SET_VEL);

    float float1 = motorMessage.fl_velocity;
    float float2 = motorMessage.bl_velocity;
    float float3 = motorMessage.fr_velocity;

    uint payload1 = *(unsigned int*)&float1;
    uint payload2 = *(unsigned int*)&float2;
    uint payload3 = *(unsigned int*)&float3;

    _canCommon->sendCANMessage(message2, (char*)&payload2, 32);
    _canCommon->sendCANMessage(message1, (char*)&payload1, 32);
    _canCommon->sendCANMessage(message3, (char*)&payload3, 32);

//    if (motorMessage.has_fl_angle) {
//        motorCANMessages[motorCANMessagesSize]
//            = canCommon->generateCANMessage(FL_ANGLE_CAN_ID, CAN_CMD_SET_POS);
//        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.fl_angle;
//        motorCANMessagesSize++;
//    }
//
//    if (motorMessage.has_fl_velocity) {
//        motorCANMessages[motorCANMessagesSize]
//                = canCommon->generateCANMessage(FL_VEL_CAN_ID, CAN_CMD_SET_VEL);
//        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.fl_velocity;
//        motorCANMessagesSize++;
//    }
//
//    if (motorMessage.has_fr_angle) {
//        motorCANMessages[motorCANMessagesSize]
//                = canCommon->generateCANMessage(FR_ANGLE_CAN_ID, CAN_CMD_SET_POS);
//        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.fr_angle;
//        motorCANMessagesSize++;
//    }
//
//    if (motorMessage.has_fr_velocity) {
//        motorCANMessages[motorCANMessagesSize]
//                = canCommon->generateCANMessage(FR_VEL_CAN_ID, CAN_CMD_SET_VEL);
//        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.fr_velocity;
//        motorCANMessagesSize++;
//    }
//
//    if (motorMessage.has_bl_angle) {
//        motorCANMessages[motorCANMessagesSize]
//                = canCommon->generateCANMessage(BL_ANGLE_CAN_ID, CAN_CMD_SET_POS);
//        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.bl_angle;
//        motorCANMessagesSize++;
//    }
//
//    if (motorMessage.has_bl_velocity) {
//        motorCANMessages[motorCANMessagesSize]
//                = canCommon->generateCANMessage(BL_VEL_CAN_ID, CAN_CMD_SET_VEL);
//        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.bl_velocity;
//        motorCANMessagesSize++;
//    }
//
//    if (motorMessage.has_br_angle) {
//        motorCANMessages[motorCANMessagesSize]
//                = canCommon->generateCANMessage(BR_ANGLE_CAN_ID, CAN_CMD_SET_POS);
//        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.br_angle;
//        motorCANMessagesSize++;
//    }
//
//    if (motorMessage.has_br_velocity) {
//        motorCANMessages[motorCANMessagesSize]
//                = canCommon->generateCANMessage(BR_VEL_CAN_ID, CAN_CMD_SET_VEL);
//        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.br_velocity;
//        motorCANMessagesSize++;
//    }
}





