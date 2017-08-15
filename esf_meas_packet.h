/*
 * esf_meas_packet.h - define UBX-ESF-MEAS packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-10
 */

#ifndef _ESF_MEAS_PACKET_H_
#define _ESF_MEAS_PACKET_H_

#include "packet.h"
#include <vector>

typedef enum EsfTimeMarkType {
    NONE_TIME_MARK = 0,
    EXT0_TIME_MARK = 1,
    EXT1_TIME_MARK = 2,
} EsfTimeMarkType;

typedef enum EsfMeasDataType {
    GYRO_AXIS_Z = 5,
    FRONT_LEFT_WHEEL = 6,
    FRONT_RIGHT_WHEEL = 7,
    REAR_LWFT_WHEEL = 8,
    REAR_RIGHT_WHEEL = 9,
    SINGLE_TICK = 10,
    SPEED = 11,
    GYRO_TEMPERATURE = 12,
    GYRO_AXIS_Y = 13,
    GYRO_AXIS_X = 14,
    ACCL_AXIS_X = 16,
    ACCL_AXIS_Y = 17,
    ACCL_AXIS_Z = 18,
} EsfMeasDataType;

typedef struct EsfMeasData {
    EsfMeasDataType dataType;
    bool forward;
    unsigned int value;
} EsfMeasData;

class EsfMeasPacket : public Packet {
public:
    unsigned int timeTag;
    EsfTimeMarkType timeMarkSent;
    bool timeMarkEdge;
    bool calibTtagValid;
    unsigned short numOfData;
    std::vector<EsfMeasData> datas;

    EsfMeasPacket():
        timeTag(0), timeMarkSent(NONE_TIME_MARK),
        timeMarkEdge(false), calibTtagValid(false),
        numOfData(0) {
            this->msgClass = 0x10;
            this->msgId = 0x02;
        }
    int setMeasData(EsfMeasDataType, bool, unsigned int);
    void encodePayload();
    void decodePayload();
    int calcLen();
    std::string toString();
};
#endif //_ESF_MEAS_PACKET_H_
