#pragma once

#include "BandMatrix.hpp"
#include "lab2_files/DynamicArray.h"

template <typename T>
class DiagonalMatrix : public BandMatrix<T> {
public:
    explicit DiagonalMatrix( size_t size )
        : BandMatrix<T>( size, DynamicArray<int>( {0} ) ) { }

    DiagonalMatrix( size_t size, const T* diag_values )
        : BandMatrix<T>( size, DynamicArray<int>( {0} ) ) {
        for ( size_t i = 0; i < size; ++i )
            this->Set( i, i, diag_values[i] );
    }

    DiagonalMatrix( const DiagonalMatrix& ) = default;
    DiagonalMatrix& operator=( const DiagonalMatrix& ) = default;
    ~DiagonalMatrix() override = default;
};
