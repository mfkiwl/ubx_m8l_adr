/*
 * mcu_client_helper.cpp - implement the interface of get speed.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-11
 */

#include "mcu_client_helper.h"
#include <iostream>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <pthread.h>

McuClientHelper::McuClientHelper(sp<InfoCtrlClient> &client) : ClientListener(client),
    speed(0), forward(false) {
}

static void* helpThread(void*) {
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    return NULL;
}
sp<McuClientHelper> McuClientHelper::create() {
    sp<InfoCtrlClient> mcu_client;
    InfoCtrlClient::connect(1, mcu_client);
    if (nullptr != mcu_client.get()) {
        sp<McuClientHelper> helper = new McuClientHelper(mcu_client);
        mcu_client->setListener(helper.get());
        pthread_t threadid;
        pthread_create(&threadid, NULL, helpThread, NULL);
        //std::thread t{helpThread};
        return helper;
    }
    return nullptr;
}

McuClientHelper::~McuClientHelper() {
}

void McuClientHelper::postData(int type, String8 &content) {
    (void) type;
    Mutex::Autolock _l(this->mtx);
    const char* data = content.string();
    //std::cout << "in post data: " << content << std::endl;
    int len = content.size();
    std::cout << "content len: " << len << std::endl;
#if 0
    int i = 0;
    for (i = 0; i < len/8; i++) {
        std::cout << i << ": " << "0x" << std::hex << (int)data[i * 8] << " " \
            << "0x" << std::hex << (int)data[i * 8 + 1] << " "
            << "0x" << std::hex << (int)data[i * 8 + 2] << " "
            << "0x" << std::hex << (int)data[i * 8 + 3] << " "
            << "0x" << std::hex << (int)data[i * 8 + 4] << " "
            << "0x" << std::hex << (int)data[i * 8 + 5] << " "
            << "0x" << std::hex << (int)data[i * 8 + 6] << " "
            << "0x" << std::hex << (int)data[i * 8 + 7] << "\t"
            << data[i * 8 + 0] << " "
            << data[i * 8 + 1] << " "
            << data[i * 8 + 2] << " "
            << data[i * 8 + 3] << " "
            << data[i * 8 + 4] << " "
            << data[i * 8 + 5] << " "
            << data[i * 8 + 6] << " "
            << data[i * 8 + 7] << "|\n";
    }
    std::cout << i << ": ";
    for (int j = 0; j < len % 8; j ++) {
        std::cout << "0x" << std::hex << (int)data[i * 8 + j] << " ";
    }
    std::cout << "\t\t";
    for (int j = 0; j < len % 8; j ++) {
        std::cout << data[i * 8 + j] << " ";
    }
    std::cout << "|\n";
#endif
    if (0x44 != *data) return;
    this->speed = ((int)(data[2] << 8) | (int)(data[1]));
    this->forward = (data[3] == 0);
}

void McuClientHelper::postInt(int type, int value) {
    (void)type;
    (void) value;
}
