//
// Created by illyasviel on 2023/8/26.
//

#include "Filer.h"

extern const int BIG_FILE_SIZE = 1024 * 1024 * 1024;

void Filer::init()  {
    if (!mmaper_) return;
    size_ = mmaper_->getFileSize();
    left_ = size_;
    if (size_ >= BIG_FILE_SIZE) {
        sendCallBack = [this](int client_fd) { return sendBigFile(client_fd);};
    }
    else {
        sendCallBack = [this](int client_fd) { return sendSmallFile(client_fd);};
    }
    state_.setState(kStart);
}

void Filer::sendSmallFile(int client_fd) {
    ssize_t n;

    int error;
    auto mmapAddr = (char*)mmaper_->getMap();
    if (mmapAddr == nullptr) {
        return;
    }
    do {
        n = socketopt::send(client_fd, mmapAddr + offset_, left_, &error);
        if (n == -1) {
            if (error == EINTR) {
                continue;
            }
            break;
        }
        offset_ += n;
        left_ -= n;
    } while (left_ > 0);
}

void Filer::sendBigFile(int client_fd) {
    if (cpFd == -1) {
        cpFd = mmaper_->getFd();
        if (cpFd == -1) {
            return;
        }
    }

    ssize_t n;
    do {
        n = sendfile(client_fd, cpFd, &offset_, 4096);
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        left_ -= n;
    } while (n > 0);
}
