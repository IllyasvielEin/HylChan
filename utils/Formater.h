//
// Created by illyasviel on 2023/8/18.
//

#ifndef HYLCHAN_FORMATER_H
#define HYLCHAN_FORMATER_H

#include <string>

namespace fmt {

    std::string format(const char* str) {
        while (*str) {
            if (*str == '{' && *(++str) == '}') {

            }
        }
    }

    template <typename ...Args>
    std::string format(const char* str, Args ...args) {

    }

} // fmt

#endif //HYLCHAN_FORMATER_H
