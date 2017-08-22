/*
 * circle_buffer.h - define a circle buffer.
 *
 * Author: Shan Jiejing
 * Date: 2017-08-21
 */
#ifndef _CIRCLE_BUFFER_H_
#define _CIRCLE_BUFFER_H_

class CircleBuffer;

class CircleBit {
    public:
        //CircleBit(char*);
        CircleBit(CircleBuffer* buff, char* p);
        CircleBit(const CircleBit& bit);
        CircleBit operator+(int);
        char operator[](int);
        CircleBit operator++();
        char operator*();

    private:
        char* value;
        CircleBuffer* buffer;
};
class CircleBuffer {
    public:
        CircleBuffer(int size);
        //CircleBuffer(CircleBuffer buff);
        ~CircleBuffer();

        friend class CircleBit;

        char* getEmptySpace();
        int getEmptySpaceLen();

        CircleBit getUsedSpace();
        int getUsedSpaceLen();
        int releaseUsedSpace(int size);
        int addUsedSpaceLen(int len);

    private:
        char* start;
        char* end;
        char* index;
        int len;
};
#endif //_CIRCLE_BUFFER_H_
