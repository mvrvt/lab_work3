#pragma once

#include <iostream>
#include "TriangularMatrix.hpp"

template <typename T>
void TriangularMatrix<T>::Print() const {
    for (size_t i = 0; i < size_; ++i) {
        std::cout << "[ ";
        for (size_t j = 0; j < size_; ++j) {
            std::cout << Get(i, j) << " ";
        }
        std::cout << "]\n";
    }
}
