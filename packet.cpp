/*
 * packet.cpp - implement UBX packet.
 *
 * Author: Shan Jiejing
 */

#include "packet.h"
#include "alg_packet.h"
#include "status_packet.h"
#include "cfg_rst_packet.h"
#include "cfg_esfla_packet.h"
#include "cfg_msg_packet.h"
#include "cfg_esfalg_packet.h"
#include "cfg_esfwt_packet.h"
#include "esf_meas_packet.h"
#include "nav_status_packet.h"
#include "known_packet.h"
#include "unknown_packet.h"
#include "util.h"
#include <iostream>

const char Packet::HEADER_F = 0xB5;
const char Packet::HEADER_S = 0x62;

int Packet::checkPacket() {
    char ckA = 0;
    char ckB = 0;
    char* p = NULL;

    if (COMPLETE != this->status) return -1;

    for(p = this->start + 2; p != this->start + this->len - 2; p ++) {
        ckA += *p;
        ckB += ckA;
    }

    if (ckA == *(this->start + this->len - 2) &&
            ckB == *(this->start + this->len - 1)) return 0;
    return -1;
}

Packet* Packet::findPacket(char* buf, int len) {

    Packet* ret = NULL;
    char* p = NULL;
    int plen = 0;

    if (NULL == buf || 0 >= len) return ret;
    if (8 > len) { // less than a complete packet.
        ret = new KnownPacket();
        ret->status = NOT_COMPLETE;
        return ret;
    }

    for (p = buf; p != buf + len; p ++) {
        if (*p == HEADER_F && *(p + 1) == HEADER_S) {
            if (p != buf) {
                ret = new UnknownPacket();
                ret->len = p - buf;
                return ret;
            }

            plen = ((unsigned short)p[4] | ((unsigned short)p[5] << 8));
            std::cout << "packet len: " << plen << std::endl;
            if (plen > 500) {
                std::cout << "packet len larger than 500." << std::endl;
                continue;
            }

            if ((plen + 8) > len) {
                ret = new KnownPacket();
                ret->status = NOT_COMPLETE;
                return ret;
            }

            /*if (p[2] == 0x10 && p[3] == 0x14) {
                ret = new AlgPacket();
                ret->start = buf;
                ret->len = plen + 8;
                ret->payload = buf + 6;
                ret->paylen = plen;
                ret->type = UBX_ESF_ALG;
                ret->status = COMPLETE;
                ret->createMode = CREATE_BY_FIND;
                //if checksum is error, discard this ret->as a known packet.
                if (0 != ret->checkPacket()) ret->type = KNOWN;
                return ret;
            }
            if (p[2] == 0x10 && p[3] == 0x10) {
                ret = new EsfStatusPacket();
                ret->start = buf;
                ret->len = plen + 8;
                ret->payload = buf + 6;
                ret->paylen = plen;
                ret->type = UBX_ESF_STATUS;
                ret->status = COMPLETE;
                ret->createMode = CREATE_BY_FIND;
                //if checksum is error, discard this ret->as a known packet.
                if (0 != ret->checkPacket()) ret->type = KNOWN;
                return ret;
            }*/
            ret = createPacket(buf, plen + 8, p[2], p[3]);
            if (NULL == ret) {
                ret = new KnownPacket();
                ret->status = COMPLETE;
                ret->len = plen + 8;
                return ret;
            }
            ret->decode();
            return ret;
        }
    }
    ret = new UnknownPacket();
    ret->len = len;
    return ret;
}

Packet* Packet::createPacket(char* start, int len, char cls, char id) {
    if (NULL == start || 0 >= len ) return NULL;

    Packet* ret = NULL;
    if (cls == 0x10 && id == 0x14) {
        ret = new AlgPacket();
        ret->type = UBX_ESF_ALG;
    } else if (cls == 0x10 && id == 0x10) {
        ret = new EsfStatusPacket();
        ret->type = UBX_ESF_STATUS;
    } else if (cls == 0x01 && id == 0x03) {
        ret = new NavStatusPacket();
        ret->type = UBX_NAV_STATUS;
    } else {
        return NULL;
    }

    ret->start = start;
    ret->len = len;
    ret->payload = start + 6;
    ret->paylen = len - 8;
    ret->status = COMPLETE;
    ret->createMode = CREATED_BY_PACKET;

    return ret;
}

int Packet::calcPacketCheckSum() {
    if (COMPLETE != this->status) return -1;

    char ckA = 0;
    char ckB = 0;
    char* p = NULL;

    for (p = this->start + 2; p != this->start + this->len - 2; p ++) {
        ckA += *p;
        ckB += ckA;
    }

    p[0] = ckA;
    p[1] = ckB;
    return 0;
}

std::string Packet::showBuffer() {
    std::ostringstream s;
    if (nullptr != this->start && 0 != this->len) {
        s << "packet: \n";
        int i = 0;
        for (i = 0; i < this->len/8; i++ ) {
            s << i << ": " << "0x" << std::hex << (int)this->start[i * 8] << " " \
                << "0x" << std::hex << (int)this->start[i * 8 + 1] << " "
                << "0x" << std::hex << (int)this->start[i * 8 + 2] << " "
                << "0x" << std::hex << (int)this->start[i * 8 + 3] << " "
                << "0x" << std::hex << (int)this->start[i * 8 + 4] << " "
                << "0x" << std::hex << (int)this->start[i * 8 + 5] << " "
                << "0x" << std::hex << (int)this->start[i * 8 + 6] << " "
                << "0x" << std::hex << (int)this->start[i * 8 + 7] << "\n";
        }
        s << i << ": ";
        for (int j = 0; j < this->len % 8; j ++) {
            s << "0x" << std::hex << (int)this->start[i * 8 + j] << " ";
        }
        s << "\n";
    }
    return s.str();
}

Packet* Packet::createPacket(PacketType type) {
    if (UBX_ESF_ALG > type) return NULL;

    std::cout << "create packet: " << (int)type << std::endl;
    Packet* ret = NULL;
    switch(type) {
        case UBX_ESF_ALG: {
            ret = new AlgPacket();
            ret->createMode = CREATED_BY_USER;
            ret->type = UBX_ESF_ALG;
            break;
        }
        case UBX_ESF_STATUS: {
            ret = new EsfStatusPacket();
            ret->createMode = CREATED_BY_USER;
            ret->type = UBX_ESF_STATUS;
            break;
        }
        case UBX_CFG_ESFLA: {
            ret = new EsfLaPacket();
            ret->createMode = CREATED_BY_USER;
            ret->type = UBX_CFG_ESFLA;
            break;
        }
        case UBX_CFG_RST: {
            ret = new CfgRstPacket();
            ret->createMode = CREATED_BY_USER;
            ret->type = UBX_CFG_RST;
            //std::cout << "UBX_CFG_RST: " << ret->toString() << std::endl;
            break;
        }
        case UBX_CFG_MSG: {
            ret = new CfgMsgPacket();
            ret->createMode = CREATED_BY_USER;
            ret->type = UBX_CFG_MSG;
            //std::cout << "UBX_CFG_MSG: " << ((CfgMsgPacket*)ret)->toString() << std::endl;
            break;
        }
        case UBX_CFG_ESFALG: {
            ret = new CfgEsfAlgPacket();
            ret->createMode = CREATED_BY_USER;
            ret->type = UBX_CFG_ESFALG;
            break;
        }
        case UBX_CFG_ESFWT: {
            ret = new CfgEsfWtPacket();
            ret->createMode = CREATED_BY_USER;
            ret->type = UBX_CFG_ESFWT;
            break;
        }
        case UBX_ESF_MEAS: {
            ret = new EsfMeasPacket();
            ret->createMode = CREATED_BY_USER;
            ret->type = UBX_ESF_MEAS;
            break;
        }
        default: {
            break;
        }
    }
    if (nullptr == ret) {
        std::cout << "ret is null." << std::endl;
    } else {
        std::cout << "ret is no null." << std::endl;
    }

    return ret;
}

/*std::string Packet::toString() {
    std::ostringstream s;
    s << "Class: " << (int)this->msgClass << ", Id: " << (int)this->msgId << "\n";
    return s.str();
}*/

void Packet::clear() {
    if (this->createMode == CREATED_BY_USER) {
        if (NULL != this->start) delete this->start;
    } else {
        this->createMode = CREATED_BY_USER;
    }
    this->start = NULL;
    this->payload = NULL;
    this->len = 0;
    this->paylen = 0;
    this->status = EMPTY_PACKET;
    //don't clear packet type.
}

Packet* Packet::decode() {
    if (this->status != COMPLETE) return this;

    if (0 != this->checkPacket()) {
        this->type = KNOWN;
        return this;
    }

    this->decodePayload();
    return this;
}

Packet* Packet::encode() {
    std::cout << "in encode "<< (int)this->status << std::endl;
    if (EMPTY_PACKET != this->status) {
        std::cout << "packet status :" << (int)this->status <<std::endl;
        return this;
    }

    if (0 >= this->calcLen()) {
        std::cout << "calcLen return  0" << std::endl;
        return this;
    }

    this->start = (char*)malloc(this->len);
    if (NULL == this->start) {
        std::cout << "malloc " << this->len << ", return NULL" << std::endl;
        return this;
    }
    memset(this->start, 0, this->len);
    this->payload = this->start + 6;
    this->status = COMPLETE;

    char* p = this->start;
    p[0] = Packet::HEADER_F;
    p[1] = Packet::HEADER_S;
    p[2] = this->msgClass;
    p[3] = this->msgId;
    us2l(p + 4, this->paylen);

    this->encodePayload();

    calcPacketCheckSum();

    return this;
}

/*void Packet::encodePayload() {
    std::cout << "in packet encodePayload." << std::endl;
}

void Packet::decodePayload() {
    std::cout << "in packet decodePayload." << std::endl;
}

int Packet::calcLen() {
    std::cout << "in packet calcLen." << std::endl;
    return 0;
}

std::string Packet::toString() {
    std::cout << " this is packet's toString function.\n" << std::endl;
    return std::string("aaaaa");
}*/
