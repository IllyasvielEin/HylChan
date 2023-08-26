//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_HTTPCONTEXT_H
#define HYLCHAN_HTTPCONTEXT_H


#include <unordered_map>
#include <string>
#include "Request.h"
#include "Buffer.h"

class HttpContext {
private:
    using Dict = std::unordered_map<std::string, std::string>;

    enum HttpRequestParseState {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll
    };

    HttpRequestParseState state_;
    Request request_;

    bool processRequestLine(const std::string& str);

    bool processHeaders(const std::string &str);
public:
    HttpContext() : state_(kExpectRequestLine) {}

    bool parseEnd() const {return state_ == kGotAll; }

    Request& request() { return request_; }

    const Request& request() const { return request_; }

    bool parseRequest(const std::unique_ptr<Buffer>& readBuf);

    void reset() {
        state_ = kExpectRequestLine;
        request_.swap(Request());
    }

    bool gotAll() const { return state_ == kGotAll; }

    std::string& getHeader(const std::string& key) {
        return request_[key];
    }

    std::string& getHeader(std::string&& key) {
        return request_[std::move(key)];
    }

    // username=1&password=2
    void processParams(const std::string& str, Dict& dict) {
        size_t left = 0, right = str.find('&', left);
        while (true) {
            auto paramPair = str.substr(left, right);
            auto equalPos = paramPair.find('=');
            if (equalPos != std::string::npos) {
                dict.emplace(paramPair.substr(0, equalPos), paramPair.substr(equalPos+1));
            }
            if (right == std::string::npos) {
                break;
            }
            left = right + 1;
            right = str.find('&', left);
        }
    }
};


#endif //HYLCHAN_HTTPCONTEXT_H
