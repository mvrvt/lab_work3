#pragma once

#include "IEnumerator.h"

template <class T>
class IEnumerable {
public:
    virtual IEnumerator<T>* GetEnumerator() = 0; // создать и вернуть итератор

    virtual ~IEnumerable() = default;
};
