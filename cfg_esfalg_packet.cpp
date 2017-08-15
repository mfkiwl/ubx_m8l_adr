/*
 * cfg_esfalg_packet.cpp - implement UBX_CFG_ESFALG packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-06
 */

#include "cfg_esfalg_packet.h"
#include "util.h"

int CfgEsfAlgPacket::calcLen() {
    this->len = 20;
    this->paylen = 12;
    return this->len;
}

void CfgEsfAlgPacket::encodePayload() {
    char* p = this->payload;

    p[0] = this->version;
    if (this->doAutoMntAlg) {
        p[1] |= 0x01;
    } else {
        p[1] &= ~0x01;
    }
    ui2l(p + 4, this->yaw);
    us2l(p + 8, this->pitch);
    us2l(p + 10, this->roll);
}

void CfgEsfAlgPacket::decodePayload() {
}

std::string CfgEsfAlgPacket::toString() {
    return "";
}
