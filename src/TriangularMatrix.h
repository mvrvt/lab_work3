#pragma once

#include "Matrix.h"
#include <cmath>
#include <stdexcept>

enum class TriangularType { Upper, Lower };

template <typename T>
class TriangularMatrix : public Matrix<T> {
private:
    TriangularType type_;
    void CheckSet( size_t i, size_t j ) const {
        if ( type_ == TriangularType::Upper && i > j )
            throw std::invalid_argument( "UpperTriangularMatrix: cannot set below diagonal" );
        if ( type_ == TriangularType::Lower && i < j )
            throw std::invalid_argument( "LowerTriangularMatrix: cannot set above diagonal" );
    }

public:
    TriangularMatrix( size_t size, TriangularType type = TriangularType::Upper ) : Matrix<T>( size, size ), type_( type ) { }

    // TriangularMatrix( size_t size, TriangularType type, const T* values, size_t count ) : Matrix<T>( size, size ), type_( type ) {
    //     size_t expected = size * ( size + 1 ) / 2;
    //     if ( count != expected )
    //         throw std::invalid_argument( "TriangularMatrix: data count mismatch" );
    //     size_t idx = 0;
    //     if ( type == TriangularType::Upper ) {
    //         for ( size_t i = 0; i < size; ++i )
    //             for ( size_t j = 0; j < size; ++j )
    //                 this->Set( i, j, values[idx++] );
    //     }
    // }
    TriangularMatrix( size_t size, TriangularType type, const T* values, size_t count )
        : Matrix<T>( size, size ), type_( type ) {

        size_t expected = size * ( size + 1 ) / 2;
        if ( count != expected )
            throw std::invalid_argument( "TriangularMatrix: data count mismatch" );

        // Зануляем всю матрицу, чтобы в пустой половине не было мусора
        this->Fill( T( 0 ) );

        size_t idx = 0;
        if ( type == TriangularType::Upper ) {
            // Идем по строкам, заполняем элементы от главной диагонали и правее
            for ( size_t i = 0; i < size; ++i ) {
                for ( size_t j = i; j < size; ++j ) { // j начинается с i
                    this->Set( i, j, values[idx++] );
                }
            }
        } else {
            // Идем по строкам, заполняем элементы от начала строки до главной диагонали
            for ( size_t i = 0; i < size; ++i ) {
                for ( size_t j = 0; j <= i; ++j ) { // j идет до i включительно
                    this->Set( i, j, values[idx++] );
                }
            }
        }
    }

    TriangularMatrix( const TriangularMatrix& ) = default;
    TriangularMatrix( TriangularMatrix&& ) = default;
    TriangularMatrix& operator=( const TriangularMatrix& ) = default;
    ~TriangularMatrix() = default;

    void Set( size_t i, size_t j, const T& value ) override {
        CheckSet( i, j );
        Matrix<T>::Set( i, j, value );
    }

    T Determinant() const {
        T det = T(1);
        for ( size_t i = 0; i < this->GetRows(); ++i )
            det = det * this->Get( i, i );
        return det;
    }

    TriangularMatrix<T> Transpose() const {
        TriangularType new_type = ( type_ == TriangularType::Upper ) ? TriangularType::Lower : TriangularType::Upper;
        TriangularMatrix<T> result( this->GetRows(), new_type );

        // Итерируется только по значимой части матрицы
        if ( type_ == TriangularType::Upper ) {
            for ( size_t i = 0; i < this->GetRows(); ++i ) {
                for ( size_t j = i; j < this->GetCols(); ++j ) {
                    result.Set( j, i, this->Get( i, j ) ); // Меняем i и j местами при записи
                }
            }
        } else {
            for ( size_t i = 0; i < this->GetRows(); ++i ) {
                for ( size_t j = 0; j <= i; ++j ) {
                    result.Set( j, i, this->Get( i, j ) );
                }
            }
        }

        return result;
    }

    TriangularType GetType() const { return type_; }
};
