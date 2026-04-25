#pragma once

#include "IMatrix.hpp"
#include "lab2_files/DynamicArray.h"
#include <stdexcept>
#include <cmath>
#include <cstddef>

enum class TriangularType { Upper, Lower };

template <typename T>
class TriangularMatrix : public IMatrix<T> {
public:
    explicit TriangularMatrix( size_t size, TriangularType type = TriangularType::Upper )
    : data_( size * ( size + 1) / 2 ), size_( size ), type_( type ) {
        for ( size_t k = 0; k < data_.GetCount(); ++k )
            data_[k] = zero_val;
    }

    ~TriangularMatrix() override = default;

    size_t GetRows() const override { return size_; }
    size_t GetCols() const override { return size_; }

    const T& Get( size_t i, size_t j ) const override {
        if ( i >= size_ || j >= size_ )
            throw std::out_of_range( "TriangularMatrix::Get: index out of range" );

        if ( IsInTriangle( i, j ) ) {
            return data_.Get( GetIndex( i, j ) );
        }
        return zero_val;
    }

    void Set( size_t i, size_t j, const T& value ) override {
        if ( i >= size_ || j >= size_ )
            throw std::out_of_range( "TriangularMatrix::Set: index out of range" );

        if ( IsInTriangle( i, j ) ) {
            data_[GetIndex( i, j )] = value;
        } else if ( value != zero_val ) {
            throw std::invalid_argument( "TriangularMatrix::Set: can't set non-zero value outside the triangle" );
        }
    }

    IMatrix<T>& operator+=( const IMatrix<T>& other ) override {
        if ( size_ != other.GetRows() || size_ != other.GetCols() )
            throw std::invalid_argument( "TriangularMatrix::+=: size mismatch" );

        // Если other - треугольная матрица того же типа
        const auto* tri_other = dynamic_cast<const TriangularMatrix<T>*>( &other );
        if ( tri_other && tri_other->type_ == this->type_ ) {
            for ( size_t k = 0; k < data_.GetCount(); ++k ) {
                data_[k] += tri_other->data_.Get( k );
            }
        } else {
            // Если other - другая матрица, сначала проверяем её на безопасность
            for ( size_t i = 0; i < size_; ++i ) {
                for ( size_t j = 0; j < size_; ++j ) {
                    if ( !IsInTriangle( i, j ) && other.Get( i, j ) != zero_val )
                        throw std::invalid_argument( "TriangularMatrix::+=: can't add a matrix with non-zero elements outside the triangle" );
                }
            }

            // Если проверка пройдена, безопасно модифицируем данные
            for ( size_t i = 0; i < size_; ++i ) {
                for ( size_t j = 0; j < size_; ++j ) {
                    if ( IsInTriangle( i, j ) ) {
                        data_[GetIndex( i, j)] += other.Get( i, j );
                    }
                }
            }
        }
        return *this;
    }

    IMatrix<T>& operator*=(const T& scalar) override {
        for (size_t k = 0; k < data_.GetCount(); ++k) {
            data_[k] *= scalar;
        }
        return *this;
    }

    T Norm() const override {
        double sum = 0.0;
        for ( size_t k = 0; k < data_.GetCount(); ++k ) {
            using std::abs;
            double mod = static_cast<double>( abs(data_[k] ) );
            sum += mod * mod;
        }
        return static_cast<T>( std::sqrt( sum ) );
    }

    T Determinant() const {
        T det = T( 1 );
        for ( size_t i = 0; i < size_; ++i )
            det = det * Get( i, i );
        return det;
    }

    void Print() const override;

private:
    DynamicArray<T> data_;
    size_t          size_;
    TriangularType  type_;

    inline static const T zero_val { };

    // Вспомогательный метод для вычисления индекса в массиве
    size_t GetIndex( size_t i, size_t j ) const {
        if ( type_ == TriangularType::Lower ) {
            // Сумма арифметической прогрессии: количество элементов в строке до i
            return ( i * ( i + 1 ) ) / 2 + j;
        } else {
            // Для Upper: пропускаем пустые ячейки слева
            // Индекс = (кол-во элементов) - (эл-ты в оставшемся треугольнике снизу)
            return i * size_ - ( i * ( i - 1 ) ) / 2 + ( j - i );
        }
    }

    bool IsInTriangle( size_t i, size_t j ) const {
        if ( type_ == TriangularType::Lower ) return i >= j;
        return i <= j;
    }
};
