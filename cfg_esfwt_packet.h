/*
 * cfg_esfwt.h - define UBX-CFG-ESFWT packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-10
 */

#ifndef _CFG_ESFWT_PACKET_H_
#define _CFG_ESFWT_PACKET_H_

#include "packet.h"

class CfgEsfWtPacket : public Packet {
    public:
    char version;
    bool combineTicks;
    bool useWtSpeed;
    bool dirPinPol;
    bool useWtPin;
    unsigned int wtFactor;
    unsigned int wtQuantError;
    unsigned int wtCountMax;
    unsigned short wtLatency;
    unsigned char wtFrequency;
    unsigned short speedDeadBand;

    std::string toString();
    void encodePayload();
    void decodePayload();
    int calcLen();
    CfgEsfWtPacket():version(0),
        combineTicks(false), useWtSpeed(false), dirPinPol(false),
        useWtPin(false), wtFactor(0), wtQuantError(0), wtCountMax(0),
        wtLatency(0), wtFrequency(0), speedDeadBand(0) {
        this->msgClass = 0x06;
        this->msgId = 0x82;
        }

};
#endif //_CFG_ESFWT_PACKET_H_
