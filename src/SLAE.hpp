# pragma once

#include "SquareMatrix.hpp"
#include "TriangularMatrix.hpp"
#include "Vector.hpp"
#include <cmath>
#include <stdexcept>
#include <utility>

namespace LinAlg {
    // Вспомогательная функция: Обратная подстановка (для верхней треугольной матрицы)
    template <typename T>
    Vector<T> BackwardSubstitution( const IMatrix<T>& U, const Vector<T>& b ) {
        size_t n = b.GetSize();
        Vector<T> x( n );
        for ( int i = n - 1; i >= 0; --i ) {
            T sum = T( 0 );
            for ( size_t j = i + 1; j < n; ++j ) {
                sum += U.Get( i, j ) * x[j];
            }
            if ( U.Get( i, i ) == T( 0 ) ) throw std::logic_error( "BackwardSubstitution: zero on diagonal" );
            x[i] = ( b[i] - sum ) / U.Get( i, i );
        }
        return x;
    }

    // Вспомогательная функция: Прямая подстановка (для нижней треугольной матрицы)
    template <typename T>
    Vector<T> ForwardSubstitution( const IMatrix<T>& L, const Vector<T>& b ) {
        size_t n = b.GetSize();
        Vector<T> x( n );
        for ( size_t i = 0; i < n; ++i ) {
            T sum = T( 0 );
            for ( size_t j = 0; j < i; ++j ) {
                sum += L.Get( i, j ) * x[j];
            }
            if ( L.Get( i, i ) == T( 0 ) ) throw std::logic_error( "ForwardSubstitution: zero on diagonal" );
            x[i] = ( b[i] - sum ) / L.Get( i, i );
        }
        return x;
    }

    // 1. Метод Гаусса (с выбором или без выбора ведущего элемента)
    template <typename T>
    Vector<T> SolveGauss( const SquareMatrix<T>& A, const Vector<T>& b, bool use_pivoting = true ) {
        size_t n = A.GetRows();
        SquareMatrix<T> mat = A;
        Vector<T> vec = b;

        for ( size_t k = 0; k < n; ++k ) {
            // Выбор ведущего элемента (pivot)
            if ( use_pivoting ) {
                size_t pivot_row = k;

                // Включаем ADL: компилятор найдет std::abs для double
                // и глобальный abs() для твоего кастомного Complex
                using std::abs;

                // Используем auto, потому что модуль комплексного числа - это double,
                // а не Complex. auto сам выведет правильный тип (double).
                auto max_val = abs( mat.Get( k, k ) );

                for ( size_t i = k + 1; i < n; ++i ) {
                    if ( abs( mat.Get( i, k ) ) > max_val ) {
                        max_val = abs( mat.Get( i, k ) );
                        pivot_row = i;
                    }
                }
                if ( pivot_row != k ) {
                    mat.SwapRows( k, pivot_row );
                    // Свапаем элементы в векторе
                    std::swap( vec[k], vec[pivot_row] );
                    // T temp = vec[k];
                    // vec[k] = vec[pivot_row];
                    // vec[pivot_row] = temp;
                }
            }
            if ( mat.Get( k , k) == T( 0 ) ) throw std::logic_error( "SolveGauss: singular matrix" );

            // Прямой ход
            for ( size_t i = k + 1; i < n; ++i ) {
                T factor = mat.Get( i, k ) / mat.Get( k, k );
                for ( size_t j = k; j < n; ++j )
                    mat.Set( i, j, mat.Get( i, j ) - factor * mat.Get( k, j ) );
                vec[i] -= factor * vec[k];
            }
        }
        return BackwardSubstitution( mat, vec );
    }

    // 2. LU-разложение (без перестановок, по методичке)
    template <typename T>
    std::pair<SquareMatrix<T>, SquareMatrix<T>> LUDecomposition(const SquareMatrix<T>& A) {
        size_t n = A.GetRows();
        SquareMatrix<T> L(n);
        SquareMatrix<T> U(n);

        for (size_t i = 0; i < n; ++i) {
            // Верхняя матрица U
            for (size_t k = i; k < n; ++k) {
                T sum = T(0);
                for (size_t j = 0; j < i; ++j) {
                    sum += (L.Get(i, j) * U.Get(j, k));
                }
                U.Set(i, k, A.Get(i, k) - sum);
            }

            // Нижняя матрица L
            for (size_t k = i; k < n; ++k) {
                if (i == k) {
                    L.Set(i, i, T(1)); // На диагонали L единицы
                } else {
                    T sum = T(0);
                    for (size_t j = 0; j < i; ++j) {
                        sum += (L.Get(k, j) * U.Get(j, i));
                    }
                    if (U.Get(i, i) == T(0)) throw std::logic_error("LUDecomposition: zero on diagonal (requires pivoting)");
                    L.Set(k, i, (A.Get(k, i) - sum) / U.Get(i, i));
                }
            }
        }
        return {L, U};
    }

    // Решение через LU
    template <typename T>
    Vector<T> SolveLU(const SquareMatrix<T>& L, const SquareMatrix<T>& U, const Vector<T>& b) {
        // Ly = b
        Vector<T> y = ForwardSubstitution(L, b);
        // Ux = y
        return BackwardSubstitution(U, y);
    }

    // 3. QR-разложение (Метод ортогонализации Грама-Шмидта)
    template <typename T>
    std::pair<SquareMatrix<T>, SquareMatrix<T>> QRDecomposition(const SquareMatrix<T>& A) {
        size_t n = A.GetRows();
        SquareMatrix<T> Q(n);
        SquareMatrix<T> R(n);

        for (size_t j = 0; j < n; ++j) {
            Vector<T> v(n);
            // Извлекаем j-й столбец
            for (size_t i = 0; i < n; ++i) v[i] = A.Get(i, j);

            for (size_t i = 0; i < j; ++i) {
                Vector<T> q_i(n);
                for (size_t k = 0; k < n; ++k) q_i[k] = Q.Get(k, i);

                T r_ij = q_i.Dot(v);
                R.Set(i, j, r_ij);

                for (size_t k = 0; k < n; ++k) {
                    v[k] -= r_ij * q_i[k];
                }
            }

            T norm = v.Norm();
            using std::abs; // Опять применяем магию ADL для поддержки и double, и Complex
            if (abs(norm) < 1e-9) throw std::logic_error("QRDecomposition: linearly dependent columns");
            R.Set(j, j, norm);

            for (size_t i = 0; i < n; ++i) {
                Q.Set(i, j, v[i] / norm);
            }
        }
        return {Q, R};
    }

}
