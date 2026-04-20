#pragma once

#include "lab2_files/DynamicArray.h"
#include "IMatrix.hpp"
#include <cmath>
#include <stdexcept>

template <typename T>
class Vector {
public:
    // Конструкторы
    explicit Vector( size_t size ) : data_( size ), size_( size ) {
        for ( size_t i = 0; i < size_; ++i )
            data_[i] = T( 0 );
    }

    Vector( size_t size, const T* values ) : data_( size ), size_( size ) {
        for ( size_t i = 0; i < size_; ++i )
            data_[i] = values[i];
    }

    // Копирование и перемещение
    Vector( const Vector& ) = default;
    Vector( Vector&& ) = default;
    Vector& operator=( const Vector& ) = default;
    Vector& operator=( Vector&& ) = default;
    ~Vector() = default;

    size_t GetSize() const { return size_; }

    // Безопасный доступ по индексу
    T& operator[]( size_t i ) {
        if ( i >= size_ ) throw std::out_of_range( "Vector: index out of range" );
        return data_[i];
    }

    const T& operator[]( size_t i ) const {
        if ( i >= size_ ) throw std::out_of_range( "Vector: index out of range" );
        return data_[i];
    }

    // Математика для СЛАУ
    // Евклидова длина вектора
    T Norm() const {
        double sum = 0.0;
        for ( size_t i = 0; i < size_; ++i ) {
            using std::abs;
            double mod = static_cast<double>( abs( data_[i] ) );
            sum += mod * mod;
        }
        return static_cast<T>( std::sqrt( sum ) );
    }

    // Скалярное произведение
    T Dot( const Vector<T>& other ) const {
        if ( size_ != other.size_ )
            throw std::invalid_argument( "Vector::Dot: sizes mismatch" );

        T sum = T( 0 );
        for ( size_t i = 0; i < size_; ++i )
            sum += data_[i] * other.data_[i];
        return sum;
    }

    // Операторы модификации
    Vector<T>& operator+=( const Vector<T>& other ) {
        if ( size_ != other.size_ )
            throw std::invalid_argument( "Vector::+=: sizes mismatch" );
        for ( size_t i = 0; i < size_; ++i )
            data_[i] += other.data_[i];
        return *this;
    }

    Vector<T>& operator-=( const Vector<T>& other ) {
        if ( size_ != other.size_ )
            throw std::invalid_argument( "Vector::-=: sizes mismatch" );
        for ( size_t i = 0; i < size_; ++i )
            data_[i] = data_[i] - other.data_[i];
        return *this;
    }

    Vector<T>& operator*=( const T& scalar ) {
        for ( size_t i = 0; i < size_; ++i )
            data_[i] *= scalar;
        return *this;
    }

private:
    DynamicArray<T> data_;
    size_t size_;
};

// Свободные операторы:

template <typename T>
Vector<T> operator+( const Vector<T>& a, const Vector<T>& b ) {
    Vector<T> result = a;
    result += b;
    return result;
}

template <typename T>
Vector<T> operator-( const Vector<T>& a, const Vector<T>& b ) {
    Vector<T> result = a;
    result -= b;
    return result;
}

template <typename T>
Vector<T> operator*( const Vector<T>& vec, const T& scalar ) {
    Vector<T> result = vec;
    result *= scalar;
    return result;
}

template <typename T>
Vector<T> operator*( const T& scalar, const Vector<T>& vec ) { return vec * scalar; }

// Умножение матрицы на вектор (нужно для решения СЛАУ)
template <typename T>
Vector<T> operator*( const IMatrix<T>& mat, const Vector<T>& vec ) {
    if ( mat.GetCols() != vec.GetSize() )
        throw std::invalid_argument( "Matrix * Vector: incompatible dimensions" );

    Vector<T> result( mat.GetRows() );
    for ( size_t i = 0; i < mat.GetRows(); ++i ) {
        T sum = T( 0 );
        for ( size_t j = 0; j < mat.GetCols(); ++j ) {
            sum += mat.Get( i, j ) * vec[j];
        }
        result[i] = sum;
    }
    return result;
}
