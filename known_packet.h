/*
 * known_packet.h - define known packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-13
 */

#ifndef _KNOWN_PACKET_H_
#define _KNOWN_PACKET_H_

#include "packet.h"

class KnownPacket: public Packet {
    public :

    KnownPacket() {
        this->type = KNOWN;
    }
    std::string toString() {
        return std::string("KNOWN PACKET.");
    }

    void encodePayload() {
    }

    void decodePayload() {
    }

    int calcLen() {
        return 0;
    }
};

#endif //_KNOWN_PACKET_H_
