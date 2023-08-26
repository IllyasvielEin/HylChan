//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_NONCOPYABLE_H
#define HYLCHAN_NONCOPYABLE_H


class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};


#endif //HYLCHAN_NONCOPYABLE_H
