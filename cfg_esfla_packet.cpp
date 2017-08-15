/*
 * cfg_esfla_packet.cpp - implement UBX-CFG-ESFLA packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-06
 */

#include "cfg_esfla_packet.h"
#include "util.h"

void EsfLaPacket::decodePayload() {
}

std::string EsfLaPacket::toString() {
    if (this->status != COMPLETE) return "UBX-CFG-ESFLA packet is not completed.\n";

    std::ostringstream s;
    s << "CFG-ESFLA packet:\n";
    s << "version: " << (int)this->version << "\n";
    s << "number of configurations: " << (int)this->numConfigs << "\n";
    s << "-----------------\n";
    for (int i = 0; i < this->numConfigs; i++) {
        s << "sensor type: " << (int)this->levers[i].sensType << "\n";
        s << "lever arm x: " << this->levers[i].leverArmX << "\n";
        s << "lever arm y: " << this->levers[i].leverArmY << "\n";
        s << "lever arm z: " << this->levers[i].leverArmZ << "\n";
        s << "-----------------\n";
    }

    return s.str();
}

int EsfLaPacket::calcLen() {
    if (1 == this->numConfigs) {
        this->len = 20;
        this->paylen = 12;
    } else {
        this->len = 28;
        this->paylen = 20;
    }

    return this->len;
}

void EsfLaPacket::encodePayload() {
    char* p = this->payload;
    *p = this->version;
    p++;
    *p = this->numConfigs;
    p += 3;
    for (int i = 0; this->numConfigs > i; i++) {
        p[0] = this->levers[i].sensType;
        us2l(p + 2, this->levers[i].leverArmX);
        us2l(p + 4, this->levers[i].leverArmY);
        us2l(p + 6, this->levers[i].leverArmZ);
        p += 8;
    }
}
