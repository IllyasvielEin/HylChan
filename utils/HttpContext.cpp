//
// Created by illyasviel on 2023/8/18.
//

#include <regex>
#include "HttpContext.h"



bool HttpContext::parseRequest(const std::unique_ptr<Buffer> &readBuf) {
    bool ok = false;
    bool hasMore = true;
    while (hasMore) {
        if (state_ == kExpectRequestLine) {
            auto crlf = readBuf->findStr(CRLF, CRLFSIZE);
            if (!crlf) {
                hasMore = false;
            } else {
                ok = processRequestLine(readBuf->retrieveAsString(crlf - readBuf->peek()));
                if (!ok) {
                    hasMore = false;
                } else {
                    state_ = kExpectHeaders;
                }
                readBuf->retrieveUntil(crlf + 2);
            }
        } else if (state_ == kExpectHeaders) {
            auto crlf = readBuf->findStr(CRLF, CRLFSIZE);
            if (!crlf) {
                hasMore = false;
                state_ = kGotAll;
            } else {
                if (!processHeaders(readBuf->retrieveAsStringUntil(crlf))) {
                    if (request_.count("Content-Length") != 0) {
                        state_ = kExpectBody;
                    } else {
                        state_ = kGotAll;
                        hasMore = false;
                    }
                }
                readBuf->retrieveUntil(crlf + 2);
            }
        } else if (state_ == kExpectBody) {
            size_t contentLength;
            try {
                std::stringstream ss(request_.getMap().at("Content-Length"));
                ss >> contentLength;
            } catch (const std::out_of_range& e) {
//                std::cerr << "Exception caught: " << e.what() << std::endl;
//                std::cerr << "Content-Length not found in the map." << std::endl;
                contentLength = 0;
            }
            request_.setBody(readBuf->retrieveAsString(contentLength));
            hasMore = false;
        } else {

        }
    }

    return ok;
}

bool HttpContext::processRequestLine(const std::string& str) {
    bool ok = true;

    std::istringstream ls(str);

    for (auto i = 0; i < 3; ++i) {
        std::string tmp;
        if (ls >> tmp) {
            switch (i) {
                case 0: request_.setMethod(tmp); break;
                case 1: request_.setFullUrl(tmp); break;
                case 2: request_.setHttpVersion(tmp); break;
            }

        }
        else {
            ok = false;
            break;

        }
    }

    return ok;
}

bool HttpContext::processHeaders(const std::string &str) {
    bool ok = false;
    size_t pos;

    if ((pos = str.find(':')) != str.npos) {
        auto key = str.substr(0, pos);
        pos++;
        while (isspace(str[pos]) && pos < str.size()) ++pos;
        request_.setDict(std::move(key), str.substr(pos));
        ok = true;
    }

    return ok;
}