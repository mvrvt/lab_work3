#pragma once

#include "IMatrix.h"
#include "lab2_files/DynamicArray.h"
#include <stdexcept>
#include <cstddef>
#include <cmath>

template <typename T>
class DiagonalMatrix : public IMatrix<T> {
private:
    DynamicArray<T> data_; // Храним только диагональ
    size_t size_;
    T zero_;               // Заглушка для возврата по ссылке нулевых элементов

public:
    DiagonalMatrix( size_t size ) : data_( size ), size_( size ), zero_( T(0) ) {
        for ( size_t i = 0; i < size_; ++i )
            data_[i] = T(0);
    }

    DiagonalMatrix( size_t size, const T* diag_values ) : data_( size ), size_( size ), zero_( T(0) ) {
        for ( size_t i = 0; i < size_; ++i )
            data_[i] = diag_values[i];
    }

    size_t GetRows() const override { return size_; }
    size_t GetCols() const override { return size_; }

    const T& Get( size_t i, size_t j ) const override {
        if ( i >= size_ || j >= size_ )
            throw std::out_of_range( "DiagonalMatrix::Get: index out of range" );

        if ( i == j ) return data_[i];
        return zero_;
    }

    void Set( size_t i, size_t j, const T& value ) override {
        if ( i >= size_ || j >= size_ )
            throw std::out_of_range( "DiagonalMatrix::Set: index out of range" );

        if ( i == j ) {
            data_[i] = value;
        } else if ( value != zero_) {
            // Попытка записать не ноль вне диагонали
            throw std::invalid_argument( "DiagonalMatrix::Set: can't set non-zero value outside main diagonal" );
        }
    }

    IMatrix<T>& operator+=( const IMatrix<T>& other ) override {
        if ( size_ != other.GetRows() || size_ != other.GetCols() )
            throw std::invalid_argument( "DiagonalMatrix::+=: size mismatch" );

        // Для диагональной матрицы мы можем прибавлять только элементы на диагонали
        for ( size_t i = 0; i < size_; ++i ) {
            data_[i] += other.Get( i, i );
        }

        return *this;
    }

    IMatrix<T>& operator*=( const T& scalar ) override {
        for ( size_t i = 0; i < size_; ++i )
            data_[i] *= scalar;
        return *this;
    }

    // T Norm() const override {
    //     T sum = T(0);
    //     for ( size_t i = 0; i < size_; ++i ) {
    //         sum += data_[i] * data_[i];
    //     }
    //     return static_cast<T>( std::sqrt( static_cast<double>( sum ) ) );
    // }

    T Norm() const override {
        double sum = 0.0;
        for ( size_t i = 0; i < size_; ++i ) {
            using std::abs;
            double mod = static_cast<double>( abs( data_[i] ) );
            sum += mod * mod;
        }
        return static_cast<T>( std::sqrt( sum ) );
    }

    void Print() const override;

    ~DiagonalMatrix() override = default;
};
