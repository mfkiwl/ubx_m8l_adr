/*
 * status_packet.cpp - implement UBX-ESF-STATUS packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-05
 */

#include "status_packet.h"
#include "util.h"

int EsfStatusPacket::calcLen() {
    return 0;
}

void EsfStatusPacket::decodePayload() {
    this->iTOW = l2ui(this->payload);
    this->version = this->payload[4];
    this->wtInitStatus = (WtInitStatus)(this->payload[5] & 0x3);
    this->mntAlgStatus = (MntAlgStatus)((this->payload[5] & 0x1C) >> 2);
    this->fusionStatus = (SensorFusionStatus) (this->payload[12]);
    this->numSens = this->payload[15];

    char* p = this->payload + 16;
    for (int i = 0; i < this->numSens; i++) {
        this->sensors[i].type = (*p & 0x3F);
        this->sensors[i].used = ((*p & 0x40) != 0);
        this->sensors[i].ready = ((*p & 0x80) != 0);
        this->sensors[i].calibStatus = (SensorCalibStatus)(p[1] & 0x03);
        this->sensors[i].timeStatus = (SensorTimeStatus)((p[1] & 0x0C) >> 2);
        this->sensors[i].freq = p[2];
        this->sensors[i].badMeas = ((p[3] & 0x01) != 0);
        this->sensors[i].badTTag = ((p[3] & 0x02) != 0);
        this->sensors[i].missingMeas = ((p[3] & 0x04) != 0);
        this->sensors[i].noisyMeas = ((p[3] & 0x08) != 0);
        p += 4;
    }

}

std::string EsfStatusPacket::toString() {
    if (this->status != COMPLETE) return "UBX-ESF-STATUS is not completed.";

    std::ostringstream s;

    s << "ESF-STATUS: \n";
    s << "iTOW: " << this->iTOW << "\n";
    s << "version: " << this->version << "\n";
    s << "wtInitStatus: ";
    switch (this->wtInitStatus) {
        case WT_OFF:
            s << "OFF\n";
            break;
        case WT_INITING:
            s << "is Initializing\n";
            break;
        case WT_INITED:
            s << "Initialized\n";
            break;
    }
    s << "mntInitStatus: ";
    switch (this->mntAlgStatus) {
        case MNT_OFF:
            s << "OFF\n";
            break;
        case MNT_INITING:
            s << "is Initializing\n";
            break;
        case MNT_INITED:
            s << "Initialized\n";
            break;
    }
    s << "fusionStatus: ";
    switch (this->fusionStatus) {
        case SF_NO_FUSION:
            s << "no fusion\n";
            break;
        case SF_ALL_USED:
            s << "GNSS and sensor data are used\n";
            break;
        case SF_NO_TEMP:
            s << "Disabled temporarily, invalid sensor data not used\n";
            break;
        case SF_DISABLED:
            s << "Disabled permanently until receiver reset, GNSS-only due to sensor failure\n";
            break;
    }
    s << "number of sensors: " << this->numSens << "\n";
    s << "----------------------\n";
    for (int i = 0; i < this->numSens; i++) {
        s << "\tsensor id: " << i << "\n";
        s << "\tsensor type: " << (int)this->sensors[i].type << "\n";
        s << "\tsensor used: " << (this->sensors[i].used ? "TRUE\n" : "FALSE\n");
        s << "\tsensor ready: " << (this->sensors[i].ready ? "TRUE\n" : "FALSE\n");
        s << "\tsensor calibration status: ";
        switch (this->sensors[i].calibStatus) {
            case SC_NO_CALIB:
                s << "No calibration\n";
                break;
            case SC_CALIBING:
                s << "Calibrating, sensor not yet calibrated\n";
                break;
            case SC_COARSELY_CALIB:
                s << "Calibrating, sensor coarsely calibrated\n";
                break;
            case SC_FINELY_CALIB:
                s << "Calibrating, sensor finely calibrated\n";
                break;
        }
        s << "\tsensor time status: ";
        switch (this->sensors[i].timeStatus) {
            case ST_NO_DATA:
                s << "No data\n";
                break;
            case ST_FIRST_BYTE:
                s << "Reception of the first byte used to tag the measurement\n";
                break;
            case ST_EVENT:
                s << "Event input used to tag the measurement\n";
                break;
            case ST_DATA:
                s << "Time tag provided with the data\n";
                break;
        }
        s << "\tsensor frequency: " << (int)this->sensors[i].freq << "\n";
        s << "\tsensor badMeas: " << (this->sensors[i].badMeas ? "TRUE\n" : "FALSE\n");
        s << "\tsensor badTTag: " << (this->sensors[i].badTTag ? "TRUE\n" : "FALSE\n");
        s << "\tsensor missingMeas: " << (this->sensors[i].missingMeas ? "TRUE\n" : "FALSE\n");
        s << "\tsensor noisyMeas: " << (this->sensors[i].noisyMeas ? "TRUE\n" : "FALSE\n");
        s << "----------------------\n";
    }

    return s.str();
}

void EsfStatusPacket::encodePayload() {
}
