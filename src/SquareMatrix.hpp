#pragma once

#include "Matrix.hpp"
#include "TriangularMatrix.hpp"
#include <stdexcept>
#include <cmath>

template <typename T>
class SquareMatrix : public Matrix<T> {
public:
    // Конструкторы
    SquareMatrix( size_t size ) : Matrix<T>( size, size ) { }

    SquareMatrix( size_t size, const T* values, size_t count ) : Matrix<T>( size, size, values, count ) {
        if ( count != size * size )
            throw std::invalid_argument( "SquareMatrix: data count doesn't match (size * size) " );
    }

    SquareMatrix( const Matrix<T>& base_matrix ) : Matrix<T>( base_matrix ) {
        if ( base_matrix.GetRows() != base_matrix.GetCols() )
            throw std::invalid_argument("Cannot convert non-square Matrix to SquareMatrix");
    }

    SquareMatrix( const SquareMatrix& other ) = default;
    SquareMatrix( SquareMatrix&& other ) = default;
    SquareMatrix& operator=( const SquareMatrix& other ) = default;
    SquareMatrix& operator=( SquareMatrix&& other ) = default;
    ~SquareMatrix() override = default;

    // След матрицы
    T Trace() const {
        T trace = T(0);
        for ( size_t i = 0; i < this->rows_; ++i )
            trace += this->Get( i, i );
        return trace;
    }

    // Транспонирование (возвращает новую квадратную матрицу)
    SquareMatrix<T>Transpose() const {
        SquareMatrix<T> result( this->rows_);
        for ( size_t i = 0; i < this->rows_; ++i )
            for ( size_t j = 0; j < this->cols_; ++j )
                result.Set( j, i, this->Get( i, j ) );
        return result;
    }
};
