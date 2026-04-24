#pragma once

#include "IMatrix.hpp"
#include "lab2_files/DynamicArray.h"
#include <cmath>
#include <stdexcept>
#include <cstddef>

template <typename T>
class Matrix : public IMatrix<T> {
public:
    // Конструкторы
    Matrix( size_t rows, size_t cols ) : data_( rows * cols ), rows_( rows ), cols_( cols ) {
        this->Fill( T( 0 ) );
    }

    Matrix( size_t rows, size_t cols, const T* values, size_t count ) : data_( rows * cols ), rows_( rows ), cols_( cols ) {
        if ( count != rows * cols )
            throw std::invalid_argument( "Matrix: data count does not match dimesions" );
        for ( size_t idx = 0; idx < count; ++idx )
            data_[idx] = values[idx];
    }

    // Копирование и перемещение (по умолчанию, поскольку DynamicArray поддерживает)
    Matrix( const Matrix& ) = default;
    Matrix( Matrix&& )      = default;
    Matrix& operator=( const Matrix& ) = default;
    Matrix& operator=( Matrix&& )      = default;

    virtual ~Matrix() = default;

    // Реализация интерфейса IMatrix
    size_t GetRows() const override { return rows_; }
    size_t GetCols() const override { return cols_; }

    const T& Get( size_t i, size_t j ) const override {
        if ( i >= rows_ || j >= cols_ )
            throw std::out_of_range( "Matrix::Get: index out of range" );
        return data_[Index( i, j )];
    }

    void Set( size_t i, size_t j, const T& value ) override {
        if ( i >= rows_ || j >= cols_ )
            throw std::out_of_range( "Matrix::Set: index out of range" );
        data_[Index( i, j )] = value;
    }

    Matrix<T>& operator+=( const IMatrix<T>& other ) override {
        if ( rows_ != other.GetRows() || cols_ != other.GetCols() )
            throw std::invalid_argument( "Matrix::+=: size mismatch" );
        for ( size_t i = 0; i < rows_; ++i )
            for ( size_t j = 0; j < cols_; ++j )
               data_[Index( i, j )] += other.Get( i, j );
        return *this;
    }

    Matrix<T>& operator*=( const T& scalar ) override {
        for ( size_t idx = 0; idx < rows_ * cols_; ++idx )
            data_[idx] *= scalar;
        return *this;
    }

    T Norm() const override {
        double sum = 0.0;
        for ( size_t i = 0; i < rows_ * cols_; ++i ) {
            using std::abs; // Разрешаем использовать std::abs для базовых типов
            double mod = static_cast<double>( abs( data_[i] ) );
            sum += mod * mod;
        }
        return static_cast<T>( std::sqrt( sum ) );
    }

    void Print() const override;

    // Доп. метод (не входит в интерфейс)
    void Fill( const T& value ) {
        for ( size_t i = 0; i < rows_ * cols_; ++i )
            data_[i] = value;
    }

    // ----- Элементарные преобразования строк (как следствие и столбцов) -----
    void MultiplyRow( size_t row, const T& scalar ) {
        if ( row >= rows_ ) throw std::out_of_range( "MultiplyRow: row index out of range" );
        if ( scalar == T(0) ) throw std::out_of_range( "MultiplyRow: can't multiply by zero" );
        for ( size_t j = 0; j < cols_; ++j ) {
            this->Set( row, j, this->Get( row, j ) * scalar );
        }
    }

    void SwapRows( size_t row1, size_t row2 ) {
        if ( row1 >= rows_ || row2 >= rows_ ) throw std::out_of_range( "SwapRows: index out of range" );
        if ( row1 == row2 ) return;
        for ( size_t j = 0; j < cols_; ++j ) {
            T temp = this->Get( row1, j );
            this->Set( row1, j, this->Get( row2, j ) );
            this->Set( row2, j, temp );
        }
    }

    void AddRowMultiplied( size_t target_row, size_t source_row, const T& scalar ) {
        if ( target_row >= rows_ || source_row >= rows_ )
            throw std::out_of_range( "AddRowMultiplied: row index out of range" );
        for ( size_t j = 0; j < cols_; ++j ) {
            this->Set( target_row, j, this->Get( target_row, j ) + this->Get( source_row, j ) * scalar );
        }
    }

protected:
    DynamicArray<T> data_;
    size_t          rows_;
    size_t          cols_;

    size_t Index( size_t i, size_t j ) const {
        return i * cols_ + j;
    }

};

// Свободный operator+ (создаёт новую матрицу)
template <typename T>
Matrix<T> operator+( const IMatrix<T>& a, const IMatrix<T>& b ) {
	if ( a.GetRows() != b.GetRows() || a.GetCols() != b.GetCols() )
		throw std::invalid_argument( "Matrix size mismatch" );
    Matrix<T> result( a.GetRows(), a.GetCols() );
    for ( size_t i = 0; i < a.GetRows(); ++i )
        for ( size_t j = 0; j < a.GetCols(); ++j )
            result.Set( i, j, a.Get( i, j ) + b.Get( i, j ) );
    return result;
}

// Свободный operator* для умножения матриц (требует, чтобы a.cols == b.rows)
template <typename T>
Matrix<T> operator*( const IMatrix<T>& a, const IMatrix<T>& b ) {
    if ( a.GetCols() != b.GetRows() )
        throw std::invalid_argument( "Matrix multiplication: incompatible dimentions" );
    Matrix<T> result( a.GetRows(), b.GetCols() );
    for ( size_t i = 0; i < result.GetRows(); ++i ) {
        for ( size_t j = 0; j < result.GetCols(); ++j ) {
            T sum = T(0);
            for ( size_t k = 0; k < a.GetCols(); ++k )
                sum = sum + a.Get( i, k ) * b.Get( k, j );
            result.Set( i, j, sum );
        }
    }
    return result;
}
