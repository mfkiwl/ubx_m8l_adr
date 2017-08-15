/*
 * mcu_client_helper.h - define the interface of get speed.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-11
 */

#ifndef _MCU_CLIENT_HELPER_H_
#define _MCU_CLIENT_HELPER_H_

#include "InfoCtrlClient.h"
#include <utils/Mutex.h>
#include <utils/String8.h>

using namespace android;

class McuClientHelper : public ClientListener {
    public:
    int getSpeed() {
        Mutex::Autolock _l(this->mtx);
        return this->speed;
    }
    bool getForward() {
        Mutex::Autolock _l(this->mtx);
        return this->forward;
    }
    ~McuClientHelper();
    static sp<McuClientHelper> create();

    protected:
    McuClientHelper(sp<InfoCtrlClient> &client);
    void postData(int msgType, String8 &content);
    void postInt(int msgType, int value);

    private:
    int speed;
    bool forward;
    Mutex mtx;
};

#endif //_MCU_CLIENT_HELPER_H_
