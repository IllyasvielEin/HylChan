//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_HTTPCONN_H
#define HYLCHAN_HTTPCONN_H


#include "Socket.h"
#include "Buffer.h"
#include "HttpContext.h"
#include "Response.h"
#include "State.h"
#include "Filer.h"

class HttpConn {
private:
    enum StateImpl {
        kNone,
        kClose,
        kStart,
    };

    State<StateImpl> state_;

    SocketPtr client_sk_;

    BufferPtr readBuffer_;
    BufferPtr writeBuffer_;

    HttpContext context;
    ResponsePtr response;
    FilerPtr    filerPtr_;

    void set_state(StateImpl s) noexcept { state_.setState(s); }

    void onRequest();

    void send(const char* data, size_t len);

    void send(const std::string& mes) {
        send(mes.c_str(), mes.length());
    }

    void shutdown() {
        if (state_.getState() == kStart) {
            set_state(kClose);
            client_sk_->shutdownWrite();
        }
    }

    void clientError(const char *cause, int errnum, const char *shortmsg, const char *longmsg);


public:
    explicit HttpConn(int client_fd);

    explicit HttpConn(SocketPtr&& client_sk);

    ~HttpConn();

    HttpConn(HttpConn&& httpconn) = default;

    bool isAlive() const { return state_.getState() >= kStart; }

    void close_conn();

    bool http_go();

    bool send();

    int fd() const { return client_sk_->fd(); }

    bool isCloseConnect() const { return response->isCloseConnection(); }

    int sendBody() {
        return filerPtr_->send(fd());
    }

    void shutdownWrite() {
        if (state_.getState() != StateImpl::kStart) return;
        socketopt::shutdownWrite(fd());
        set_state(StateImpl::kClose);
    }
};

using ConnPtr = std::shared_ptr<HttpConn>;

#endif //HYLCHAN_HTTPCONN_H
