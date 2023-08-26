//
// Created by illyasviel on 2023/8/21.
//

#ifndef HYLCHAN_STATE_H
#define HYLCHAN_STATE_H

template <typename EnumClass>
class State {
public:
    explicit State(EnumClass initEnum) : enumMember(initEnum) {}
    EnumClass getState() const { return enumMember; }
    void setState(EnumClass enumElement) { enumMember = enumElement; }

    bool operator==(const State& rhs) const { return enumMember == rhs.enumMember; }
    bool operator==(EnumClass rhs) const { return enumMember == rhs; }
private:
    EnumClass enumMember;
};


#endif //HYLCHAN_STATE_H
