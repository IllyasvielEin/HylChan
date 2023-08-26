//
// Created by illyasviel on 2023/8/18.
//

#include "EpollNeko.h"

#include <iostream>
#include <bitset>

EpollNeko::EpollNeko() :
    state_(kNone),
    epollSk_(std::make_unique<Socket>(createEpoll())),
    events(1024)
{
    if (epollSk_->is_alive()) {
        state_ = kStart;
    } else {
//        std::cerr << "EpollNeko fd Initial Fail" << std::endl;
    }
}

int EpollNeko::addSocket(int fd, size_t eventNum) {
    eventBak_.emplace(fd, eventNum);
    return ctlEpoll(fd, EPOLL_CTL_ADD, eventNum);
}

int EpollNeko::deleteSocket(int fd) {
    eventBak_.erase(fd);
    return ctlEpoll(fd, EPOLL_CTL_DEL);
}

int EpollNeko::modifySocket(int fd, size_t newEventNum) {
    return ctlEpoll(fd, EPOLL_CTL_MOD, newEventNum);
}

int EpollNeko::ctlEpoll(int socket_fd, int ctlNum, size_t epollEvent) {
    epoll_event event{};
    epoll_event* eventPtr = nullptr;
    if (epollEvent != 0) {
        event.events = epollEvent;
        event.data.fd = socket_fd;
        eventPtr = &event;
    }

    int ret = epoll_ctl(epollSk_->fd(), ctlNum, socket_fd, eventPtr);
    if (ret == -1) {
//        std::cerr << "Add [" << socket_fd << "] to [" << epollSk_->fd() << "]" << std::endl;
//        perror("epoll_ctl add");
    }

    return ret;
}

int EpollNeko::poll(int timeout) {
    if (!epollSk_->is_alive()) {
//        std::cerr << "Epoll socket error" << std::endl;
        return -1;
    }

    int eventCount = epoll_wait(epollSk_->fd(), &(*events.begin()), static_cast<int>(events.size()), timeout);

    int error = errno;
    if (eventCount > 0) {
//        std::cout << eventCount << " events happen" << std::endl;
        // fill channel

        if (eventCount == events.size() && eventCount * 2 <= MAXEVENT) {
            events.resize(events.size() * 2);
        }
    }
    else if (eventCount == 0) {
//        std::cout << "no events" << std::endl;
    }
    else {
        if (error != EINTR) {
            errno = error;
//            std::cerr << "epoll_wait error" << std::endl;
        }
        else {
            eventCount = 0;
        }
    }

    return eventCount;
}

int EpollNeko::fillChannelList(ChannelList& channelList) {
    auto eventCount = poll();
    for (auto i = 0; i < eventCount; ++i) {
        channelList.emplace_back(static_cast<int>(events[i].data.fd), static_cast<uint32_t>(events[i].events));
    }
    return eventCount;
}
