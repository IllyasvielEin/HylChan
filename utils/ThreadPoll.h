//
// Created by illyasviel on 2023/8/22.
//

#ifndef HYLCHAN_THREADPOLL_H
#define HYLCHAN_THREADPOLL_H

#include <vector>
#include <thread>
#include <condition_variable>
#include <functional>
#include <queue>

#include "State.h"

class ThreadPoll {
public:
    enum StateImpl {
        kNone,
        kStart,
        kClose
    };

    explicit ThreadPoll(size_t threadCount, size_t maxTasks);

    ~ThreadPoll();

    template<typename Func>
    bool enqueueTask(const Func& func) {
        bool ok = true;
        {
            std::unique_lock<std::mutex> uniqueLock(mutex_);
            if (taskQueue_.size() < maxTasks_) {
                taskQueue_.emplace(std::move(func));
            }
            else {
                ok = false;
            }
        }
        cv.notify_one();
        return ok;
    }

    auto getState() const { return state_.getState(); }
private:
    State<StateImpl> state_;
    const size_t maxTasks_;
    std::mutex mutex_;
    std::condition_variable cv;
    std::queue<std::function<void()>> taskQueue_;
    std::vector<std::thread> threads_;
};

using ThreadPollPtr = std::unique_ptr<ThreadPoll>;

#endif //HYLCHAN_THREADPOLL_H
