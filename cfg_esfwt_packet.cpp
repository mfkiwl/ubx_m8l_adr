/*
 * cfg_esfwt_packet.cpp - implement UBX-CFG-ESFWT packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-10
 */

#include "cfg_esfwt_packet.h"
#include "util.h"

std::string CfgEsfWtPacket::toString() {
    return "";
}

void CfgEsfWtPacket::encodePayload() {
    char* p  = this->payload;

    *p = this->version;
    if (this->combineTicks) p[1] |= 0x01;
    if (this->useWtSpeed) p[1] |= 0x10;
    if (this->dirPinPol) p[1] |= 0x20;
    if (this->useWtPin) p[1] |= 0x40;
    ui2l(p + 4, this->wtFactor);
    ui2l(p + 8, this->wtQuantError);
    ui2l(p + 12, this->wtCountMax);
    us2l(p + 16, this->wtLatency);
    p[18] = this->wtFrequency;
    us2l(p + 20, this->speedDeadBand);

}

void CfgEsfWtPacket::decodePayload() {
}

int CfgEsfWtPacket::calcLen() {
    this->len = 40;
    this->paylen = 32;
    return this->len;
}

