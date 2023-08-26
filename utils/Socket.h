//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_SOCKET_H
#define HYLCHAN_SOCKET_H


#include <sys/socket.h>
#include <memory>
#include <netdb.h>

#include "NonCopyable.h"
#include "State.h"

class Socket : public NonCopyable {
    enum StateImpl {
        kCrash = -2,
        kNone = -1,
        kClose,
        kStart,
        kListen,
        kConnect
    };

    State<StateImpl> state_;
    int fd_;

    static std::string enum2string(const StateImpl &s);

    bool error_not_state(const StateImpl &desired) {
        if (state_.getState() != desired) {
            return true;
        }
        return false;
    }

    bool error_in_range_state(const StateImpl &desired_start, const StateImpl &desired_end) {
        if (state_.getState() >= desired_start && state_.getState() <= desired_end) {
            return true;
        } else {
            return false;
        }
    }

    bool error_not_range_state(const StateImpl &desired_start, const StateImpl &desired_end) {
        return error_in_range_state(desired_start, desired_end);
    }


    void set_state(const StateImpl &s) {
        state_.setState(s);
    }

public:
    Socket() : fd_(-1), state_(kNone) {};

    explicit Socket(int fd)
            : state_(StateImpl::kNone), fd_(fd) {
        if (fd_ != -1) {
            state_.setState(StateImpl::kStart);
        }
    }

    ~Socket() {
        if (fd_ != -1 && state_.getState() >= StateImpl::kStart) {
            close();
        }
    }

    Socket(Socket&& s)  noexcept : fd_(s.fd_), state_(s.state_) {
        s.fd_ = -1;
        s.set_state(StateImpl::kNone);
    }

    bool is_alive() const noexcept { return state_.getState() >= StateImpl::kStart; }

    int fd() const { return fd_; }

    void init(int fd);

    bool listen(int backlog = 256);

    bool connect(const sockaddr_in* addr);

    void close();

    void shutdownWrite();
};

using SocketPtr = std::unique_ptr<Socket>;


#endif //HYLCHAN_SOCKET_H
