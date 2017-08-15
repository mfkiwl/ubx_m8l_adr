/*
 * esf_meas_packet.cpp - implement UBX-ESF-MEAS packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-10
 */

#include "esf_meas_packet.h"
#include "util.h"

int EsfMeasPacket::calcLen() {
    this->len = 8 + 8 + (4 * this->numOfData);
    this->paylen = this->len - 8;
    return this->len;
}

std::string EsfMeasPacket::toString() {
    return "";
}

void EsfMeasPacket::encodePayload() {
    char* p = this->payload;
    ui2l(p, this->timeTag);
    switch (this->timeMarkSent) {
        case NONE_TIME_MARK:
            p[4] = 0;
            break;
        case EXT0_TIME_MARK:
            p[4] = 0x01;
            break;
        case EXT1_TIME_MARK:
            p[4] = 0x02;
            break;
    }
    if (this->timeMarkEdge) p[4] |= 0x04;
    if (this->calibTtagValid) p[4] |= 0x08;

    us2l(p + 6, this->numOfData);

    p += 8;
    unsigned int tmp = 0;
    for (auto i : this->datas) {
        tmp = i.value;
        tmp |= ((char)i.dataType << 24);
        if (!i.forward) tmp |= 0x800000;
        ui2l(p, tmp);
        p += 4;
    }
}

void EsfMeasPacket::decodePayload() {
}

int EsfMeasPacket::setMeasData(EsfMeasDataType type, bool forward, unsigned int value) {
    bool hasValue = false;
    std::vector<EsfMeasData>::iterator i;

    for (i = this->datas.begin(); i != this->datas.end(); i++) {
        if (type == i->dataType) {
            hasValue = true;
            break;
        }
    }

    if (hasValue) {
        i->dataType = type;
        i->forward = forward;
        i->value = value;
    } else {
        EsfMeasData data;
        data.dataType = type;
        data.forward = forward;
        data.value = value;
        this->datas.push_back(data);
        this->numOfData ++;
    }

    return 0;
}
