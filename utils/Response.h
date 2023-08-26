//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_RESPONSE_H
#define HYLCHAN_RESPONSE_H

#include <unordered_map>
#include <string>
#include <sys/sendfile.h>
#include "Buffer.h"
#include "Mmaper.h"

using ResponseDict = std::unordered_map<std::string, std::string>;

class Response {
public:
    enum HttpStatusCode {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 300,
        k400BadRequest = 400,
        k403Forbidden = 403,
        k404NotFound = 404,
        k503Unavailable = 503
    };

    explicit Response(bool close);

    void setStatusCode(HttpStatusCode code) {
        statusCode_ = code;
    }

    void setCloseConnection(bool on) {
        closeConnection_ = on;
    }

    void setStatusMessage(const std::string& statusMessage) {
        statusMessage_ = statusMessage;
    }

    void setBody(const std::string& body) {
        body_ = body;
    }

    const std::string& getBody() const { return body_; }

    bool isCloseConnection() const { return closeConnection_; }

    void append2Buffer(const std::unique_ptr<Buffer> &buffer);

    void setDict(const std::string& key, const std::string& value) {
        dict.emplace(key, value);
    }

    void swap(Response&& rhs) {
        closeConnection_    = rhs.closeConnection_;
        statusCode_         = rhs.statusCode_;
        statusMessage_      = std::move(rhs.statusMessage_);
        body_               = std::move(rhs.body_);
        dict                = std::move(rhs.dict);
    }

    void reset() {
        swap(Response(true));
    }

    size_t getFileSize() const {
        return fileSize_;
    }

    void setFileSize(size_t fileSize) {
        fileSize_ = fileSize;
    }

private:
    bool closeConnection_;
    HttpStatusCode statusCode_;
    std::string statusMessage_;
    std::string body_;
    ResponseDict dict;
    size_t fileSize_;
};

using ResponsePtr = std::unique_ptr<Response>;

#endif //HYLCHAN_RESPONSE_H
