#pragma once

#include "Matrix.h"
#include "TriangularMatrix.h"
#include <stdexcept>
#include <cmath>

template <typename T>
class SquareMatrix : public Matrix<T> {
public:
    // Конструкторы
    SquareMatrix( size_t size ) : Matrix<T>( size ) { }

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

    // Статический метод: приводит квадратную матрицу к верхней треугольной форме
    // Возвращает пару (треугольная матрица, знак перестановок строк/столбцов)
    static std::pair<TriangularMatrix<T>, int> FactorizeToUpper( const SquareMatrix<T>& mat ) {
        size_t length = mat.GetRows();

        SquareMatrix<T> temp = mat;

        int sign = 1;
        T zero = T(0);

        // Основной цикл по столбцам (и по строкам)
        for ( size_t k = 0; k < length; ++k ) {
            // Поиск ведущего элемента
            size_t pivot = k;
            T pivot_val = temp.Get( k, k );
            for ( size_t i = k + 1; i < length; ++i ) {
                // if ( std::abs( temp.Get( i, k ) ) > std::abs( pivot_val ) ) {
                using std::abs; // разрешаем компилятору использовать std::abs для int/double
                if ( abs( temp.Get( i, k) ) > abs( pivot_val ) ) { // а для Complex он сам найдет внешнюю функцию abs
                    pivot_val = temp.Get( i, k );
                    pivot = i;
                }
            }
            // Если ведущий эл-нт(больший по модулю) равен нулю, то матрица вырождена
            if ( pivot_val == zero ) continue;

            // Если ведущая строка не текущая k, меняем их местами
            if ( pivot != k ) {
                for ( size_t j = 0; j < length; ++j ) {
                    T swap_temp = temp.Get( k, j );
                    temp.Set( k, j, temp.Get( pivot, j ) );
                    temp.Set( pivot, j, swap_temp );
                }
                sign = - sign;
            }

            // Обнуление элементов ниже диагонали в столбце k
            for ( size_t i = k + 1; i < length; ++i ) {
                // Множитель: на сколько нужно умножить ведущую строку, чтобы вычесть её из i-ой строки
                T factor = temp.Get( i, k ) / pivot_val;
                for ( size_t j = k; j < length; ++j ) {
                    temp.Set( i, j, temp.Get( i, j ) - factor * temp.Get( k, j ) );
                }
            }
        }

        // Копирование элементов из временной SquareMatrix в итоговую TriangularMatrix.
        // Только когда все элементы под диагональю гарантированно равны нулю,
        // мы можем безопасно перенести данные в треугольный формат.
        TriangularMatrix<T> result( length, TriangularType::Upper );
        for ( size_t i = 0; i < length; ++i ) {
            for ( size_t j = i; j < length; ++j ) {
                result.Set( i, j, temp.Get( i, j ) );
            }
        }

        return { std::move( result ), sign };
    }

    T Determinant() const {
        auto [tri, sign] = FactorizeToUpper( *this );
        return tri.Determinant() * static_cast<T>( sign );
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
