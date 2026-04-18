#pragma once

#include <iostream>
#include "SparseMatrix.h"
#include "Complex_io.h"

template <typename T>
void SparseMatrix<T>::Print() const {
    for ( size_t i = 0; i < rows_; ++i ) {
        std::cout << "[";
        for ( size_t j = 0; j < cols_; ++j ) {
            std::cout << Get( i, j ) << ( j == cols_ - 1 ? "" : " " );
        }
        std::cout << "]\n";
    }
}
