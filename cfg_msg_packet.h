/*
 * cfg_msg_packet.h - define UBX-CFG-MSG packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-06
 */

#ifndef _CFG_MSG_PACKET_H_
#define _CFG_MSG_PACKET_H_

#include "packet.h"

class CfgMsgPacket : public Packet {
    public:
    char msgCls;
    char msgid;
    char rate;

    CfgMsgPacket(): msgCls(0),
        msgid(0), rate(0) {
        this->msgClass = 0x06;
        this->msgId = 0x01;
        }
    virtual std::string toString();
    virtual void encodePayload();
    virtual void decodePayload();
    virtual int calcLen();
};

#endif //_CFG_MSG_PACKET_H_
