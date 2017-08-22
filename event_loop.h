/*
 * event_loop.h - define event loop.
 *
 * Author: Shan Jiejing
 * Date: 2017-8-14
 */

#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <list>
#include <sys/epoll.h>
#include <unistd.h>

typedef enum EventFlag {
    EVENT_READ = 1,
    EVENT_TIMEOUT = 2,
    EVENT_WRITE = 4,
} EventFlag;

class EventListener {
public:
    virtual int onTimeOut() = 0;
    virtual int onReadEvent() = 0;
    virtual int onWriteEvent() = 0;
    int fd;
    int flags;
    EventListener(int f = 0, int fg = 0): fd(f), flags(fg) {}
    virtual ~EventListener() {}
};
class EventLoop {
public:
    int addEventListener(EventListener* listener);
    int rmEventListener(EventListener* listener);
    void setTimeout(int timeout) {
        this->timeout = timeout;
    }
    int start();
    int stop();
    int clear() {
        if (0 != this->epollfd) close(this->epollfd);
        this->epollfd = 0;
        this->needStop = false;
        this->timeout = -1;
        this->listeners.clear();
        return 0;
    }
    EventLoop() : epollfd(0), needStop(false), timeout(-1) {}
    ~EventLoop() {
        needStop = false;
        if (0 < epollfd) {
            close(epollfd);
            epollfd = 0;
        }
        while(!listeners.empty()) {
            EventListener* p = listeners.front();
            listeners.pop_front();
            delete p;
        }
        listeners.clear();
    }

private:
    int epollfd;
    volatile bool needStop;
    int timeout;
    std::list<EventListener*> listeners;
    static const int MAX_EVENTS;
};

#endif //_EVENT_LOOP_H_
