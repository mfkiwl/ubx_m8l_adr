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
    EVENT_TIMEOUT = 1,
    EVENT_READ = 2,
    EVENT_WRITE = 4,
} EventFlag;

class EventListener {
public:
    virtual int onTimeOut() = 0;
    virtual int onReadEvent() = 0;
    virtual int onWriteEvent() = 0;
    int fd;
    EventFlag flags;
    EventListener(): fd(0), flags(0) {}
};
class EventLoop {
public:
    int addEventListener(EventListener* listener);
    int rmEventListener(EventListener* listener);
    int start();
    int stop();
    EventLoop() : epollfd(0), stop(false) {}
    ~EventLoop() {
        stop = false;
        if (0 < epollfd) {
            epoll_close(epollfd);
            epollfd = 0;
        }
        listeners.clear();
    }

private:
    int epollfd;
    bool stop;
    std::list<EventListener*> listeners;
};

#endif //_EVENT_LOOP_H_
