//
// Created by illyasviel on 2023/8/21.
//

#include <iostream>
#include "Reactor.h"
#include "Channel.h"

int Reactor::run(ChannelList& channelList) {
    return epollNeko_->fillChannelList(channelList);
}

int Reactor::addListen(int fd, size_t optval) {
    return epollNeko_->addSocket(fd, optval);
}

int Reactor::deleteListen(int fd) {
    return epollNeko_->deleteSocket(fd);
}

int Reactor::modifyListen(int fd, size_t optval) {
    return epollNeko_->modifySocket(fd, optval);
}
