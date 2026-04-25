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

    // Дополнение: возведение в степень (целая неотрицательная)
    SquareMatrix<T> Power( int exponent ) const {
        if ( exponent < 0 )
            throw std::invalid_argument( "Power: exponent must be non-negative" );

        if ( exponent == 0 ) {
            SquareMatrix<T> sqr_matrix( this->rows_ );
            for ( size_t i = 0; i < this->rows_; ++i )
                sqr_matrix.Set( i, i, T(1) );
            return sqr_matrix;
        }
        SquareMatrix<T> result = *this;
        SquareMatrix<T> base = *this;
        int exp = exponent - 1;
        while ( exp > 0 ) {
            if ( exp & 1 )
                result = SquareMatrix<T>( result * base );
            base = SquareMatrix<T>( base * base );
            exp >>= 1;
        }
        return result;
    }
};
