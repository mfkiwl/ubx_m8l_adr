/*
 * unknown_packet.h - define unknown packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-13
 */

#ifndef _UNKNOWN_PACKET_H_
#define _UNKNOWN_PACKET_H_

#include "packet.h"

class UnknownPacket : public Packet {
    public:

    UnknownPacket() {
        this->type = UNKNOWN;
    }
    std::string toString() {
        return std::string("UNKNOWN PACKET");
    }

    void encodePayload() {
    }

    void decodePayload() {
    }

    int calcLen() {
        return 0;
    }
};

#endif //_UNKNOWN_PACKET_H_
