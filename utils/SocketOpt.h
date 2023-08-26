//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_SOCKETOPT_H
#define HYLCHAN_SOCKETOPT_H

#include <string>
#include <netdb.h>

namespace socketopt {

    ssize_t send(int fd, const char *msg, size_t size, int *errno_save);

    ssize_t send(int fd, const std::string &msg, int *errno_save);

    ssize_t recv(int fd, std::string &msg, int *errno_save);

    ssize_t readv(int fd, const iovec *iov, int iovcnt);

    int create_listenfd(unsigned short port, bool is_nonblock = false);

    int create_clientfd(sockaddr_in *peer, const char *hostname, const char *port, bool is_nonblock = false);

    int accept(int listen_fd, sockaddr* addr = nullptr, int flags = 0);

    bool setfd(int fd, int optname, bool on);

    bool setsocket(int fd, int optname, bool on);

    void shutdownWrite(int fd);

    void close(int fd);

} // socketopt

#endif //HYLCHAN_SOCKETOPT_H
