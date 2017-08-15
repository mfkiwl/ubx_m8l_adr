/*
 * cfg_msg_packet.cpp - implement UBX-CFG-MSG packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-06
 */

#include "cfg_msg_packet.h"
#include <iostream>

std::string CfgMsgPacket::toString() {
    //std::cout << "this is in CfgMsgPacket.toString function." << std::endl;
    std::ostringstream s;
    s << "UBX-CFG-MSG: config class: " << std::hex << (int)this->msgCls;
    s << " id: " << std::hex << (int)this->msgid;
    s << " rate: " << (int) this->rate << "\n";

    return s.str();
}

void CfgMsgPacket::decodePayload() {
}

int CfgMsgPacket::calcLen() {
    std::cout << "in CfgMsgPacket calcLen." << std::endl;
    this->len = 11;
    this->paylen = 3;
    return this->len;
}

void CfgMsgPacket::encodePayload() {
    std::cout << "in CfgMsgPacket encode Payload." << std::endl;
    char* p = this->payload;
    p[0] = this->msgCls;
    p[1] = this->msgid;
    p[2] = this->rate;
}
