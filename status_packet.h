/*
 * status_packet.h - define UBX-ESF-STATUS packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-05
 */

#ifndef _STATUS_PACKET_H_
#define _STATUS_PACKET_H_

#include "packet.h"

typedef enum WtInitStatus {
    WT_OFF = 0,
    WT_INITING = 1,
    WT_INITED = 2,
} WtInitStatus;

typedef enum MntAlgStatus {
    MNT_OFF = 0,
    MNT_INITING = 1,
    MNT_INITED = 2,
} MntAlgStatus;

typedef enum SensorCalibStatus {
    SC_NO_CALIB = 0,
    SC_CALIBING = 1,
    SC_COARSELY_CALIB = 2,
    SC_FINELY_CALIB = 3,
} SensorCalibStatus;

typedef enum SensorTimeStatus {
    ST_NO_DATA = 0,
    ST_FIRST_BYTE = 1,
    ST_EVENT = 2,
    ST_DATA = 3,
} SensorTimeStatus;

typedef enum SensorFusionStatus {
    SF_NO_FUSION = 0,
    SF_ALL_USED = 1,
    SF_NO_TEMP = 2,
    SF_DISABLED = 3,
} SensorFusionStatus;

class EsfSensor {
public:
    char type;
    bool used;
    bool ready;
    SensorCalibStatus calibStatus;
    SensorTimeStatus timeStatus;
    bool badMeas;
    bool badTTag;
    bool missingMeas;
    bool noisyMeas;
    unsigned char freq;
};

class EsfStatusPacket: public Packet {
public:
    unsigned int iTOW;
    unsigned char version;
    WtInitStatus wtInitStatus;
    MntAlgStatus mntAlgStatus;
    SensorFusionStatus fusionStatus;
    unsigned char numSens;
    EsfSensor sensors[18];

    EsfStatusPacket():iTOW(0), version(0), wtInitStatus(WT_OFF), mntAlgStatus(MNT_OFF),
        fusionStatus(SF_NO_FUSION), numSens(0) {
        memset(sensors, 0, sizeof(sensors));
        this->msgClass = 0x10;
        this->msgId = 0x10;
    }
    void decodePayload();
    void encodePayload();
    std::string toString();
    int calcLen();
};
#endif //_STATUS_PACKET_H_
