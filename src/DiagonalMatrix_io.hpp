#pragma once

#include "DiagonalMatrix.hpp"
#include <iostream>

template <typename T>
void DiagonalMatrix<T>::Print() const {
    for ( size_t i = 0; i < size_; ++i ) {
        std::cout << "[";
        for ( size_t j = 0; j < size_; ++j ) {
            std::cout << Get( i, j ) << ( j == size_ - 1 ? "" : " " );
        }
        std::cout << "]\n";
    }
}
