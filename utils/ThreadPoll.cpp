//
// Created by illyasviel on 2023/8/22.
//

#include "ThreadPoll.h"

ThreadPoll::~ThreadPoll() {
    {
        std::unique_lock<std::mutex> uniqueLock(mutex_);
        state_.setState(kClose);
    }
    cv.notify_all();
    for (auto& i : threads_) {
        i.join();
    }
}

ThreadPoll::ThreadPoll(size_t threadCount, size_t maxTasks) :
        state_(kNone),
        maxTasks_(maxTasks)
{
    for (auto i = 0; i < threadCount; ++i) {
        threads_.emplace_back([this](){
            while (true) {
                std::unique_lock<std::mutex> uniqueLock(mutex_);
                cv.wait(uniqueLock, [this]() { return !taskQueue_.empty() || state_.getState() == kClose; });
                if (state_.getState() != kStart) {
                    break;
                }
                auto task = taskQueue_.front();
                taskQueue_.pop();
                task();
            }
        });
    }
    state_.setState(kStart);
}
