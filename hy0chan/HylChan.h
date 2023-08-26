//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_HYLCHAN_H
#define HYLCHAN_HYLCHAN_H


#include "HttpServer.h"
#include "EpollNeko.h"
#include "Reactor.h"
#include "ThreadPoll.h"


class HylChan : public HttpServer {
public:

    HylChan() :
    HttpServer(),
    reactor_(std::make_unique<Reactor>(listen_sk_->fd())),
    threadPoll_(std::make_unique<ThreadPoll>(params.getThreadCount(), 20000))
    {
        init();
    }

    ~HylChan() {
        reactor_->setState(Reactor::kClose);
    }

protected:
    void loop() override;

    void processRead(ConnPtr conn);

    size_t processWrite(ConnPtr conn);
private:
    void init();

    ReactorPtr reactor_;
    ThreadPollPtr threadPoll_;
};


#endif //HYLCHAN_HYLCHAN_H
