#pragma once

#include "IMatrix.hpp"
#include "lab2_files/DynamicArray.h"
#include <stdexcept>
#include <cstddef>
#include <cmath>

template <typename T>
class DiagonalMatrix : public IMatrix<T> {
public:
    explicit DiagonalMatrix( size_t size ) : data_( size ), size_( size ) {
        for ( size_t i = 0; i < size_; ++i )
            data_[i] = T( 0 );
    }

    DiagonalMatrix( size_t size, const T* diag_values ) : data_( size ), size_( size ) {
        for ( size_t i = 0; i < size_; ++i )
            data_[i] = diag_values[i];
    }

    ~DiagonalMatrix() override = default;

    size_t GetRows() const override { return size_; }
    size_t GetCols() const override { return size_; }

    const T& Get( size_t i, size_t j ) const override {
        if ( i >= size_ || j >= size_ )
            throw std::out_of_range( "DiagonalMatrix::Get: index out of range" );

        if ( i == j ) return data_[i];
        return zero_val;
    }

    void Set( size_t i, size_t j, const T& value ) override {
        if ( i >= size_ || j >= size_ )
            throw std::out_of_range( "DiagonalMatrix::Set: index out of range" );

        if ( i == j ) {
            data_[i] = value;
        } else if ( value != zero_val ) {
            // Попытка записать не ноль вне диагонали
            throw std::invalid_argument( "DiagonalMatrix::Set: can't set non-zero value outside main diagonal" );
        }
    }

    IMatrix<T>& operator+=( const IMatrix<T>& other ) override {
        if ( size_ != other.GetRows() || size_ != other.GetCols() )
            throw std::invalid_argument( "DiagonalMatrix::+=: size mismatch" );

        if ( dynamic_cast<const DiagonalMatrix<T>*>( &other) ) {
            for ( size_t i = 0; i < size_; ++i ) {
                data_[i] += other.Get( i, i );
            }
        } else {
            // Если матрица не является диагональной, то проверяем, что вне диагонали у неё строго нули
            for ( size_t i = 0; i < size_; ++i ) {
                for ( size_t j = 0; j < size_; ++j ) {
                    if ( i != j && other.Get( i, j ) != zero_val )
                        throw std::invalid_argument( "DiagonalMatrix::+=: can't add a matrix with non-zero off-diagonal elements" );
                }
                data_[i] += other.Get( i, i );
            }
        }
        return *this;
    }

    // IMatrix<T>& operator+=( const IMatrix<T>& other ) override {
    //     if ( size_ != other.GetRows() || size_ != other.GetCols() )
    //         throw std::invalid_argument( "DiagonalMatrix::+=: size mismatch" );
    //
    //     // Для диагональной матрицы мы можем прибавлять только элементы на диагонали
    //     for ( size_t i = 0; i < size_; ++i ) {
    //         data_[i] += other.Get( i, i );
    //     }
    //
    //     return *this;
    // }

    IMatrix<T>& operator*=( const T& scalar ) override {
        for ( size_t i = 0; i < size_; ++i )
            data_[i] *= scalar;
        return *this;
    }

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

private:
    DynamicArray<T> data_; // Храним только диагональ
    size_t          size_;

    inline static const T zero_val { };

};
