/*
 * packet.h - define the UBX packet.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-05
 */

#ifndef _PACKET_H_
#define _PACKET_H_

#include <string>
#include <sstream>
#include <iostream>

//using namespace std;

typedef enum PacketType {
    UNKNOWN = 0,
    KNOWN = 1,
    UBX_ESF_ALG = 2,
    UBX_ESF_STATUS = 3,
    UBX_CFG_ESFLA = 4,
    UBX_CFG_RST = 5,
    UBX_CFG_MSG = 6,
    UBX_CFG_ESFALG = 7,
    UBX_CFG_ESFWT = 8,
    UBX_NAV_STATUS = 9,
    UBX_ESF_MEAS = 10,
} PacketType;

typedef enum PacketStatus {
    EMPTY_PACKET = 0,
    NOT_COMPLETE = 1,
    COMPLETE = 2,
} PacketStatus;

class Packet {
public:
    PacketStatus status;
    PacketType type;
    char* start;
    int len;
    char* payload;
    int paylen;
    char msgClass;
    char msgId;
    Packet* encode();
    Packet* decode();
    virtual std::string toString() = 0;
    std::string showBuffer();
    int checkPacket();
    int calcPacketCheckSum();
    static Packet* findPacket(char* buf, int len);
    static Packet* createPacket(PacketType type);
    void clear();
    static const char HEADER_F;
    static const char HEADER_S;
    virtual ~Packet(){}
protected:
    Packet():status(EMPTY_PACKET), type(UNKNOWN), start(NULL), len(0), payload(NULL), paylen(0) {}
    static Packet* createPacket(char* start, int len, char cls, char id);
    virtual void encodePayload() = 0;
    virtual void decodePayload() = 0;
    virtual int calcLen() = 0;

private:
    typedef enum CreatedMode {
        CREATED_BY_PACKET = 0,
        CREATED_BY_USER = 1,
    } CreatedMode;
    CreatedMode createMode;
};


#endif //_PACKET_H_
