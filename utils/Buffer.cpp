//
// Created by illyasviel on 2023/8/18.
//

#include "Buffer.h"
#include "SocketOpt.h"

const char* CRLF = "\r\n";
const size_t CRLFSIZE = strlen(CRLF);

ssize_t Buffer::readFD(int fd, int *error) {
    char tmpBuf[65536];
    iovec iov[2];

    const size_t writable = writableBytes();
    iov[0].iov_base = writerBegin();
    iov[0].iov_len  = writable;
    iov[1].iov_base = tmpBuf;
    iov[1].iov_len  = sizeof tmpBuf;

    const int iovcnt = ( writable < sizeof tmpBuf ) ? 2 : 1;
    const ssize_t n = socketopt::readv(fd, iov, iovcnt);
    if (n < 0) {
        *error = errno;
//        perror("buffer readv");
    }
    else if (n <= writable) {
        hasWritten(n);
    }
    else {
        writerIndex_ = buffer_.size();
        append(tmpBuf, n - writable);
    }
    return n;
}
