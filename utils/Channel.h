//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_CHANNEL_H
#define HYLCHAN_CHANNEL_H

#include <cstdint>
#include <sys/epoll.h>
#include <vector>

const uint32_t ERR = (EPOLLHUP | EPOLLRDHUP | EPOLLERR);

class Channel {
public:
    using ChannelType = uint32_t;

    Channel(int fd, ChannelType eventType) : peerFd_(fd), channelType_(eventType) {}

    int fd() const { return peerFd_; };

    bool isRead() const {
        return channelType_ & EPOLLIN;
    }

    bool isWrite() const {
        return channelType_ & EPOLLOUT;
    }

    bool isError() const {
        return channelType_ & EPOLLERR;
    }

    bool isHup() const {
        return channelType_ & EPOLLHUP;
    }

    bool isRdHup() const {
        return channelType_ & EPOLLRDHUP;
    }

private:
    int peerFd_;
    ChannelType channelType_;
};

using ChannelList = std::vector<Channel>;

#endif //HYLCHAN_CHANNEL_H
