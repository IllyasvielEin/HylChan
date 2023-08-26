//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_EPOLLNEKO_H
#define HYLCHAN_EPOLLNEKO_H

#include <sys/epoll.h>
#include <queue>
#include <memory>
#include <unordered_map>

#include "Channel.h"
#include "Socket.h"



class EpollNeko {
public:
    using EventList = std::vector<epoll_event>;
    static const int MAXEVENT = 65535;

    EpollNeko();

    bool isStart() const { return state_ == kStart; }

    int addSocket(int fd, size_t eventNum);

    int deleteSocket(int fd);

    int modifySocket(int fd, size_t newEventNum);

    int fillChannelList(ChannelList& channelList);

    int poll(int timeout = -1);

private:
    static int createEpoll() {
        return epoll_create(1);
    }

    int ctlEpoll(int socket_fd, int ctlNum, size_t epollEvent = 0);
private:
    enum State {
        kNone,
        kClose,
        kStart
    };

    State state_;
    SocketPtr epollSk_;

    EventList events;
    std::unordered_map<int, uint32_t> eventBak_;
};

using EpollNekoPtr = std::unique_ptr<EpollNeko>;

#endif //HYLCHAN_EPOLLNEKO_H
