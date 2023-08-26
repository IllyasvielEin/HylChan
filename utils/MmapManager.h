//
// Created by illyasviel on 2023/8/24.
//

#ifndef HYLCHAN_MMAPMANAGER_H
#define HYLCHAN_MMAPMANAGER_H

#include <unordered_map>
#include <mutex>
#include "Mmaper.h"

class MmapManager {
public:
    MmaperPtr getFiler(const std::string& file) {
        std::unique_lock<std::mutex> uniqueLock(mutex);
        if (mmap_manager_.count(file) == 0) {
            mmap_manager_.emplace(file, std::make_shared<Mmaper>(file));
            mmap_manager_[file]->init();
        }
        return mmap_manager_[file];
    }

    static MmapManager& getMmapmanger() {
        static MmapManager mmapMannger;
        return mmapMannger;
    }
private:
    explicit MmapManager(size_t max_mmap_count = 100) : max_mmap_count_(max_mmap_count) {}

    std::mutex mutex;
    const size_t max_mmap_count_;
    std::unordered_map<std::string, MmaperPtr> mmap_manager_;
};


#endif //HYLCHAN_MMAPMANAGER_H
