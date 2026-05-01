#pragma once

#include "IMatrix.hpp"
#include "Matrix.hpp"   // для создания результата
#include <stdexcept>
#include <type_traits>
#include <cmath>
#include <limits>

// ---------- Бинарные операторы, возвращающие Matrix<T> ----------

template <typename T>
Matrix<T> operator+( const IMatrix<T>& a, const IMatrix<T>& b ) {
    if ( a.GetRows() != b.GetRows() || a.GetCols() != b.GetCols() )
        throw std::invalid_argument( "operator+: size mismatch" );
    Matrix<T> result( a.GetRows(), a.GetCols() );
    for ( size_t i = 0; i < a.GetRows(); ++i )
        for ( size_t j = 0; j < a.GetCols(); ++j )
            result.Set( i, j, a.Get( i, j ) + b.Get( i, j ) );
    return result;
}

template <typename T>
Matrix<T> operator-( const IMatrix<T>& a, const IMatrix<T>& b ) {
    if ( a.GetRows() != b.GetRows() || a.GetCols() != b.GetCols() )
        throw std::invalid_argument( "operator-: size mismatch" );
    Matrix<T> result( a.GetRows(), a.GetCols() );
    for ( size_t i = 0; i < a.GetRows(); ++i )
        for ( size_t j = 0; j < a.GetCols(); ++j )
            result.Set( i, j, a.Get( i, j ) - b.Get( i, j ) );
    return result;
}

// Умножение матриц
template <typename T>
Matrix<T> operator*( const IMatrix<T>& a, const IMatrix<T>& b ) {
    if ( a.GetCols() != b.GetRows() )
        throw std::invalid_argument( "operator*: incompatible dimensions for multiplication" );
    Matrix<T> result( a.GetRows(), b.GetCols() );
    for ( size_t i = 0; i < result.GetRows(); ++i )
        for ( size_t j = 0; j < result.GetCols(); ++j ) {
            T sum = T(0);
            for ( size_t k = 0; k < a.GetCols(); ++k )
                sum += a.Get( i, k ) * b.Get( k, j );
            result.Set( i, j, sum );
        }
    return result;
}

// Умножение матрицы на скаляр (слева и справа)
template <typename T>
Matrix<T> operator*( const IMatrix<T>& mat, const T& scalar ) {
    Matrix<T> result( mat.GetRows(), mat.GetCols() );
    for ( size_t i = 0; i < mat.GetRows(); ++i )
        for ( size_t j = 0; j < mat.GetCols(); ++j )
            result.Set( i, j, mat.Get( i, j ) * scalar );
    return result;
}

template <typename T>
Matrix<T> operator*( const T& scalar, const IMatrix<T>& mat ) {
    return mat * scalar;
}

// Деление матрицы на скаляр
template <typename T>
Matrix<T> operator/( const IMatrix<T>& mat, const T& scalar ) {
    if ( scalar == T(0) )
        throw std::domain_error( "operator/: division by zero" );
    Matrix<T> result( mat.GetRows(), mat.GetCols() );
    for ( size_t i = 0; i < mat.GetRows(); ++i )
        for ( size_t j = 0; j < mat.GetCols(); ++j )
            result.Set( i, j, mat.Get( i, j ) / scalar );
    return result;
}

// Унарный минус
template <typename T>
Matrix<T> operator-( const IMatrix<T>& mat ) {
    Matrix<T> result( mat.GetRows(), mat.GetCols() );
    for ( size_t i = 0; i < mat.GetRows(); ++i )
        for ( size_t j = 0; j < mat.GetCols(); ++j )
            result.Set( i, j, -mat.Get( i, j ) );
    return result;
}

// Сравнение с учётом эпсилон для плавающих типов
template <typename T>
bool operator==( const IMatrix<T>& a, const IMatrix<T>& b ) {
    if ( a.GetRows() != b.GetRows() || a.GetCols() != b.GetCols() )
        return false;
    for ( size_t i = 0; i < a.GetRows(); ++i )
        for ( size_t j = 0; j < a.GetCols(); ++j ) {
            if constexpr ( std::is_floating_point_v<T> ) {
                using std::abs;
                if ( abs( a.Get( i, j ) - b.Get( i, j ) ) > std::numeric_limits<T>::epsilon() )
                    return false;
            } else {
                if ( a.Get( i, j ) != b.Get( i, j ) )
                    return false;
            }
        }
    return true;
}

template <typename T>
bool operator!=( const IMatrix<T>& a, const IMatrix<T>& b ) {
    return !(a == b);
}