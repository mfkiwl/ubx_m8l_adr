/*
 * circle_buffer.cpp - implement circle buffer.
 *
 * Author: Shan Jiejing
 * Date: 2017-08-21
 */

#include "circle_buffer.h"
#include <string>

CircleBuffer::CircleBuffer(int size) {
    if (0 >= size) return;
    this->start = (char*) calloc(size, sizeof(char));
    if (nullptr == this->start) return;
    this->end = this->start + size;
    this->index = this->start;
    this->len = 0;
}

CircleBuffer::~CircleBuffer() {
    if (nullptr != this->start) delete this->start;
    this->start = nullptr;
    this->end = nullptr;
    this->index = nullptr;
    this->len = 0;
}

char* CircleBuffer::getEmptySpace() {
    if (nullptr == this->start) return nullptr;

    if (this->index + len >= this->end) {
        return (this->index + len - this->end + this->start);
    }
    return (this->index + len);
}

int CircleBuffer::getEmptySpaceLen() {
    if (nullptr == this->start) return 0;

    if (this->index + len >= this->end) {
        return (this->end - this->start - len);
    }
    return (this->end - this->index - len);
}

CircleBit::CircleBit(CircleBuffer* buff, char* p) {
    if (nullptr == p) return ;
    this->value = p;
    this->buffer = buff;
}

CircleBit::CircleBit(const CircleBit& bit) {
    this->value = bit.value;
    this->buffer = bit.buffer;
}

CircleBit CircleBit::operator+(int l) {
    //TODO: throw exceptions
    if (nullptr == this->value ||
            nullptr == this->buffer ||
            nullptr == this->buffer->start) return *this;
    if (this->buffer->start > this->value ||
            this->value >= this->buffer->end) return *this;
    //if (l > this->buffer->len) return *this;

    if (this->value + l >= this->buffer->end) {
        return CircleBit(this->buffer,
                this->value + l -
                this->buffer->end +
                this->buffer->start);
    }
    return CircleBit(this->buffer, this->value + l);
}

CircleBit CircleBit::operator++() {
    //TODO: throw exceptions
    if (nullptr == this->value ||
            nullptr == this->buffer ||
            nullptr == this->buffer->start) return *this;
    if (this->buffer->start > this->value ||
            this->value >= this->buffer->end) return *this;
    //if (l > this->buffer->len) return *this;

    if (this->value + 1 >= this->buffer->end) {
        this->value = this->value + 1 -
            this->buffer->end +
            this->buffer->start;
    } else {
        this->value ++;
    }
    return *this;
}

char CircleBit::operator[](int l) {
    //TODO: throw exceptions
    if (nullptr == this->value ||
            nullptr == this->buffer ||
            nullptr == this->buffer->start) return 0;
    if (this->buffer->start > this->value ||
            this->value >= this->buffer->end) return 0;
    //if (l > this->buffer->len) return *this;

    if (this->value + l >= this->buffer->end) {
        return *(this->value + l -
                this->buffer->end +
                this->buffer->start);
    }
    return *(this->value + l);
}

char CircleBit::operator*() {
    //TODO: throw exceptions
    if (nullptr == this->value ||
            nullptr == this->buffer ||
            nullptr == this->buffer->start) return 0;
    if (this->buffer->start > this->value ||
            this->value >= this->buffer->end) return 0;
    //if (l > this->buffer->len) return *this;

    return *(this->value);
}

CircleBit CircleBuffer::getUsedSpace() {
    if (nullptr == this->start) return CircleBit(this, nullptr);

    return CircleBit(this, this->index);
}

int CircleBuffer::getUsedSpaceLen() {
    if (nullptr == this->start) return -1;
    return this->len;
}

int CircleBuffer::addUsedSpaceLen(int len) {
    if (nullptr == this->start) return -1;
    if (0 >= len || len > (this->end - this->start - this->len)) return -1;

    this->len += len;
    return 0;
}

int CircleBuffer::releaseUsedSpace(int size) {
    if (nullptr == this->start) return -1;
    if (0 >= size || size > this->len) return -1;

    if (this->index + size >= this->end) {
        this->index = this->index + size -
            this->end +
            this->start;
    } else {
        this->index += size;
    }
    this->len -= size;
    return 0;
}
