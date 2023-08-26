//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_HTTPSERVER_H
#define HYLCHAN_HTTPSERVER_H


#include <unordered_map>
#include <atomic>
#include "Socket.h"
#include "HttpConn.h"
#include "Mmaper.h"
#include "State.h"

class HttpServer {
public:
    explicit HttpServer();

    virtual ~HttpServer() {
        state_.setState(kClose);
    }

    void start(int backlog = 256);

protected:
    using ucmap = std::unordered_map< int, ConnPtr >;

    enum StateInstanse {
        kNone,
        kClose,
        kStart
    };

    State<StateInstanse> state_;

    SocketPtr listen_sk_;

    ucmap conn_manager_;

    std::shared_ptr<HttpConn>& accept_client();

    virtual void loop();

    std::atomic<int> conn_count;

private:
    void init();

};

using HttpServerPtr = std::unique_ptr<HttpServer>;

#endif //HYLCHAN_HTTPSERVER_H
