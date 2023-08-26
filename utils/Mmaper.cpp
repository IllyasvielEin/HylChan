//
// Created by illyasviel on 2023/8/18.
//

#include "Mmaper.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <mutex>


void Mmaper::init() const {
    if (url_.empty()) {
        state_.setState(kNotFound);
        return;
    }

    int error;
    fileFd_ = openFile(&error);
    if (fileFd_ < 0) {
        switch (error) {
            case ENOENT: state_.setState(kNotFound); break;
            case EACCES: state_.setState(kForbidden); break;
            case EISDIR: state_.setState(kIsDir); break;
            default: state_.setState(kUnavailable);
        }
    }

    if (state_.getState() != kNone) {
        return;
    }

    struct stat st{};
    if (fstat(fileFd_, &st) == -1) {
        close(fileFd_);
        state_.setState(kUnavailable);
        fileFd_ = -1;
        return;
    }

    if (!S_ISREG(st.st_mode)) {
        return;
    }

    st_size_ = st.st_size;
    if (st_size_ < BIG_FILE_SIZE ) {
        fileMap_ = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fileFd_, 0);
        if (fileMap_ == MAP_FAILED) {
            close(fileFd_);
            state_.setState(kUnavailable);
            fileMap_ = nullptr;
            fileFd_ = -1;
            return;
        }
    }
    state_.setState(kStart);
}

void *Mmaper::getMap() const {

    return fileMap_;
}

int Mmaper::getFd() const {
    if (state_ == kStart) {
        int error;
        auto cpFd = openFile(&error);
        return cpFd;
    }
    return -1;
}

int Mmaper::openFile(int *error) const {
    int fd;
    do {
        fd = open(url_.c_str(), O_RDONLY);
        if (fd == -1) {
            *error = errno;
            if (*error == EINTR) {
                continue;
            } else {
                break;
            }
        }
    } while (fd == -1);
    return fd;
}

void Mmaper::reset() {

}

Mmaper::~Mmaper() {
    if (state_.getState() == kStart) {
        if (fileMap_ != nullptr) {
            munmap(fileMap_, st_size_);
        }
        if (fileFd_ != -1) {
            close(fileFd_);
        }
        state_.setState(kClose);
        fileMap_ = nullptr;
        fileFd_ = -1;
        st_size_ = 0;
    }
}


