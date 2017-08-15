/*
 * cfg_esfalg_packet.h - define UBX-CFG-ESFALG packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-06
 */

#ifndef _CFG_ESFALG_PACKET_H_
#define _CFG_ESFALG_PACKET_H_

#include "packet.h"

class CfgEsfAlgPacket: public Packet {
    public:
    char version;
    bool doAutoMntAlg;
    unsigned int yaw;
    short pitch;
    short roll;

    CfgEsfAlgPacket(): version(0), doAutoMntAlg(false), yaw(0), pitch(0), roll(0) {
        this->msgClass = 0x06;
        this->msgId = 0x56;
    }
    void decodePayload();
    void encodePayload();
    int calcLen();
    std::string toString();
};

#endif //_CFG_ESFALG_PACKET_H_
