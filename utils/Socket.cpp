//
// Created by illyasviel on 2023/8/18.
//

#include "Socket.h"
#include "socketopt.h"

#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cassert>
#include <cerrno>
#include <cstring>

std::string Socket::enum2string(const StateImpl &s) {
    switch (s) {
        case StateImpl::kCrash: return "kCrash"; break;
        case StateImpl::kNone: return "kNone"; break;
        case StateImpl::kClose: return "kClose"; break;
        case StateImpl::kStart: return "kStart"; break;
        case StateImpl::kConnect: return "kConnect"; break;
        case StateImpl::kListen: return "kListen"; break;
        default: return "error state"; break;
    }
}

void Socket::init(int fd) {
    if (fd > 0) {
        state_.setState(StateImpl::kStart);
        fd_ = fd;
    }
}

bool Socket::listen(int backlog) {
    if (error_not_state(StateImpl::kStart)) {
//        std::cerr << fmt::format("Socket[{}] failed in kStart listen where state:{}({})",
//                                 fd_, enum2string(state_), static_cast<int>(state_)) << std::endl;
        return false;
    }

    if (::listen(fd_, backlog) == -1) {
//        perror("Listen error");
        return false;
    }
//    std::cout << "Start listen in fd[" << fd() << "]" << std::endl;

    set_state(StateImpl::kListen);
    return true;
}

bool Socket::connect(const sockaddr_in *addr) {
    if (error_not_state(StateImpl::kStart)) {
//        std::cerr << fmt::format("Socket[{}] failed in kStart connect where state:{}({})",
//                                 fd_, enum2string(state_), static_cast<int>(state_)) << std::endl;
        return false;
    }
    if (::connect(fd_, (const sockaddr*)addr, sizeof(sockaddr_in)) == -1) {
//        perror("connect");
        return false;
    }

    set_state(StateImpl::kConnect);
    return true;
}

void Socket::close() {
    if (state_.getState() == StateImpl::kClose) {
//        std::cout << fmt::format("Socket[{}] is already closed", fd_) << std::endl;
        return;
    }

    if (error_in_range_state(StateImpl::kCrash, StateImpl::kClose)) {
//        std::cerr << fmt::format("Socket[{}] failed in kClose due to state:{}({})",
//                                 fd_, enum2string(state_), static_cast<int>(state_)) << std::endl;
        return;
    }

    socketopt::close(fd_);
    set_state(StateImpl::kClose);
}

void Socket::shutdownWrite() {
    socketopt::shutdownWrite(fd_);
}