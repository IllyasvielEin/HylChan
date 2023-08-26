//
// Created by illyasviel on 2023/8/18.
//

#include "SocketOpt.h"

#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/uio.h>

namespace socketopt {
    ssize_t send(int fd, const char *msg, size_t size, int *errno_save) {
        auto n = ::send(fd, msg, size, 0);
        if (n < 0) {
            *errno_save = errno;
        }
        return n;
    }

    ssize_t send(int fd, const std::string &msg, int *errno_save) {
        auto n = ::send(fd, msg.c_str(), msg.size(), 0);
        if (n < 0) {
            *errno_save = errno;
        }
        return n;
    }

    void close(int fd) {
        if (::close(fd) < 0) {
//            perror("socket kClose");
//            std::cerr << "Failed in socket kClose" << std::endl;
        }
    }

    int create_listenfd(unsigned short port, bool is_nonblock) {
        int retval;
        addrinfo hints{}, *listp{}, *p{};

        memset(&hints, 0, sizeof hints);
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE | AI_NUMERICSERV;

        if ((retval = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &listp)) == -1) {
//            std::cout << gai_strerror(retval) << std::endl;
            return -1;
        }

        int listen_fd = -1;
        for (p = listp; p; p = p->ai_next) {
            if ((listen_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
                continue;
            }

            if (!socketopt::setsocket(listen_fd, SO_REUSEPORT, true)
                || (is_nonblock && !socketopt::setfd(listen_fd, O_NONBLOCK, true))) {
                ::close(listen_fd);
                return -1;
            }

            if (bind(listen_fd, p->ai_addr, p->ai_addrlen) == 0) {
                break;
            } else {
                perror("bind");
                ::close(listen_fd);
                return -1;
            }
        }

        if (!p) {
            listen_fd = -1;
            return -1;
        }

        return listen_fd;
    }

    int create_clientfd(sockaddr_in *peer, const char *hostname, const char *port, bool is_nonblock) {
        int retval;
        addrinfo hints{}, *listp{}, *p{};

        memset(&hints, 0, sizeof hints);
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET;

        if ((retval = getaddrinfo(hostname, port, &hints, &listp)) != 0) {
//            std::cout << gai_strerror(retval) << std::endl;
            return -1;
        }

        int client_fd = -1;
        for (p = listp; p; p = p->ai_next) {
            if ((client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                continue;
            }

            if (is_nonblock && !socketopt::setfd(client_fd, O_NONBLOCK, true)) {
                ::close(client_fd);
                return -1;
            }

            memcpy(peer, p->ai_addr, sizeof(sockaddr));
            break;
        }

        if (!p) {
            return -1;
        } else {
            return client_fd;
        }
    }

    bool setfd(int fd, int optname, bool on) {
        int flags = fcntl(fd, F_GETFL);

        if (flags == -1) {
//            perror("fcntl getfl");
            return false;
        } else if (on && !(flags & optname)) {
            flags |= optname;
        } else if (!on && (flags & optname)) {
            flags &= ~optname;
        }

        if (fcntl(fd, F_SETFL, flags) == -1) {
//            perror("fcntl setfl");
            return false;
        }
        return true;
    }

    bool setsocket(int fd, int optname, bool on) {
        int optval = 1;
        if (!on) {
            optval = 0;
        }

        if (setsockopt(fd, SOL_SOCKET, optname, &optval, sizeof optval) != 0) {
//            perror("set socket opt error");
            return false;
        }

        return true;
    }

    ssize_t readv(int fd, const iovec *iov, int iovcnt) {
        return ::readv(fd, iov, iovcnt);
    }

    void shutdownWrite(int fd) {
        if (::shutdown(fd, SHUT_WR) < 0) {
//            perror("shutdownWrite");
//            std::cerr << "Failed in shutdown write." << std::endl;
        }
    }

    int accept(int listen_fd, sockaddr *addr, int flags) {
        socklen_t len = sizeof(sockaddr);
        return ::accept4(listen_fd, addr, &len, flags);
    }
}