//
// Created by illyasviel on 2023/8/18.
//

#include "Response.h"
#include "ServerParams.h"

namespace {
    std::unordered_map<int, std::string> codeMap {
            {200, "200 OK"},
            {400, "400 Bad Request"},
            {403, "403 Forbidden"},
            {404, "404 Not Found"},
            {405, "405 Method Not Allowed"},
            {408, "408 Request Timeout"},
            {500, "500 Internal Server Error"},
            {501, "501 Not Implemented"},
            {503, "503 Service Unavailable"},
            {505, "505 HTTP Version Not Supported"},
    };

    std::unordered_map<std::string, std::string> mimeTypeMap {
            {"txt", "text/plain"},
            {"html", "text/html"},
            {"css", "text/css"},
            {"js", "application/javascript"},
            {"json", "application/json"},
            {"jpg", "image/jpeg"},
            {"jpeg", "image/jpeg"},
            {"png", "image/png"},
            {"gif", "image/gif"},
            {"pdf", "application/pdf"},
            {"ico", "image/x-icon"},
    };
}

Response::Response(bool close) : statusCode_(kUnknown), closeConnection_(close), fileSize_(0) {
    dict.emplace("server", params.getServerName());
}

void Response::append2Buffer(const std::unique_ptr<Buffer> &buffer) {
    std::string tmp("HTTP/1.1 ");
    tmp.reserve(tmp.size() + statusMessage_.size() + CRLFSIZE);
    tmp += std::to_string(statusCode_);
    tmp += " ";
    tmp += statusMessage_;
    buffer->appendHeaderLine(tmp);

    if (closeConnection_) {
        buffer->appendDict("connection", "close");
    }
    else {
        if (!body_.empty()) {
            buffer->appendDict("content-length", std::to_string(body_.size()));
        }
        else {
            buffer->appendDict("content-length", std::to_string(fileSize_));
        }
    }

    for (const auto& p : dict) {
        buffer->appendDict(p.first, p.second);
    }

    buffer->append(CRLF, CRLFSIZE);
    if (!body_.empty()) buffer->append(body_);
}