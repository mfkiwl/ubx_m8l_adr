/*
 * gnss_device.cpp - implement gnss device.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-06
 */

#include "gnss_device.h"
#include <termios.h>
#include <stdio.h>
#include <iostream>
#include "cfg_rst_packet.h"
#include "cfg_msg_packet.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>

#include "packet.h"
#include "status_packet.h"
#include "alg_packet.h"
#include "cfg_esfla_packet.h"
#include "cfg_esfalg_packet.h"
#include "esf_meas_packet.h"
#include "cfg_esfwt_packet.h"
#include "nav_status_packet.h"
#include "time_helper.h"

const int GnssDevice::TIMEOUT = 100;

int GnssDevice::CalibrationEventListener::onTimeOut() {
    return 0;
}
int GnssDevice::CalibrationEventListener::onReadEvent() {
    int len = 0;
    len = read(this->device->fd,
        this->device->rbuf + this->device->index,
        (BUF_SIZE - this->device->index));
    std::cout << "read :" << len << "errno:" << strerror(errno) << std::endl;
    if (0 >= len) return -1;
    this->device->index += len;
    this->device->showReadBuffer();

    Packet* ret = Packet::findPacket(this->device->rbuf, this->device->index);
    if (ret->type == UNKNOWN) {
        goto next_read;
    }

    if (ret->type == KNOWN) {
        if (ret->status == NOT_COMPLETE) goto next_read_not_move;
        if (ret->status == COMPLETE) {
            goto next_read;
        }
        std::cout << "find error packet.\n" << std::endl;
        memset(this->device->rbuf, 0, this->device->index);
        this->device->index = 0;
        return -1;
    }

    std::cout << ret->toString() << std::endl;
    std::cout << ret->showBuffer() << std::endl;

    //find ALG/STATUS packet
    if (ret->type == UBX_ESF_ALG) {
        AlgPacket* pAlg = (AlgPacket*)ret;
        //print ALG/STATUS packet
        //check calibration stauts
        if (pAlg->algStatus == ALG_CALIBRATED) {
            //if calibrated, stop
            std::cout << "Device: " << this->device->serial << " has calibrated.\n" << std::endl;
            this->device->calib = true;
            this->device->eventLoop.stop();
        }
    }

    if (ret->type == UBX_ESF_STATUS) {
        EsfStatusPacket *pStatus = (EsfStatusPacket*)ret;
        if (pStatus->mntAlgStatus == MNT_INITED) {
            std::cout << "Device: " << this->device->serial << " has calibrated.\n" << std::endl;
            this->device->calib = true;
            this->device->eventLoop.stop();
        }
    }

next_read:
    if (this->device->index > ret->len)
        memcpy(this->device->rbuf, this->device->rbuf + ret->len, this->device->index - ret->len);
    this->device->index -= ret->len;
next_read_not_move:
    ret->clear();
    delete ret;
    return 0;
}
int GnssDevice::CalibrationEventListener::onWriteEvent() {
    return 0;
}

int GnssDevice::ADREventListener::onTimeOut() {
    //if timeout
    std::cout << "time out." << std::endl;
    //get speed and direction data from mcu service.
    int speed = this->device->client->getSpeed();
    bool forward = this->device->client->getForward();
    std::cout << "speed: " << speed << ", forward: " << forward << std::endl;
    //create esf meas packet
    EsfMeasPacket* meas = (EsfMeasPacket*) Packet::createPacket(UBX_ESF_MEAS);
    meas->setMeasData(SPEED, forward, speed);
    this->device->latestTimeTick = TimeHelper::getMonoTick();
    meas->timeTag = (unsigned int)this->device->latestTimeTick;
    meas->encode();
    std::cout << meas->showBuffer() << std::endl;
    std::cout << meas->toString();
    //write esf meas packet to chip
    if (0 >= write(this->fd, meas->start, meas->len)) return -1;
    meas->clear();
    delete meas;
    meas = NULL;
    this->device->eventLoop.setTimeout(GnssDevice::TIMEOUT);
    return 0;
}

int GnssDevice::ADREventListener::onReadEvent() {
    long long currentTime = TimeHelper::getMonoTick();
    unsigned int timeout = 0;
    //read serial
    int len = read(this->device->fd, this->device->rbuf + this->device->index, (BUF_SIZE - this->device->index));
    if (0 >= len) {
        timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - this->device->latestTimeTick);
        if (0 > timeout) timeout = GnssDevice::TIMEOUT;
        this->device->latestTimeTick = TimeHelper::getMonoTick();
        this->device->eventLoop.setTimeout(timeout);
        return -1;
    }
    this->device->index += len;
    this->device->showReadBuffer();

    int dealIndex = 0;
    bool dealAll = false;
    Packet* ret = nullptr;
    while (!dealAll) {
        std::cout << "dealIndex: " << dealIndex << ", index: " <<this->device->index << std::endl;
        ret = Packet::findPacket(this->device->rbuf + dealIndex, this->device->index - dealIndex);
        if (ret->type == UNKNOWN) {
            goto next_deal;
        }

        if (ret->type == KNOWN) {
            if (ret->status == NOT_COMPLETE) {
                break; //next read
            }
            if (ret->status == COMPLETE) {
                goto next_deal;
            }
            std::cout << "find error packet.\n" << std::endl;
            memset(this->device->rbuf, 0, this->device->index);
            this->device->index = 0;
            timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - this->device->latestTimeTick);
            this->device->eventLoop.setTimeout(timeout);
            this->device->latestTimeTick = TimeHelper::getMonoTick();
            ret->clear();
            delete ret;
            return -1;
        }

        std::cout << ret->toString() << std::endl;
        std::cout << ret->showBuffer() << std::endl;
        //find nav status packet
        if (ret->type == UBX_NAV_STATUS) {
            NavStatusPacket* nav = (NavStatusPacket*) ret;
            if (GPS_FIX_DR_ONLY == nav->fixMode ||
                    GPS_FIX_DR_COMBIN == nav->fixMode) {
                std::cout << "ADR has works." << std::endl;
            }
        }
        //check fix mode if ADR works
        //calculate timeout time.
next_deal:
        dealIndex += ret->len;
        ret->clear();
        delete ret;
        if (dealIndex >= this->device->index) {
            this->device->index = 0;
            timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - this->device->latestTimeTick);
            if (0 > timeout) timeout = GnssDevice::TIMEOUT;
            std::cout << "new time out is " << timeout << std::endl;
            this->device->latestTimeTick = TimeHelper::getMonoTick();
            this->device->eventLoop.setTimeout(timeout);
            dealAll = true;
        }
    }

    //dealIndex += ret->len;
    if (dealIndex < this->device->index) {
        memcpy(this->device->rbuf, this->device->rbuf + dealIndex, (this->device->index - dealIndex));
    }
    this->device->index -= dealIndex;
    std::cout << "next read index: " << this->device->index << std::endl;
    timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - this->device->latestTimeTick);
    if (0 > timeout) timeout = GnssDevice::TIMEOUT;
    std::cout << "new time out is " << timeout << std::endl;
    this->device->latestTimeTick = TimeHelper::getMonoTick();
    this->device->eventLoop.setTimeout(timeout);
    if (nullptr != ret) {
        ret->clear();
        delete ret;
    }

    return 0;
}

int GnssDevice::ADREventListener::onWriteEvent() {
    return 0;
}
GnssDevice::GnssDevice(std::string ser, int band) :
    serial(ser), ttyband(band), fd(0), mode(GNSS_STOP),
    yaw(0), pitch(0), roll(0),
    index(0), flagOfLever(0), needStop(false), eventLoop(),
    latestTimeTick(0) {
    memset(rbuf, 0, BUF_SIZE);
    memset(levers, 0, sizeof(levers));
    if (115200 != band && 9600 != band) return;

    this->fd = open(this->serial.c_str(), O_RDWR | O_NOCTTY );
    if (0 >= this->fd) {
        std::cout<< "can't open " << this->serial << ", errno: " << errno << ", " << strerror(errno) << std::endl;
        return ;
    }
    std::cout << "open " << this->serial << ",band " << this->ttyband << " success." << std::endl;
    close(this->fd);
    this->fd = 0;
    this->mode = GNSS_OPEN;
    this->client = McuClientHelper::create();
}

int GnssDevice::setSensorLever(EsfLaSensorType type, short armx,
        short army, short armz) {
    this->flagOfLever |= (0x01 << (int)type);
    this->levers[type].sensType = type;
    this->levers[type].leverArmX = armx;
    this->levers[type].leverArmY = army;
    this->levers[type].leverArmZ = armz;
    return 0;
}

EsfLaSensorLever GnssDevice::getSensorLever(EsfLaSensorType type) {
    return this->levers[type];
}

int GnssDevice::start() {
    if (GNSS_OPEN != this->mode) return -1;

    this->fd = open(this->serial.c_str(), O_RDWR | O_NOCTTY );
    if (0 >= this->fd) {
        std::cout<< "can't open " << this->serial << ", errno: " << errno << ", " << strerror(errno) << std::endl;
        return -1;
    }

    struct termios config;
    if (0 > tcgetattr(this->fd, &config)) {
        close(this->fd);
        std::cout << "tcgetattr failed. errno: " << strerror(errno) << std::endl;
        return -1;
    }
    cfmakeraw (&config);
    config.c_cflag |= CLOCAL;
    config.c_cc[VMIN] = 1;
    config.c_cc[VTIME] = 0;
    int band = 0;
    if (115200 == this->ttyband) {
        band = B115200;
    } else {
        band = B9600;
    }
    if (0 > cfsetispeed(&config, band) ||
            0 > cfsetospeed(&config, band)) {
        close(this->fd);
        std::cout << "set speed failed. errno: " << strerror(errno) << std::endl;
        return -1;
    }

    if (0 > tcsetattr(this->fd, TCSANOW, &config)) {
        close(this->fd);
        std::cout << "tcsetattr failed. errno: " << strerror(errno) << std::endl;
        return -1;
    }
    tcflush(this->fd, TCIFLUSH);
    fcntl(this->fd, F_SETFL, 0);

    //this->mode = GNSS_START;

    //send CFG-RST command.
    CfgRstPacket* rst = (CfgRstPacket*)Packet::createPacket(UBX_CFG_RST);
    rst->navBbr = RST_NAV_BBR_COLD_START;
    rst->resetMode = RST_GNSS_START;
    rst->encode();

    if (0 >= write(this->fd, rst->start, rst->len)) {
        delete rst;
        std::cout << "write rst packet failed. errno: " << strerror(errno) << std::endl;
        return -1;
    }
    delete rst;
    std::cout << "send start command success." << std::endl;
    sleep(5); // sleep 5 seconds to wait for chip restart.
    this->mode = GNSS_START;
    return 0;
}

int GnssDevice::stop() {
    if (GNSS_START != this->mode) return 0;
    close(this->fd);
    this->fd = 0;
    this->mode = GNSS_STOP;
    return 0;
}

int GnssDevice::configOutputMsg(char cls, char id, char rate) {
    if (GNSS_START != this->mode) return -1;
    //write CFG-MSG packet
    //Packet* cfgmsg = (CfgMsgPacket*)Packet::createPacket(CFGMSG);
    CfgMsgPacket* cfgmsg = (CfgMsgPacket*)Packet::createPacket(UBX_CFG_MSG);

    cfgmsg->msgCls = cls;
    cfgmsg->msgid = id;
    cfgmsg->rate = rate;
    cfgmsg->encode();

    std::cout << "config msg: " << cfgmsg->toString() << std::endl;
    if (0 >= write(this->fd, cfgmsg->start, cfgmsg->len)) {
        delete cfgmsg;
        std::cout << "config packet:" << (int)cls << ", " << (int)id <<", " << (int)rate <<",failed. errno: " << strerror(errno) << std::endl;
        return -1;
    }
    cfgmsg->clear();

    delete cfgmsg;
    std::cout << "config packet:" << std::hex << (int)cls << ", " << std::hex << (int)id <<", " << (int)rate <<",success." << std::endl;
    return 0;
}

/*int GnssDevice::writeCommand(char* buf, int len) {
    if (NULL == buf || 0 >= len) return -1;
    if (GNSS_START != this->mode) return -1;

    return write(this->fd, buf, len);
}

int GnssDevice::readCommand(char* buf, int len) {
    if (GNSS_START != this->mode) return -1;
    if (NULL == buf || 0 >= len) return -1;

    return read(this->fd, buf, len);
}*/
int GnssDevice::configLevers() {
    EsfLaPacket* la = (EsfLaPacket*)Packet::createPacket(UBX_CFG_ESFLA);
    la->version = 0;
    if (this->flagOfLever != 0) {
        if (3 == this->flagOfLever) {
            la->numConfigs = 2;
            la->levers[0] = this->levers[0];
            la->levers[1] = this->levers[1];
        } else {
            la->numConfigs = 1;
            la->levers[0] = this->levers[this->flagOfLever - 1];
        }
    }
    la->encode();
    std::cout << la->toString() << std::endl;
    std::cout << la->showBuffer() << std::endl;
    if (0 >= write(this->fd, la->start, la->len)) {
        delete la;
        std::cout << "config lever: failed. errno: " << strerror(errno) << std::endl;
        return -1;
    }
    la->clear();

    delete la;
    la = NULL;
    std::cout << "config lever success." << std::endl;
    return 0;
}

int GnssDevice::startAutoCalib() {
    CfgEsfAlgPacket* alg = (CfgEsfAlgPacket*) Packet::createPacket(UBX_CFG_ESFALG);
    alg->version = 0;
    alg->doAutoMntAlg = true;
    alg->yaw = this->yaw;
    alg->pitch = this->pitch;
    alg->roll = this->roll;
    alg->encode();
    std::cout << "CFG-ESF-ALG :" << alg->toString() << std::endl;
    std::cout << alg->showBuffer() << std::endl;
    if (0 >= write(this->fd, alg->start, alg->len)) {
        delete alg;
        std::cout<< "start auto calib failed." << strerror(errno) <<std::endl;
        return -1;
    }
    alg->clear();
    delete alg;
    alg = NULL;
    std::cout<< "start auto calib success." <<std::endl;
    return 0;
}

int GnssDevice::configADR() {
    CfgEsfWtPacket* wt = (CfgEsfWtPacket*) Packet::createPacket(UBX_CFG_ESFWT);
    wt->version = 0;
    wt->useWtSpeed = true;
    wt->encode();
    std::cout << "CFG-ESF-WT: " << wt->toString() << std::endl;
    if (0 >= write(this->fd, wt->start, wt->len)) {
        delete wt;
        std::cout<< "config adr failed." << strerror(errno) <<std::endl;
        return -2;
    }
    std::cout << wt->showBuffer() << std::endl;
    wt->clear();
    delete wt;
    wt = NULL;
    std::cout<< "config adr success." << strerror(errno) <<std::endl;
    return 0;
}

int GnssDevice::calibrate() {
    if (GNSS_START != this->mode) return -1;
    //write CFG-MSG packet
    if (0 != configOutputMsg(0x10, 0x14, 1)) return -1;

    if (0 != configOutputMsg(0x10, 0x10, 1)) return -1;
    if (0 != configOutputMsg(0xF0, 0x04, 0)) return -1;
    if (0 != configOutputMsg(0xF0, 0x05, 0)) return -1;
    if (0 != configOutputMsg(0xF0, 0x03, 0)) return -1;
    if (0 != configOutputMsg(0xF0, 0x02, 0)) return -1;
    //if (0 != configOutputMsg(0xF0, 0x02, 0)) return -1;

    //write lever data to chip
    if (0 != configLevers()) return -1;

    //write auto calibration command to chip
    if (0 != startAutoCalib()) return -1;

    this->eventLoop.clear();
    this->eventLoop.addEventListener(new CalibrationEventListener(this, this->fd));
    this->eventLoop.start();
#if 0 // don't do calibration.
    int epollfd = 0;
    int nfds = 0;
    struct epoll_event ev;

    epollfd = epoll_create(1);
    if (epollfd < 0) {
        std::cout << "epoll create fail." << std::endl;
        return -1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = this->fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, this->fd, &ev) == -1) {
        std::cout << "epoll add event fail." << std::endl;
        return -1;
    }

    bool stop = false;
    int len = 0;
    while (!stop) {
        //read serial
        nfds = epoll_wait(epollfd, &ev, 1, -1);
        if (-1 == nfds) {
            std::cout << "epoll wait fail." << std::endl;
            return nfds;
        }

        //if (ev.data.fd != this->fd) { continue;}
        len = read(this->fd, this->rbuf + this->index, (BUF_SIZE - this->index));
        std::cout << "read :" << len << "errno:" << strerror(errno) << std::endl;
        if (0 >= len) continue;
        this->index += len;
        showReadBuffer();

        Packet* ret = Packet::findPacket(this->rbuf, this->index);
        if (ret->type == UNKNOWN) {
            goto next_read;
            /*if (ret->len < this->index) {
                memcpy(this->rbuf, this->rbuf + ret->len, (this->index - ret->len));
            }
            this->index -= ret->len;
            continue;*/
        }

        if (ret->type == KNOWN) {
            if (ret->status == NOT_COMPLETE) goto next_read_not_move;
            if (ret->status == COMPLETE) {
                goto next_read;
                /*if (ret->len < this->index) {
                    memcpy(this->rbuf, this->rbuf + ret->len, (this->index - ret->len));
                }
                this->index -= ret->len;
                continue;*/
            }
            std::cout << "find error packet.\n" << std::endl;
            memset(this->rbuf, 0, this->index);
            this->index = 0;
            continue;
        }

        std::cout << ret->toString() << std::endl;
        std::cout << ret->showBuffer() << std::endl;

        //find ALG/STATUS packet
        if (ret->type == UBX_ESF_ALG) {
            AlgPacket* pAlg = (AlgPacket*)ret;
            //print ALG/STATUS packet
            //check calibration stauts
            if (pAlg->algStatus == ALG_CALIBRATED) {
                //if calibrated, stop
                std::cout << "Device: " << this->serial << " has calibrated.\n" << std::endl;
                this->calib = true;
                break;
            }
        }

        if (ret->type == UBX_ESF_STATUS) {
            EsfStatusPacket *pStatus = (EsfStatusPacket*)ret;
            if (pStatus->mntAlgStatus == MNT_INITED) {
                std::cout << "Device: " << this->serial << " has calibrated.\n" << std::endl;
                this->calib = true;
                break;
            }
        }

next_read:
        if (this->index > ret->len)
            memcpy(this->rbuf, this->rbuf + ret->len, this->index - ret->len);
        this->index -= ret->len;
next_read_not_move:
        ret->clear();
        delete ret;
    }
    //if calibrated, return 0;
    if (this->calib) return 0;
#endif
    this->calib = true;
    return 0;
}

int GnssDevice::setSensorMountAngles(short yaw, short pitch, short roll) {
    this->yaw = yaw;
    this->pitch = pitch;
    this->roll = roll;
    return 0;
}

void GnssDevice::showReadBuffer() {
    if (this->index == 0) {
        std::cout << "empty buffer." << std::endl;
        return ;
    }
    std::cout << "read buffer len: " << this->index << std::endl;
    int i = 0;
    for (i = 0; i < this->index/8; i++ ) {
        std::cout << i << ": " << "0x" << std::hex << (int)this->rbuf[i * 8] << " " \
            << "0x" << std::hex << (int)this->rbuf[i * 8 + 1] << " "
            << "0x" << std::hex << (int)this->rbuf[i * 8 + 2] << " "
            << "0x" << std::hex << (int)this->rbuf[i * 8 + 3] << " "
            << "0x" << std::hex << (int)this->rbuf[i * 8 + 4] << " "
            << "0x" << std::hex << (int)this->rbuf[i * 8 + 5] << " "
            << "0x" << std::hex << (int)this->rbuf[i * 8 + 6] << " "
            << "0x" << std::hex << (int)this->rbuf[i * 8 + 7] << "\t"
            << (this->rbuf[i * 8 + 0] == 0x0d ? '^' : (this->rbuf[i * 8 + 0] == 0x0a ? '~' : this->rbuf[i * 8 + 0])) << " "
            << (this->rbuf[i * 8 + 1] == 0x0d ? '^' : (this->rbuf[i * 8 + 1] == 0x0a ? '~' : this->rbuf[i * 8 + 1])) << " "
            << (this->rbuf[i * 8 + 2] == 0x0d ? '^' : (this->rbuf[i * 8 + 2] == 0x0a ? '~' : this->rbuf[i * 8 + 2])) << " "
            << (this->rbuf[i * 8 + 3] == 0x0d ? '^' : (this->rbuf[i * 8 + 3] == 0x0a ? '~' : this->rbuf[i * 8 + 3])) << " "
            << (this->rbuf[i * 8 + 4] == 0x0d ? '^' : (this->rbuf[i * 8 + 4] == 0x0a ? '~' : this->rbuf[i * 8 + 4])) << " "
            << (this->rbuf[i * 8 + 5] == 0x0d ? '^' : (this->rbuf[i * 8 + 5] == 0x0a ? '~' : this->rbuf[i * 8 + 5])) << " "
            << (this->rbuf[i * 8 + 6] == 0x0d ? '^' : (this->rbuf[i * 8 + 6] == 0x0a ? '~' : this->rbuf[i * 8 + 6])) << " "
            << (this->rbuf[i * 8 + 7] == 0x0d ? '^' : (this->rbuf[i * 8 + 7] == 0x0a ? '~' : this->rbuf[i * 8 + 7])) << "|\n";
    }
    if (0 == this->index % 8) return ;
    std::cout << i << ": ";
    for (int j = 0; j < this->index % 8; j ++) {
        std::cout << "0x" << std::hex << (int)this->rbuf[i * 8 + j] << " ";
    }
    std::cout << "\t\t";
    for (int j = 0; j < this->index % 8; j ++) {
        std::cout << (this->rbuf[i * 8 + j] == 0x0d ? '^' : (this->rbuf[i * 8 + j] == 0x0a ? '~' : this->rbuf[i * 8 + j])) << " ";
    }
    std::cout << "|\n";
}

int GnssDevice::startADR() {
    if (this->mode != GNSS_START || !this->calib) return -1;

    //config output nav status packet
    if (0 != configOutputMsg(0x01, 0x03, 1)) return -1;
    //config esfwt to use speed
    if (0 != configADR()) return -1;

    this->latestTimeTick = TimeHelper::getMonoTick();
    this->eventLoop.clear();
    this->eventLoop.setTimeout(GnssDevice::TIMEOUT);
    this->eventLoop.addEventListener(new ADREventListener(this, this->fd));
    this->eventLoop.start();
#if 0
    int epollfd = 0;
    int nfds = 0;
    struct epoll_event ev;

    epollfd = epoll_create(1);
    if (-1 == epollfd) {
        std::cout << "epoll create fail." << std::endl;
        return -3;
    }

    ev.events = EPOLLIN;
    ev.data.fd = this->fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, this->fd, &ev) == -1) {
        std::cout << "epoll add event fail." << std::endl;
        return -3;
    }
    long long latestTimeTick = TimeHelper::getMonoTick();
    int timeout = GnssDevice::TIMEOUT;
    sp<McuClientHelper> client = McuClientHelper::create();
    while(!this->needStop) {
        //epoll_wait event
        nfds = epoll_wait(epollfd, &ev, 1, timeout);
        if (-1 == nfds) {
            std::cout << "epoll wait fail." << std::endl;
            return -3;
        }

        if (0 == nfds) {
        //if timeout
            std::cout << "time out." << std::endl;
            //get speed and direction data from mcu service.
            int speed = client->getSpeed();
            bool forward = client->getForward();
            std::cout << "speed: " << speed << ", forward: " << forward << std::endl;
            //create esf meas packet
            EsfMeasPacket* meas = (EsfMeasPacket*) Packet::createPacket(UBX_ESF_MEAS);
            meas->setMeasData(SPEED, forward, speed);
            latestTimeTick = TimeHelper::getMonoTick();
            meas->timeTag = (unsigned int)latestTimeTick;
            meas->encode();
            std::cout << meas->showBuffer() << std::endl;
            std::cout << meas->toString();
            //write esf meas packet to chip
            if (0 >= write(this->fd, meas->start, meas->len)) return -2;
            meas->clear();
            delete meas;
            meas = NULL;
            timeout = GnssDevice::TIMEOUT;
            continue;
        }
        //if can read
        if (ev.data.fd == this->fd) {
            //read serial
            int len = read(this->fd, this->rbuf + this->index, (BUF_SIZE - this->index));
            if (0 >= len) {
                timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - latestTimeTick);
                if (0 > timeout) timeout = GnssDevice::TIMEOUT;
                continue;
            }
            this->index += len;
            showReadBuffer();

            int dealIndex = 0;
            bool dealAll = false;
            while (!dealAll) {
                std::cout << "dealIndex: " << dealIndex << ", index: " <<this->index << std::endl;
                Packet* ret = Packet::findPacket(this->rbuf + dealIndex, this->index - dealIndex);
                if (ret->type == UNKNOWN) {
                    goto next_deal;
                    /*if (ret->len < this->index) {
                      memcpy(this->rbuf, this->rbuf + ret->len, (this->index - ret->len));
                      }
                      this->index -= ret->len;
                      timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - latestTimeTick);
                      ret->clear();
                      delete ret;
                      continue;*/
                }

                if (ret->type == KNOWN) {
                    if (ret->status == NOT_COMPLETE) {
                        goto adr_next_read;
                        /*timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - latestTimeTick);
                          ret->clear();
                          delete ret;
                          continue;*/
                    }
                    if (ret->status == COMPLETE) {
                        goto next_deal;
                        /*if (ret->len < this->index) {
                          memcpy(this->rbuf, this->rbuf + ret->len, (this->index - ret->len));
                          }
                          this->index -= ret->len;
                          timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - latestTimeTick);
                          ret->clear();
                          delete ret;
                          continue;*/
                    }
                    std::cout << "find error packet.\n" << std::endl;
                    memset(this->rbuf, 0, this->index);
                    this->index = 0;
                    timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - latestTimeTick);
                    ret->clear();
                    delete ret;
                    continue;
                }

                std::cout << ret->toString() << std::endl;
                std::cout << ret->showBuffer() << std::endl;
                //find nav status packet
                if (ret->type == UBX_NAV_STATUS) {
                    NavStatusPacket* nav = (NavStatusPacket*) ret;
                    if (GPS_FIX_DR_ONLY == nav->fixMode ||
                            GPS_FIX_DR_COMBIN == nav->fixMode) {
                        std::cout << "ADR has works." << std::endl;
                    }
                }
                //check fix mode if ADR works
                //calculate timeout time.
next_deal:
                dealIndex += ret->len;
                ret->clear();
                delete ret;
                if (dealIndex >= this->index) {
                    this->index = 0;
                    timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - latestTimeTick);
                    if (0 > timeout) timeout = GnssDevice::TIMEOUT;
                    std::cout << "new time out is " << timeout << std::endl;
                    dealAll = true;
                }
                continue;
adr_next_read:
                //dealIndex += ret->len;
                if (dealIndex < this->index) {
                    memcpy(this->rbuf, this->rbuf + dealIndex, (this->index - dealIndex));
                }
                this->index -= dealIndex;
                std::cout << "next read index: " << this->index << std::endl;
                timeout = GnssDevice::TIMEOUT - (TimeHelper::getMonoTick() - latestTimeTick);
                if (0 > timeout) timeout = GnssDevice::TIMEOUT;
                std::cout << "new time out is " << timeout << std::endl;
                ret->clear();
                delete ret;
                dealAll = true;
            }
        }
    }
#endif
    return 0;
}

int GnssDevice::stopADR() {
    this->needStop = true;
    return 0;
}
