//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_SERVERPARAMS_H
#define HYLCHAN_SERVERPARAMS_H


#include <cstddef>
#include <string>
#include <utility>
#include <cstring>

class ServerParams {
public:
    ServerParams(unsigned short port,
                 size_t max_connections,
                 int peer_response_timeout,
                 int receive_timeout,
                 int keep_alive_timeout,
                 size_t request_size_limit,
                 std::string server_name,
                 bool et_on,
                 size_t threadCount,
                 std::string work_root
                 ) :
                 port_(port),
                 max_connections_(max_connections),
                 peer_response_timeout_(peer_response_timeout),
                 receive_timeout_(receive_timeout),
                 keep_alive_timeout_(keep_alive_timeout),
                 request_size_limit_(request_size_limit),
                 server_name_(std::move(server_name)),
                 ET_On(et_on),
                 threadCount_(threadCount),
                 work_root_(std::move(work_root))
    {

    }

    unsigned short getPort() const {
        return port_;
    }

    void setPort(unsigned short port) {
        port_ = port;
    }

    size_t getMaxConnections() const {
        return max_connections_;
    }

    void setMaxConnections(size_t maxConnections) {
        max_connections_ = maxConnections;
    }

    int getPeerResponseTimeout() const {
        return peer_response_timeout_;
    }

    void setPeerResponseTimeout(int peerResponseTimeout) {
        peer_response_timeout_ = peerResponseTimeout;
    }

    int getReceiveTimeout() const {
        return receive_timeout_;
    }

    void setReceiveTimeout(int receiveTimeout) {
        receive_timeout_ = receiveTimeout;
    }

    int getKeepAliveTimeout() const {
        return keep_alive_timeout_;
    }

    void setKeepAliveTimeout(int keepAliveTimeout) {
        keep_alive_timeout_ = keepAliveTimeout;
    }

    size_t getRequestSizeLimit() const {
        return request_size_limit_;
    }

    void setRequestSizeLimit(size_t requestSizeLimit) {
        request_size_limit_ = requestSizeLimit;
    }

    const std::string &getServerName() const {
        return server_name_;
    }

    void setServerName(const std::string &serverName) {
        server_name_ = serverName;
    }

    bool isEtOn() const {
        return ET_On;
    }

    void setEtOn(bool etOn) {
        ET_On = etOn;
    }

    size_t getThreadCount() const {
        return threadCount_;
    }

    void setThreadCount(size_t threadCount) {
        threadCount_ = threadCount;
    }

    const std::string &getWorkRoot() const {
        return work_root_;
    }

    void setWorkRoot(const std::string &workRoot) {
        work_root_ = workRoot;
    }

private:
    unsigned short port_;
    size_t max_connections_;
    int peer_response_timeout_;	/* timeout of each read or write operation */
    int receive_timeout_;	    /* timeout of receiving the whole message */
    int keep_alive_timeout_;
    size_t request_size_limit_;
    std::string server_name_;
    bool ET_On;
    size_t threadCount_;
    std::string work_root_;
};

extern ServerParams params;

#endif //HYLCHAN_SERVERPARAMS_H
