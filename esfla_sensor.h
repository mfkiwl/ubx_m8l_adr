/*
 * esfla_sensor.h - define the sensor lever.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-07
 */
#ifndef _ESFLA_SENSOR_H_
#define _ESFLA_SENSOR_H_

typedef enum EsfLaSensorType {
    ESFLA_GNSS_ANTENNA = 0,
    ESFLA_ACCELEROMETER = 1,
} EsfLaSensorType;

class EsfLaSensorLever {
    public:
    EsfLaSensorType sensType;
    short leverArmX;
    short leverArmY;
    short leverArmZ;
};

#endif //_ESFLA_SENSOR_H_
