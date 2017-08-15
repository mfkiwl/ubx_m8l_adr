/*
 * alg_packet.h - define UBX-ESF-ALG packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-05
 */

#ifndef _ALG_PACKET_H_
#define _ALG_PACKET_H_

#include "packet.h"

typedef enum AlgErrorStatus {
    ALG_NO_ERROR = 0,
    ALG_ERROR = 1,
} AlgErrorStatus;

typedef enum AlgStatus {
    ALG_USER_DEFINED = 0,
    ALG_CALIBRATING_ROLL_PITCH = 1,
    ALG_CALIBRATING_ROLL_PITCH_YAW = 2,
    ALG_CALIBRATED = 3,
} AlgStatus;

class AlgPacket : public Packet {
public :
    unsigned int iTOW;
    unsigned char version;
    AlgErrorStatus algError;
    AlgStatus algStatus;
    short roll;
    short pitch;
    short yaw;

    AlgPacket(): iTOW(0), version(0), algError(ALG_NO_ERROR), algStatus(ALG_USER_DEFINED),
        roll(0), pitch(0), yaw(0) {
        this->msgClass = 0x10;
        this->msgId = 0x14;
    }
    void decodePayload();
    void encodePayload();
    int calcLen();
    std::string toString();
};
#endif //_ALG_PACKET_H_
