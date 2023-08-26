//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_MMAPER_H
#define HYLCHAN_MMAPER_H


#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <shared_mutex>

#include "State.h"
#include "NonCopyable.h"

class Mmaper : NonCopyable {
public:
    static constexpr int BIG_FILE_SIZE = 1024;

    enum StateImpl {
        kNone,
        kClose,
        kNotFound,
        kForbidden,
        kIsDir,
        kUnavailable,
        kStart,
    };

    explicit Mmaper(const char* url) :
            state_(kNone),
            url_(url),
            fileMap_(nullptr),
            fileFd_(-1),
            st_size_(0)
    {

    }

    explicit Mmaper(std::string url) :
            state_(kNone),
            url_(std::move(url)),
            fileMap_(nullptr),
            fileFd_(-1),
            st_size_(0)
    {

    }

    ~Mmaper();

    void init() const;

    void* getMap() const;

    int getFd() const;

    StateImpl getState() const { return state_.getState(); }

    auto getFileSize() const { return st_size_; }

private:
    int openFile(int *error) const;

    void reset();
private:
    mutable State<StateImpl> state_;
    mutable std::string url_;
    mutable void* fileMap_;
    mutable int fileFd_;
    mutable size_t st_size_;
};

using MmaperPtr = std::shared_ptr<Mmaper>;

#endif //HYLCHAN_MMAPER_H
