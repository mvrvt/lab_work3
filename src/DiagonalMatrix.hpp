#pragma once

#include "BandMatrix.hpp"
#include "lab2_files/DynamicArray.h"

template <typename T>
class DiagonalMatrix : public BandMatrix<T> {
private:
    static DynamicArray<int> MakeSingleOffset() {
        DynamicArray<int> arr(1);   // размер 1, все элементы инициализированы 0
        arr.Set(0, 0);
        return arr;
    }

public:
    explicit DiagonalMatrix( size_t size )
        : BandMatrix<T>( size, MakeSingleOffset() ) { }

    DiagonalMatrix( size_t size, const T* diag_values )
        : BandMatrix<T>( size, MakeSingleOffset() ) {
        for ( size_t i = 0; i < size; ++i )
            this->Set( i, i, diag_values[i] );
    }

    DiagonalMatrix( const DiagonalMatrix& ) = default;
    DiagonalMatrix& operator=( const DiagonalMatrix& ) = default;
    ~DiagonalMatrix() override = default;
};