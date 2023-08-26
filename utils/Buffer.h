//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_BUFFER_H
#define HYLCHAN_BUFFER_H


#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <cstring>

class Config;

extern const char* CRLF;
extern const size_t CRLFSIZE;

/**
 * --------------------------------------
 * |        |               |           |
 * |        |               |           |
 * --------------------------------------
 *        reader           writer
 */

class Buffer {
public:
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
            : buffer_(initialSize),
              readerIndex_(0),
              writerIndex_(0)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
    }

    void swap(Buffer& rhs) {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_, rhs.readerIndex_);
        std::swap(writerIndex_, rhs.writerIndex_);
    }

    size_t readableBytes() const { return writerIndex_ - readerIndex_; }

    size_t writableBytes() const { return buffer_.size() - writerIndex_; }

    size_t prependableBytes() const { return readerIndex_; };

    const char* peek() const { return begin() + readerIndex_; }

    const char* begin() const { return &(*buffer_.begin()); }

    char* writerBegin() { return begin()+writerIndex_; }

    const char* writerBegin() const {return begin()+writerIndex_;}

    char* begin() { return &*buffer_.begin(); }

    void append(const char* str, size_t len) {
        ensureWritable(len);
        std::copy(str, str+len, writerBegin());
        hasWritten(len);
    }

    void append(const std::string& str) {
        append(str.c_str(), str.length());
    }

    void hasWritten(size_t len) {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }

    void undoWrite(size_t len) {
        assert(len <= readableBytes());
        writerIndex_ -= len;
    }

    void appendDict(const std::string& field, const std::string& value) {
        std::string tmp(field);
        tmp.reserve(value.size() + 2 + CRLFSIZE);
        tmp += ": ";
        tmp += value;
        tmp += CRLF;
        append(tmp);
    }

    void appendHeaderLine(const char* str, size_t len) {
        append(str, len);
        append(CRLF, CRLFSIZE);
    }

    void appendHeaderLine(const std::string& str) {
        append(str);
        append(CRLF, CRLFSIZE);
    }

    void shrink(size_t reserve) {
        buffer_.shrink_to_fit();
    }

    void ensureWritable(size_t len) {
        if (writableBytes() >= len) return;
        else {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    void dataTopping() {
        size_t readable = readableBytes();
        std::copy(begin()+readerIndex_,
                  begin()+writerIndex_,
                  begin());
        readerIndex_ = 0;
        writerIndex_ = readerIndex_ + readable;
        assert(readable == readableBytes());
    }

    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len) {
        assert(len <= readableBytes());
        std::string ret(peek(), len);
        retrieve(len);
        return ret;
    }

    std::string retrieveAsStringUntil(const char* end) {
        assert(peek() <= end);
        assert(end - peek() <= readableBytes());
        size_t len = end - peek();
        std::string ret(peek(), len);
        retrieve(len);
        return ret;
    }

    void retrieveUntil(const char* end) {
        assert(peek() <= end);
        assert(writerBegin() >= end);
        retrieve(end - peek());
    }

    auto findStr(const char* str, size_t len) const {
        void* result = memmem(peek(), readableBytes(), str, len);
        return static_cast<const char*>(result);
    }

    ssize_t readFD(int fd, int *error);

    size_t internalCapacity() const { return buffer_.capacity(); }

    void retrieve(size_t len) {
        assert(len <= readableBytes());
        if (len == 0) return;
        if (len < readableBytes()) {
            readerIndex_ += len;
        } else {
            retrieveAll();
        }
    }

private:

    void retrieveAll() {
        readerIndex_ = 0;
        writerIndex_ = 0;
    }

    void makeSpace(size_t len) {
        if (writableBytes() >= len) {

        }
        else if (writableBytes() + readerIndex_ >= len) {
            dataTopping();
        }
        else {
            auto fac = buffer_.size();
            do {
                fac *= 2;
            } while (fac < len);
            buffer_.resize(fac);
        }
        assert(writableBytes() >= len);
    }

private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};

using BufferPtr = std::unique_ptr<Buffer>;


#endif //HYLCHAN_BUFFER_H
