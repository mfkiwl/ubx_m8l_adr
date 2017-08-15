/*
 * alg_packet.cpp - implement ESF-ALG packet.
 *
 * Author: Shan Jiejing.
 * Date: 2017-07-05
 */

#include "alg_packet.h"
#include "util.h"

void AlgPacket::encodePayload() {
}

int AlgPacket::calcLen() {
    return 0;
}

void AlgPacket::decodePayload() {
    this->iTOW = l2ui(this->payload);
    this->version = this->payload[4];
    this->algError = (AlgErrorStatus) ((this->payload[5] & 0x08) >> 3);
    this->algStatus = (AlgStatus)(this->payload[5] & 0x07);
    this->roll = (short)l2us(this->payload + 10);
    this->pitch = (short)l2us(this->payload + 12);
    this->yaw = (short)l2us(this->payload + 14);
}

std::string AlgPacket::toString() {
    if (this->status != COMPLETE) return std::string("ESF-ALG packet is not complete.");

    std::ostringstream s;
    s << "ESF-ALG:\n";
    s << std::string("iTOW:") << (this->iTOW) << "\n";
    s << "version: " << (int)this->version << "\n";
    s << "algError: " << (this->algError == ALG_NO_ERROR ? "NO ERROR\n" : "ERROR\n");
    s << "algStatus: ";
    switch (this->algStatus) {
        case ALG_USER_DEFINED:
            s << "User defined\n";
            break;
        case ALG_CALIBRATING_ROLL_PITCH:
            s << "Calibrating roll/pitch\n";
            break;
        case ALG_CALIBRATING_ROLL_PITCH_YAW:
            s << "Calibrating roll/pitch/yaw\n";
            break;
        case ALG_CALIBRATED:
            s << "Calibrated\n";
            break;
    }
    s << "roll: " << roll << "\n";
    s << "pitch: " << pitch << "\n";
    s << "yaw: " << yaw << "\n";
    return s.str();
}
