//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_REQUEST_H
#define HYLCHAN_REQUEST_H


#include <utility>
#include <string>
#include <unordered_map>

class Request {
public:

    template<typename T>
    void setBody(T&& body) {
        body = std::forward<T>(body);
    }

    const std::string& getBody() const { return body_; }

    void swap(Request&& rhs) {
        std::swap(body_, rhs.body_);
        std::swap(dict, rhs.dict);
    }

    auto count(const std::string& key) {
        return dict.count(key);
    }

    std::string& operator[](const std::string& key) {
        return dict[key];
    }

    std::string& operator[](std::string&& key) {
        return dict[std::move(key)];
    }

    auto& getMap() { return dict; }

    template<typename T1, typename T2>
    void setDict(T1&& key, T2&& value) {
        dict.emplace(std::forward<T1>(key), std::forward<T2>(value));
    }

    const std::string &getMethod() const {
        return method_;
    }

    void setMethod(const std::string &method) {
        Request::method_ = method;
    }

    const std::string &getFullUrl() const {
        return full_url_;
    }

    void setFullUrl(const std::string &fullUrl) {
        full_url_ = fullUrl;
    }

    const std::string &getUrl() const {
        return url_;
    }

    void setUrl(const std::string &url) {
        Request::url_ = url;
    }

    const std::string &getHttpVersion() const {
        return http_version_;
    }

    void setHttpVersion(const std::string &httpVersion) {
        http_version_ = httpVersion;
    }

private:
    std::string method_;
    std::string full_url_;
    std::string url_;
    std::string http_version_;

    std::string body_;
    std::unordered_map<std::string, std::string> dict;
};


#endif //HYLCHAN_REQUEST_H
