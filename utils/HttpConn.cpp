//
// Created by illyasviel on 2023/8/18.
//

#include <unordered_map>
#include <sys/sendfile.h>

#include "HttpConn.h"
#include "ServerParams.h"
#include "SocketOpt.h"
#include "MmapManager.h"

namespace {
    constexpr size_t BUFFERSIZE = 256;

    std::unordered_map<Response::HttpStatusCode, const char*> shortmsgMap {
            {Response::k200Ok, "OK"},
            {Response::k400BadRequest, "Bad Request"},
            {Response::k403Forbidden, "Forbidden"},
            {Response::k404NotFound, "Not Found"},
            {Response::k503Unavailable, "Unavailable"},
            {Response::k301MovedPermanently, "Moved Permanently"},
            {Response::kUnknown, "Unknown"},
    };

    std::unordered_map<std::string, std::string> mimeMap {
            {"txt",     "text/plain"},
            {"html",    "text/html"},
            {"css",     "text/css"},
            {"pdf",     "application/pdf"},
            {"ico",     "image/x-icon"},
            {"other",   "application/octet-stream"},
    };

    void makeErrorPage(const ResponsePtr& response,
                       Response::HttpStatusCode errnum,
                       const char *cause,
                       const char *longmsg) {
        auto shortmsg = shortmsgMap[errnum];
        response->setStatusCode(errnum);
        response->setStatusMessage(shortmsg);
        char body[BUFFERSIZE];
        snprintf(body, BUFFERSIZE,
                 "<html><title>Hy0 Error</title><body>%d %s\r\n<p>%s %s\r\n</p></body></html>",
                 errnum, shortmsg, longmsg, cause);
        response->setBody(body);
    }

    void makeMimeType(const ResponsePtr& response, const std::string& url) {
        auto point = url.rfind('.');
        std::string suffix;
        if (point == url.npos) {
            suffix = "other";
        }
        else {
            suffix = url.substr(point+1);
        }
        if (mimeMap.count(suffix) == 0) {
            suffix = "other";
        }
        response->setDict("Content-Type", mimeMap[suffix]);
    }
}

HttpConn::HttpConn(SocketPtr&& client_sk)  :
        state_(kNone),
        client_sk_(std::move(client_sk)),
        readBuffer_(std::make_unique<Buffer>()),
        writeBuffer_(std::make_unique<Buffer>()),
        context(),
        response(std::make_unique<Response>(false)),
        filerPtr_(std::make_unique<Filer>())
{
    if (client_sk_ && client_sk_->is_alive()) {
        set_state(kStart);
    }
}

HttpConn::HttpConn(int client_fd) :
        state_(kNone),
        client_sk_(std::make_unique<Socket>(client_fd)),
        readBuffer_(std::make_unique<Buffer>()),
        writeBuffer_(std::make_unique<Buffer>()),
        context(),
        response(std::make_unique<Response>(false)),
        filerPtr_(std::make_unique<Filer>())
{
    if (client_sk_ && client_sk_->is_alive()) {
        set_state(StateImpl::kStart);
    }
}

HttpConn::~HttpConn() {
    close_conn();
}

void HttpConn::close_conn() {
    if (state_.getState() != StateImpl::kStart) return;
    client_sk_->close();
    set_state(StateImpl::kClose);
}

bool HttpConn::http_go() {
//    std::cout << fmt::format("Start http transaction in socket[{}]\n", fd());

    bool ok = true;
    int error;
    ssize_t n;
    do {
        n = readBuffer_->readFD(fd(), &error);
    } while ( (n < 0 && error == EINTR) /*|| ET*/ );

    if (n < 0) {
//        std::cerr << fmt::format("Read error in socket[{}], error code:{}\n", fd(), error);
        ok = false;
    }
    else if (n == 0) {
//        std::cerr << fmt::format("Read empty in socket[{}]\n", fd());
        ok = false;
    }

    if (!ok) {
        clientError("No data read", 503, "Service Unavailable", "Haha");
    }

    if (!context.parseRequest(readBuffer_)) {
        clientError("Bad Request", 400, "Bad Request", "Haha");
        ok = false;
    }

    if (ok && context.gotAll()) {
        response->reset();
        onRequest();
        response->append2Buffer(writeBuffer_);
        context.reset();
    } else {
        clientError("Bad Request", 400, "Bad Request", "Haha");
    }

    return true;
}

void HttpConn::onRequest() {
    auto keepalive = context.getHeader("Connection");
    std::transform(keepalive.begin(), keepalive.end(), keepalive.begin(), [](unsigned char c) {
        if (isalpha(c)) return std::tolower(c);
        return (int)c;
    });
    bool close = (params.getKeepAliveTimeout() == -1) || (keepalive != "keep-alive");
    response->setCloseConnection(close);

    // method_
    auto method = context.request().getMethod();
    auto full_url = context.request().getFullUrl();
    if (method == "GET") {
        auto pos = full_url.find('?');
        if (pos != std::string::npos) {
            std::unordered_map<std::string, std::string> urlParams;
            context.processParams(full_url.substr(pos+1), urlParams);
        }
        else {
            // get file
            if (full_url == "/") {
                full_url = "/index.html";
            }
            auto root_url = params.getWorkRoot() + full_url;

            auto mmaper = MmapManager::getMmapmanger().getFiler(root_url);
            if (mmaper->getState() == Mmaper::kStart) {
                response->setFileSize(mmaper->getFileSize());
                filerPtr_->exchange(std::move(mmaper));
                response->setStatusCode(Response::k200Ok);
                response->setStatusMessage("OK");
                makeMimeType(response, full_url);
            }
            else if (mmaper->getState() == Mmaper::kUnavailable) {
                makeErrorPage(response, Response::k503Unavailable, "Service Unavailable", "Haha");
            }
            else if (mmaper->getState() == Mmaper::kNotFound) {
                makeErrorPage(response, Response::k404NotFound, "Not Found", "Haha");
            }
            else if (mmaper->getState() == Mmaper::kForbidden) {
                makeErrorPage(response, Response::k403Forbidden, "Forbidden", "Haha");
            }
            else if (mmaper->getState() == Mmaper::kIsDir) {
                makeErrorPage(response, Response::k403Forbidden, "Target is Dir", "Haha");
            }
            else {
                makeErrorPage(response, Response::k503Unavailable, "Service Unavailable", "Haha");
            }
        }
    }
    else if (method == "POST") {

    }
    else {

    }
}

void HttpConn::send(const char *data, size_t len)  {
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if (state_.getState() < StateImpl::kStart) {
//        std::cout << "Connection not start." << std::endl;
        return;
    }

    if (writeBuffer_->readableBytes() == 0) {
        int error;
        nwrote = socketopt::send(fd(), data, len, &error);
        if (nwrote >= 0) {
            remaining = len - nwrote;
        }
        else {  // nwrote < 0
            nwrote = 0;
            if (error != EWOULDBLOCK || error != EAGAIN) {
//                std::cerr << "Failed in send" << std::endl;
                if (error == EPIPE || error == ECONNRESET) {
                    faultError = true;
                }
            } else {
                std::cout << "?" << std::endl;
            }
        }
    }

    assert(remaining <= len);
    if (!faultError && remaining > 0) {
        size_t oldLen = writeBuffer_->readableBytes();
        writeBuffer_->append(data+nwrote, remaining);
    }
}

void HttpConn::clientError(const char *cause, int errnum, const char *shortmsg, const char *longmsg) {
    char body[BUFFERSIZE];
    snprintf(body, BUFFERSIZE,
             "<html><title>Hy0 Error</title><body>%d %s\r\n<p>%s %s\r\n</p></body></html>",
             errnum, shortmsg, longmsg, cause);
    auto bodySize = strlen(body);
    char header[1024];
    snprintf(header, 1024, "HTTP/1.1 %d %s\r\nContent-Type: text/html\r\nContent-Length: %zu\r\n\r\n", errnum, shortmsg, bodySize);
    auto headerSize = strlen(header);
    writeBuffer_->ensureWritable(bodySize + headerSize);
    writeBuffer_->append(header, strlen(header));
    writeBuffer_->append(body, strlen(body));
}

bool HttpConn::send() {
    int error;
    auto nLeft = writeBuffer_->readableBytes();
    if (nLeft == 0) return true;
    ssize_t nWrite;
    do {
        nWrite = socketopt::send(fd(), writeBuffer_->peek(), nLeft, &error);
        if (nWrite < 0) {
            if (error == EINTR) {
                continue;
            }
            break;
        }

        nLeft -= nWrite;
        writeBuffer_->retrieve(nWrite);
    } while (nLeft > 0);

    return nLeft + writeBuffer_->readableBytes() == 0;
}
