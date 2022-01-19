//
// Created by robonav on 12/19/21.
//

#include "CANCommon.h"

CANCommon::CANCommon(CAN *_can) {
    can = _can;
}

int CANCommon::generateCANMessage(int canID, int cmdID) {
    return (canID << 5) | cmdID;
}

void CANCommon::sendCANMessage(int message) {
    can->write(CANMessage(message));
}

void CANCommon::sendCANMessage(int message, char *payload, uint payload_length) {
    can->write(CANMessage(message, payload, payload_length));
}