//
// Created by illyasviel on 2023/8/26.
//

#ifndef HYLCHAN_FILER_H
#define HYLCHAN_FILER_H


#include <sys/sendfile.h>
#include "Mmaper.h"
#include "SocketOpt.h"

class Filer {
public:
    enum StateImpl {
        kNone,
        kStart,
        kClose
    };

    static constexpr int BIG_FILE_SIZE = 1024 * 1024 * 1024;

    Filer() :
            state_(kNone),
            mmaper_(nullptr),
            offset_(0),
            size_(0),
            left_(size_),
            cpFd(-1)
    {

    }

    Filer(const MmaperPtr& mmaperPtr) :
            state_(kNone),
            mmaper_(mmaperPtr),
            offset_(0),
            size_(0),
            left_(size_),
            cpFd(-1)
    {
        init();
    }

    Filer(MmaperPtr&& mmaperPtr) :
            state_(kNone),
            mmaper_(std::move(mmaperPtr)),
            offset_(0),
            size_(0),
            left_(size_),
            cpFd(-1)
    {
        init();
    }

    ~Filer() {
        close(cpFd);
        state_.setState(kClose);
    }

    void exchange(const MmaperPtr& mmaperPtr) {
        mmaper_ = mmaperPtr;
        sendCallBack = nullptr;
        init();
    }

    int send(int client_fd) {
        if (state_.getState() != kStart) {
            return -1;
        }

        sendCallBack(client_fd);
        if (left_ == 0) {
            resetRWPtr();
        }
        return left_ == 0? 0 : 1;
    }

private:
    void init();

    void sendSmallFile(int client_fd);

    void sendBigFile(int client_fd);

    void resetRWPtr() {
        offset_ = 0;
        left_ = size_;
        if (cpFd != -1) {
            lseek(cpFd, 0, SEEK_SET);
        }
    }
private:
    State<StateImpl> state_;
    std::function<void(int)> sendCallBack;

    MmaperPtr   mmaper_;
    size_t      size_;
    off_t       offset_;
    size_t      left_;
    int         cpFd;
};

using FilerPtr = std::unique_ptr<Filer>;

#endif //HYLCHAN_FILER_H
