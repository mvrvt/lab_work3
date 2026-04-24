#pragma once

#include "Matrix.hpp"
#include <iostream>

// Определение метода Print для класса Matrix<T>
template <typename T>
void Matrix<T>::Print() const {
    for (size_t i = 0; i < rows_; ++i) {
        std::cout << "[";
        for (size_t j = 0; j < cols_; ++j) {
            if (j == cols_ - 1)
                std::cout << Get(i, j);
            else
                std::cout << Get(i, j) << " ";
        }
        std::cout << "]" << std::endl;
    }
}
