/*
 * nav_status_packet.h - define UBX-NAV-STATUS packet
 *
 * Author: Shan Jiejing
 * Date: 2017-07-10
 */

#ifndef _NAV_STATUS_PACKET_H_
#define _NAV_STATUS_PACKET_H_

#include "packet.h"

typedef enum GpsFixMode {
    GPS_FIX_NO_FIX = 0,
    GPS_FIX_DR_ONLY = 1,
    GPS_FIX_2D_FIX = 2,
    GPS_FIX_3D_FIX = 3,
    GPS_FIX_DR_COMBIN = 4,
    GPS_FIX_TIME_ONLY_FIX = 5,
} GpsFixMode;

typedef enum MapMatchingStatus {
    MAP_MATCHING_NONE = 0,
    MAP_MATCHING_VALID_NO_USED = 1,
    MAP_MATCHING_VALID_USED = 2,
    MAP_MATCHING_VALID_USED_DR = 3,
} MapMatchingStatus;

typedef enum PowerSaveMode {
    PSM_ACQUISITION = 0,
    PSM_TRACKING = 1,
    PSM_OPTIMIZED_TRACKING = 2,
    PSM_INACTIVE = 3,
} PowerSaveMode;

class NavStatusPacket : public Packet {
    public:
        unsigned int iTOW;
        GpsFixMode fixMode;
        bool gpsFixOk;
        bool diffSoln;
        bool wknSet;
        bool towSet;
        bool dgpsIStat;
        MapMatchingStatus mapMatchingStatus;
        PowerSaveMode psmState;
        unsigned int ttff;
        unsigned int msss;

NavStatusPacket(): iTOW(0), fixMode(GPS_FIX_NO_FIX),
    gpsFixOk(false), diffSoln(false), wknSet(false), towSet(false),
    dgpsIStat(false), mapMatchingStatus(MAP_MATCHING_NONE),
    psmState(PSM_ACQUISITION), ttff(0), msss(0) {
    this->msgClass = 0x01;
    this->msgId = 0x03;
    }

    void encodePayload();
    void decodePayload();
    int calcLen();
    std::string toString();
};

#endif //_NAV_STATUS_PACKET_H_
