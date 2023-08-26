//
// Created by illyasviel on 2023/8/21.
//

#ifndef HYLCHAN_REACTOR_H
#define HYLCHAN_REACTOR_H


#include "EpollNeko.h"
#include "State.h"
#include "ServerParams.h"

class Reactor {
public:

    enum ReactorState {
        kNone,
        kStart,
        kClose
    };

    explicit Reactor(int fd) :
            state_(kNone),
        fd_(fd),
        dispatch_(std::make_unique<Dispatch>()),
        epollNeko_(std::make_unique<EpollNeko>())
    {
        if (fd_ > 0) {
            int ret = epollNeko_->addSocket(fd_, EPOLLIN | EPOLLERR);
            if (ret == 0) {
                state_.setState(kStart);
            }
        }
    }

    ~Reactor() { state_.setState(kClose); }

    auto getState() const { return state_.getState(); }

    void setState(ReactorState state) {
        state_.setState(state);
    }

    int addListen(int fd, size_t optval = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR);

    int deleteListen(int fd);

    int modifyListen(int fd, size_t optval = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR);

    int run(ChannelList& channelList);
private:
    State<ReactorState> state_;
    int fd_;
    DispatchPtr  dispatch_;
    EpollNekoPtr epollNeko_;
};

using ReactorPtr = std::unique_ptr<Reactor>;

#endif //HYLCHAN_REACTOR_H
