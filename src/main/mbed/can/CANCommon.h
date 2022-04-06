//
// Created by robonav on 12/19/21.
//

#ifndef IGVC_FIRMWARE_CANCOMMON_H
#define IGVC_FIRMWARE_CANCOMMON_H

#include "mbed.h"
#include "rtos.h"

#define FL_ANGLE_CAN_ID 0X3
#define FL_VEL_CAN_ID   0X5
#define FR_ANGLE_CAN_ID 0X13
#define FR_VEL_CAN_ID   0X15
#define BL_ANGLE_CAN_ID 0X33
#define BL_VEL_CAN_ID   0X35
#define BR_ANGLE_CAN_ID 0X23
#define BR_VEL_CAN_ID   0X25

#define CAN_CMD_SET_VEL         0x00D
#define CAN_CMD_SET_POS         0x00C
#define CAN_CMD_SET_AXIS_STATE  0x007
#define CAN_CMD_CLEAR_ERROR     0x018

#define AXIS_STATE_IDLE         0x001
#define AXIS_STATE_CONTROL      0x008
#define AXIS_STATE_CALIB        0x003

class CANCommon {

private:
    CAN *can;

public:
    CANCommon(CAN *_can);
    int generateCANMessage(int canID, int cmdID);
    void sendCANMessage(int message);
    int sendCANMessage(int message, char *payload, uint payload_length);
};


#endif //IGVC_FIRMWARE_CANCOMMON_H
