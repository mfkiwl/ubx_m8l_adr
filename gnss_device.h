/*
 * gnss_device.h - define the gnss device.
 *
 * Author: Shan jiejing
 * Date: 2017-07-06
 */

#ifndef _GNSS_DEVICE_H_
#define _GNSS_DEVICE_H_

#include "packet.h"
#include "esfla_sensor.h"
#include "event_loop.h"
#include "mcu_client_helper.h"

typedef enum GnssDeviceMode {
    GNSS_STOP = 0,
    GNSS_OPEN  = 1,
    GNSS_START = 2,
} GnssDeviceMode;

class GnssDevice {
public:
    int start();
    int stop();
    int calibrate();
    int startADR();
    int stopADR();
    int setSensorLever(EsfLaSensorType, short, short, short);
    EsfLaSensorLever getSensorLever(EsfLaSensorType);
    int setSensorMountAngles(short yaw, short pitch, short roll);
    //int writeCommand(char* buf, int len);
    //int readCommand(char* buf, int len);
    GnssDevice(std::string serial, int ttyband);

protected:
    int configOutputMsg(char cls, char id, char rate);
    int configLevers();
    int startAutoCalib();
    int configADR();
    void showReadBuffer();

    class CalibrationEventListener: public EventListener {
        public:
        int onTimeOut();
        int onReadEvent();
        int onWriteEvent();
        CalibrationEventListener(GnssDevice* dev, int fd) : EventListener(fd, EVENT_READ),
            device(dev) {}

        private:
        GnssDevice* device;
    };

    class ADREventListener: public EventListener {
        public:
        int onTimeOut();
        int onReadEvent();
        int onWriteEvent();
        ADREventListener(GnssDevice* dev, int fd) : EventListener(fd, EVENT_TIMEOUT | EVENT_READ),
            device(dev) {}

        private:
        GnssDevice* device;
    };

    friend class CalibrationEventListener;
    friend class ADREventListener;

private:
    int fd;
    std::string serial;
    int ttyband;
    GnssDeviceMode mode;
#define BUF_SIZE 1024
    char rbuf[BUF_SIZE];
    int index;
    int flagOfLever;
    EsfLaSensorLever levers[2];
    short yaw;
    short pitch;
    short roll;
    bool calib;
    volatile bool needStop;
    static const int TIMEOUT;
    EventLoop eventLoop;
    sp<McuClientHelper> client;
    long long latestTimeTick;
    //CalibrationEventListener calibrationEventListener;
    //ADREventListener adrEventListener;
};
#endif //_GNSS_DEVICE_H_
