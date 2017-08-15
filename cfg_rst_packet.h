/*
 * cfg_rst_packet.h - define UBX-CFG-RST packet
 *
 * Author: Shan Jiejing
 * Date: 2017-07-06
 */

#ifndef _CFG_RST_PACKET_H_
#define _CFG_RST_PACKET_H_

typedef enum RstNavBbr {
    RST_NAV_BBR_HOT_START = 0,
    RST_NAV_BBR_WARM_START = 1,
    RST_NAV_BBR_COLD_START = 0xFFFF,
} RstNavBbr;

typedef enum RstResetMode {
    RST_HW_RESET = 0,
    RST_SW_RESET = 1,
    RST_SW_RESET_GNSS = 2,
    RST_HW_RESET_AFTER_SHUTDOWN = 4,
    RST_GNSS_STOP = 8,
    RST_GNSS_START = 9,
} RstResetMode;

#include "packet.h"

class CfgRstPacket: public Packet {
    public:
    RstNavBbr navBbr;
    RstResetMode resetMode;

    CfgRstPacket():navBbr(RST_NAV_BBR_COLD_START), resetMode(RST_GNSS_START) {
        this->msgClass = 0x06;
        this->msgId = 0x04;
    }
    void encodePayload();
    void decodePayload();
    std::string toString();
    int calcLen();
};
#endif //_CFG_RST_PACKET_H_
