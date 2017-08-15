/*
 * cfg_esfla_packet.h - define UBX-CFG-ESFLA packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-06
 */

#ifndef _CFG_ESFLA_PACKET_H_
#define _CFG_ESFLA_PACKET_H_

#include "packet.h"
#include "esfla_sensor.h"

class EsfLaPacket : public Packet {
    public:
        char version;
        char numConfigs;
        EsfLaSensorLever levers[2];

    EsfLaPacket(): version(0), numConfigs(0) {
        memset(levers, 0, sizeof(levers));
        this->msgClass = 0x06;
        this->msgId = 0x2F;
    }
    void decodePayload();
    int calcLen();
    std::string toString();
    void encodePayload();
};
#endif //_CFG_ESFLA_PACKET_H_
