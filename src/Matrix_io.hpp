#pragma once

#include <iostream>
#include "Matrix.hpp"
#include "Complex_io.hpp"

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
