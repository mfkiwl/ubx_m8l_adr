/*
 * nav_status_packet.cpp - implement UBX-NAV-STATUS packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-10
 */

#include "nav_status_packet.h"
#include "util.h"
#include <sstream>

void NavStatusPacket::encodePayload() {
}

void NavStatusPacket::decodePayload() {
    char* p = this->payload;

    this->iTOW = l2ui(p);
    this->fixMode = (GpsFixMode)p[4];
    this->gpsFixOk = (p[5] & 0x01);
    this->diffSoln = (p[5] & 0x02);
    this->wknSet = (p[5] & 0x04);
    this->towSet = (p[5] & 0x08);
    this->dgpsIStat = (p[6] & 0x01);
    this->mapMatchingStatus = (MapMatchingStatus)((p[6] & 0xC0) >> 6);
    this->psmState = (PowerSaveMode)(p[7] & 0x03);
    this->ttff = l2ui(p + 8);
    this->msss = l2ui(p + 12);
}

int NavStatusPacket::calcLen() {
    this->len = 16 + 8;
    this->paylen = 16;
    return this->len;
}

std::string NavStatusPacket::toString() {
    std::ostringstream s;

    s << "UBX-NAV-STATUS:\n" << std::endl;
    s << "iTOW: " << this->iTOW << std::endl;
    s << "Fix mode: ";
    switch (this->fixMode) {
        case GPS_FIX_NO_FIX:
            s << "NO FIX" << std::endl;
            break;
        case GPS_FIX_DR_ONLY:
            s << "DR ONLY" << std::endl;
            break;
        case GPS_FIX_2D_FIX:
            s << "2D FIX" << std::endl;
            break;
        case GPS_FIX_3D_FIX:
            s << "3D FIX" << std::endl;
            break;
        case GPS_FIX_DR_COMBIN:
            s << "GPS and DR combined" << std::endl;
            break;
        case GPS_FIX_TIME_ONLY_FIX:
            s << "TIME only fix" << std::endl;
            break;
    }
    s << "GPS fix OK: " << (this->gpsFixOk ? "OK" : "NOT OK") << std::endl;
    s << "DGPS used: " << (this->diffSoln ? "USED" : "NOT USED") << std::endl;
    s << "Weekly Num valid: " << (this->wknSet ? "Valid" : "Not Valid") << std::endl;
    s << "Time of Weekly valid: " << (this->towSet ? "Valid" : "Not Valid") << std::endl;
    s << "DGPS Input status: " << (this->dgpsIStat ? "PR+PRP Correction" : "NONE") << std::endl;
    s << "Map Matching Status: ";
    switch (this->mapMatchingStatus) {
        case MAP_MATCHING_NONE:
            s << "NONE" << std::endl;
            break;
        case MAP_MATCHING_VALID_NO_USED:
            s << "Valid but not used" << std::endl;
            break;
        case MAP_MATCHING_VALID_USED:
            s << "Valid and used" << std::endl;
            break;
        case MAP_MATCHING_VALID_USED_DR:
            s << "Valid and used with DR" << std::endl;
            break;
    }
    s << "Power Save Mode State: ";
    switch (this->psmState) {
        case PSM_ACQUISITION:
            s << "ACQUISITION" << std::endl;
            break;
        case PSM_TRACKING:
            s << "TRACKING" << std::endl;
            break;
        case PSM_OPTIMIZED_TRACKING:
            s << "OPTIMIZED TRACKING" << std::endl;
            break;
        case PSM_INACTIVE:
            s << "INACTIVE" << std::endl;
            break;
    }
    s << "TTFF: " << this->ttff << std::endl;
    s << "msss: " << this->msss << std::endl;

    return s.str();
}
