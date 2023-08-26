//
// Created by illyasviel on 2023/8/18.
//

#include <sys/sendfile.h>
#include "HylChan.h"
#include "ServerParams.h"

void HylChan::init() {
    if (reactor_->getState() == Reactor::kStart && threadPoll_->getState() == ThreadPoll::kStart) {
        state_.setState(kStart);
    }
}

void HylChan::loop() {
    if (state_.getState() != kStart || reactor_->getState() != Reactor::kStart) {
//        std::cerr << "Error State." << std::endl;
        return;
    }

    while (reactor_->getState() == Reactor::kStart) {
        ChannelList channelList;
        auto epollRet = reactor_->run(channelList);
        if (epollRet < 0) {
            std::cerr << "Epoll Error." << std::endl;
            return;
        }

        for (const auto& channel : channelList) {
            if (channel.fd() == listen_sk_->fd()) {

                if (channel.isError()) {
                    // ?
//                    std::cerr << "Error occur in listen socket" << std::endl;
                    return;
                }

                auto conn = accept_client();
                int optval = EPOLLIN | ERR;
                if (params.isEtOn()) {
                    optval |= EPOLLET;
                }
                auto ret = reactor_->addListen(conn->fd(), optval);
                if (ret == -1) {
//                    std::cerr << "Error in add listen" << std::endl;
                }

            }
            else if (channel.isError() || channel.isHup()) {
//                std::cerr << "Error occur in socket[" << channel.fd() << "]" << std::endl;
                auto ret = conn_manager_.erase(channel.fd());
                if (ret == 0) {
//                    std::cout << "Erase return empty" << std::endl;
                }
            }
            else if (channel.isRdHup()) {
                auto ret = conn_manager_.erase(channel.fd());
                shutdown(channel.fd(), SHUT_WR);
            } else {

                ConnPtr conn;
                try {
                    conn = conn_manager_.at(channel.fd());
                } catch (const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                    continue;
                }

                if (channel.isRead()) {
                    threadPoll_->enqueueTask([this, conn](){
                        processRead(conn);
                    });
                }
                else if (channel.isWrite()) {
                    threadPoll_->enqueueTask([this, conn](){
                        processWrite(conn);
                    });
                }

            }
        }
    }
}

void HylChan::processRead(ConnPtr conn) {
    assert(conn->isAlive());

    int optval = ERR | EPOLLONESHOT | EPOLLOUT;
    conn->http_go();
    reactor_->modifyListen(conn->fd(), optval);
}

size_t HylChan::processWrite(ConnPtr conn) {
    assert(conn->isAlive());
    auto n = conn->send();
    if (!n) {
        reactor_->modifyListen(conn->fd(), EPOLLOUT | ERR | EPOLLONESHOT);
    }
    else {
        int n2 = conn->sendBody();
        if (n2 > 0) {
            reactor_->modifyListen(conn->fd(), EPOLLOUT | ERR | EPOLLONESHOT);
        }
        else {
            if (!conn->isCloseConnect()) {
                reactor_->modifyListen(conn->fd(), EPOLLIN | ERR | EPOLLONESHOT);
            }
            else {
//                conn->shutdownWrite();
//                conn_manager_.erase(conn->fd());
            }
        }
    }

    return n;
}
