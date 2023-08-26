//
// Created by illyasviel on 2023/8/18.
//

#include <csignal>
#include "HttpServer.h"
#include "SocketOpt.h"
#include "ServerParams.h"

HttpServer::HttpServer() : state_(kNone),
                           listen_sk_(std::make_unique<Socket>(socketopt::create_listenfd(params.getPort()))),
                           conn_count(0)
{
//    std::cout << "create class HttpServer." << std::endl;
    init();
}

std::shared_ptr<HttpConn>& HttpServer::accept_client() {
    if (++conn_count <= params.getMaxConnections()) {
        sockaddr clientaddr{};
        memset(&clientaddr, 0, sizeof clientaddr);
        auto client_fd = socketopt::accept(listen_sk_->fd(), &clientaddr, SOCK_CLOEXEC | SOCK_NONBLOCK);
        if (client_fd == -1) {
//            std::cerr << "Error in client connect." << std::endl;
            return conn_manager_[-1];
        }

        char host[NI_MAXHOST], port[NI_MAXSERV];
        memset(&host, 0, sizeof host);
        memset(&port, 0, sizeof port);
        getnameinfo(&clientaddr, sizeof clientaddr, host, NI_MAXHOST, port, NI_MAXSERV, 0);
//        std::cout << "Accept connect from (" << host << ":" << port << ")" << std::endl;
        conn_manager_.emplace(client_fd, std::make_shared<HttpConn>(client_fd));

        return conn_manager_.at(client_fd);
    }
    else {
        --conn_count;
        return conn_manager_.at(-1);
    }
}

void HttpServer::init() {
//    std::cout << "init http server." << std::endl;

    if (listen_sk_->is_alive()) {
        state_.setState(kStart);
        conn_manager_.emplace(-1, std::make_shared<HttpConn>(-1));
    }
}

void HttpServer::loop() {
    while (state_.getState() == kStart) {
        auto client_conn = accept_client();
        client_conn->http_go();
        conn_manager_.erase(client_conn->fd());
    }
}

void HttpServer::start(int backlog) {
//    std::cout << "Server start init" << std::endl;

    if (state_.getState() != kStart) {
//        std::cerr << "Server is not running." << std::endl;
        return;
    }

    if (listen_sk_->listen(backlog)) {
        loop();
    }
}

//int HttpServer::prepare_file(const std::string& url) {
//    if (mmap_manager_.count(url) != 0) {
//        return 200;
//    }
//    FILE* file = fopen(url.c_str(), "r");
//    if (file == NULL) {
//        int error = errno;
//        if (error == ENOENT || error == EISDIR) {
//            return 404;
//        } else if (error == EACCES) {
//            return 403;
//        } else {
//            return 503;
//        }
//    }
//    int fileFd = fileno(file);
//    struct stat fileStat{};
//    memset(&fileStat, 0, sizeof fileStat);
//    if (fstat(fileFd, &fileStat) == -1) {
//        return 500;
//    }
//    off_t fileSize = fileStat.st_size;
//    mmap_manager_.emplace(url, std::make_unique<Mmaper>(fileSize, fileFd));
//    fclose(file);
//    return 200;
//}