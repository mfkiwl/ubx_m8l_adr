/*
 * time_helper.cpp - implement get mono time interface.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-11
 */

#include "time_helper.h"
#include <time.h>

long long TimeHelper::getMonoTick() {
    struct timespec ts; 
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec*1000 + ts.tv_nsec/1000000;
}
