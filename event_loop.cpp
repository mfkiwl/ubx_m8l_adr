/*
 * event_loop.cpp - implement event loop.
 *
 * Author: Shan Jiejing
 * Date: 2017-08-15
 */

#include "event_loop.h"

const int EventLoop::MAX_EVENTS = 10;

int EventLoop::addEventListener(EventListener* listener) {
    if (nullptr == listener) return -1;
    if (MAX_EVENTS > this->listeners.size()) {
        this->listeners.push_back(listener);
        return 0;
    }
    return -1;
}

int EventLoop::rmEventListener(EventListener* listener) {
    if (0 == this->listeners.size()) return 0;
    if (nullptr != listener) this->listeners.remove(listener);
    return 0;
}

int EventLoop::start() {
    if (0 >= this->listeners.size()) return 0;

    struct epoll_event ev, events[MAX_EVENTS];
    int nfds = 0;
    int size = this->listeners.size();
    if (0 < this->epollfd) close(this->epollfd);
    this->epollfd = epoll_create(size);

    if (-1 == this->epollfd) {
        return -1;
    }

    for (std::list<EventListener*>::iterator it = this->listeners.begin();
            it != this->listeners.end();
            it ++) {
        if (0 != ((*it)->flags & EVENT_READ)) ev.events |= EPOLLIN;
        if (0 != ((*it)->flags & EVENT_WRITE)) ev.events |= EPOLLOUT;
        if (0 == ev.events) continue;
        ev.data.fd = (*it)->fd;
        if (epoll_ctl(this->epollfd, EPOLL_CTL_ADD, (*it)->fd, &ev) == -1) {
            this->listeners.erase(it);
            continue;
        }
    }

    while(!this->needStop) {
        nfds = epoll_wait(this->epollfd, events, MAX_EVENTS, this->timeout);
        if (-1 == nfds) {
            return -1;
        }
    
        if (0 == nfds) {
            for (std::list<EventListener*>::iterator it = this->listeners.begin();
                    it != this->listeners.end();
                    it ++) {
                if (0 != ((*it)->flags & EVENT_TIMEOUT)) {
                    (*it)->onTimeOut();
                }
            }
        }
    
        for (int i = 0; i < nfds; i++) {
            for (std::list<EventListener*>::iterator it = this->listeners.begin();
                    it != this->listeners.end();
                    it ++) {
                if ((*it)->fd != events[i].data.fd) continue;
                if (0 != (events[i].events & EPOLLIN)) (*it)->onReadEvent();
                if (0 != (events[i].events & EPOLLOUT)) (*it)->onWriteEvent();
            }
        }
    }

    return 0;
}

int EventLoop::stop() {
    this->needStop = true;
    return 0;
}
