//
// Created by robonav on 12/19/21.
//

#include "MotorCommand.h"

MotorCommand::MotorCommand(ParseProtobufMbed *eth, CANCommon *_canCommon) {

    canCommon = _canCommon;

    motorMessage = eth->getRequestMessage();
    translateMotorMessage(canCommon);

}

void MotorCommand::sendMotorMessages() {
    for (int i = 0; i < motorCANMessagesSize; i++) {
        canCommon->sendCANMessage(motorCANMessages[i], (char*)&motorPayloads[i], 32);
    }
}

void MotorCommand::printMotorMessages() {
    for (int i = 0; i < motorCANMessagesSize; i++) {

        int mask = 0b11111;

        printf("can id: 0x%x\n", motorCANMessages[i] >> 5);
        printf("data: 0x%x\n", motorCANMessages[i] & mask);
        printf("payload: 0x%x\n", motorPayloads[i]);
    }
}

void MotorCommand::translateMotorMessage(CANCommon *canCommon) {
    if (motorMessage.has_fl_angle) {
        motorCANMessages[motorCANMessagesSize]
            = canCommon->generateCANMessage(FL_ANGLE_CAN_ID, CAN_CMD_SET_POS);
        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.fl_angle;
        motorCANMessagesSize++;
    }

    if (motorMessage.has_fl_velocity) {
        motorCANMessages[motorCANMessagesSize]
                = canCommon->generateCANMessage(FL_VEL_CAN_ID, CAN_CMD_SET_VEL);
        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.fl_velocity;
        motorCANMessagesSize++;
    }

    if (motorMessage.has_fr_angle) {
        motorCANMessages[motorCANMessagesSize]
                = canCommon->generateCANMessage(FR_ANGLE_CAN_ID, CAN_CMD_SET_POS);
        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.fr_angle;
        motorCANMessagesSize++;
    }

    if (motorMessage.has_fr_velocity) {
        motorCANMessages[motorCANMessagesSize]
                = canCommon->generateCANMessage(FR_VEL_CAN_ID, CAN_CMD_SET_VEL);
        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.fr_velocity;
        motorCANMessagesSize++;
    }

    if (motorMessage.has_bl_angle) {
        motorCANMessages[motorCANMessagesSize]
                = canCommon->generateCANMessage(BL_ANGLE_CAN_ID, CAN_CMD_SET_POS);
        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.bl_angle;
        motorCANMessagesSize++;
    }

    if (motorMessage.has_bl_velocity) {
        motorCANMessages[motorCANMessagesSize]
                = canCommon->generateCANMessage(BL_VEL_CAN_ID, CAN_CMD_SET_VEL);
        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.bl_velocity;
        motorCANMessagesSize++;
    }

    if (motorMessage.has_br_angle) {
        motorCANMessages[motorCANMessagesSize]
                = canCommon->generateCANMessage(BR_ANGLE_CAN_ID, CAN_CMD_SET_POS);
        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.br_angle;
        motorCANMessagesSize++;
    }

    if (motorMessage.has_br_velocity) {
        motorCANMessages[motorCANMessagesSize]
                = canCommon->generateCANMessage(BR_VEL_CAN_ID, CAN_CMD_SET_VEL);
        motorPayloads[motorCANMessagesSize] = *(unsigned int*)&motorMessage.br_velocity;
        motorCANMessagesSize++;
    }
}





