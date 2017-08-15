/*
 * event_loop.cpp - implement event loop.
 *
 * Author: Shan Jiejing
 * Date: 2017-08-15
 */

#include "event_loop.h"

int EventLoop::addEventListener(EventListener* listener) {
    if (nullptr != listener) this->listeners.push_back(listener);
    return 0;
}

int EventLoop::rmEventListener(EventListener* listener) {
    if (nullptr != listener) this->listeners.remove(listener);
    return 0;
}

int EventLoop::start() {
    if (0 >= this->listeners.size()) return 0;

    int size = this->listeners.size();
    if (0 < this->epollfd) epoll_close(this->epollfd);
    this->epollfd = epoll_create(size);

}
