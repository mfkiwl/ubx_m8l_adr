/*
 * cfg_rst_packet.cpp - implement UBX-CFG-RST packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-06
 */

#include "cfg_rst_packet.h"
#include "util.h"

int CfgRstPacket::calcLen() {
    this->len = 12;
    this->paylen = 4;
    return this->len;
}

void CfgRstPacket::encodePayload() {
    char* p = this->payload;
    us2l(p, this->navBbr);
    p[2] = this->resetMode;
}

void CfgRstPacket::decodePayload() {
}

std::string CfgRstPacket::toString() {
    return "";
}
