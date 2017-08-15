/*
 * adr_auto_cal.c - implement M8L adr auto calibration.
 *
 * implement M8L adr auto ocalibration function.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-04
 */

#include <iostream>

#include "gnss_device.h"
//#include "esfla_sensor.h"

static const std::string UBX_SERIAL("/dev/ttyS9") ;
static const short ANTENNA_ARMX = 189;
static const short ANTENNA_ARMY = -30;
static const short ANTENNA_ARMZ = 67;
static const short ACCEL_ARMX = 186;
static const short ACCEL_ARMY = -7;
static const short ACCEL_ARMZ = 60;
static const short YAW = 90;
static const short PITCH = 0;
static const short ROLL = 6;


int main() {
    GnssDevice device(UBX_SERIAL, 9600);

    device.setSensorLever(ESFLA_GNSS_ANTENNA, ANTENNA_ARMX, ANTENNA_ARMY, ANTENNA_ARMZ);
    device.setSensorLever(ESFLA_ACCELEROMETER, ACCEL_ARMX, ACCEL_ARMY, ACCEL_ARMZ);
    device.setSensorMountAngles(YAW, PITCH, ROLL);
    device.start();

    int r = device.calibrate();
    std::cout << "calibration resualt : " << r << std::endl;

    device.startADR();

    device.stop();

    return 0;
}
