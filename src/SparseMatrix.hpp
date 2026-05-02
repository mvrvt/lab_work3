#pragma once

#include "IMatrix.hpp"
#include "lab2_files/DynamicArray.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>

template <typename T>
class SparseMatrix : public IMatrix<T> {
public:
    SparseMatrix( size_t rows, size_t cols )
        : rows_( rows ), cols_( cols ), elements_() { }

    ~SparseMatrix() override = default;

    size_t GetRows() const override { return rows_; }
    size_t GetCols() const override { return cols_; }

    const T& Get( size_t i, size_t j ) const override {
        if ( i >= rows_ || j >= cols_ )
            throw std::out_of_range( "SparseMatrix::Get: index out of range" );

        SearchResult result = BinarySearch( i, j );
        if ( result.found )
            return elements_.Get( result.index ).value;
        return zero_val;
    }

    void Set( size_t i, size_t j, const T& value ) override {
        if ( i >= rows_ || j >= cols_ )
            throw std::out_of_range( "SparseMatrix::Set: index out of range" );

        SearchResult result = BinarySearch( i, j );

        if ( result.found ) {
            if ( value == zero_val ) {
                // Удаление: сдвиг элементов влево
                for ( size_t k = result.index; k < elements_.GetCount() - 1; ++k )
                    elements_[k] = elements_[k + 1];
                elements_.Resize( elements_.GetCount() - 1 );
            } else {
                // Обновление существующего элемента
                elements_[result.index].value = value;
            }
        } else {
            if ( value != zero_val ) {
                // Вставка нового элемента
                if ( elements_.GetCount() == rows_ * cols_ )
                    throw std::length_error( "SparseMatrix::Set: too many non-zero elements" );

                // Увеличиваем размер на 1
                size_t oldCount = elements_.GetCount();
                elements_.Resize( oldCount + 1 );

                // Сдвигаем элементы вправо, начиная с конца
                for ( size_t k = oldCount; k > result.index; --k )
                    elements_[k] = elements_[k - 1];

                // Вставляем новый элемент
                elements_[result.index] = { i, j, value };
            }
        }
    }

    // Операторы модификации
    IMatrix<T>& operator+=( const IMatrix<T>& other ) override {
        if ( rows_ != other.GetRows() || cols_ != other.GetCols() )
            throw std::invalid_argument( "SparseMatrix::+=: size mismatch" );

        const SparseMatrix<T>* sparse_other = dynamic_cast<const SparseMatrix<T>*>( &other );

        if ( sparse_other ) {
            // Проходим по всем ненулевым элементам другой матрицы
            for ( size_t k = 0; k < sparse_other->elements_.GetCount(); ++k ) {
                const auto& el = sparse_other->elements_.Get( k );
                T newVal = this->Get( el.row, el.col ) + el.value;
                this->Set( el.row, el.col, newVal );
            }
        } else {
            // Плотная другая матрица – проходим по всем ячейкам
            for ( size_t i = 0; i < rows_; ++i ) {
                for ( size_t j = 0; j < cols_; ++j ) {
                    T otherVal = other.Get( i, j );
                    if ( otherVal != zero_val ) {
                        T newVal = this->Get( i, j ) + otherVal;
                        this->Set( i, j, newVal );
                    }
                }
            }
        }
        return *this;
    }

    IMatrix<T>& operator-=( const IMatrix<T>& other ) override {
        if ( rows_ != other.GetRows() || cols_ != other.GetCols() )
            throw std::invalid_argument( "SparseMatrix::-=: size mismatch" );

        const SparseMatrix<T>* sparse_other = dynamic_cast<const SparseMatrix<T>*>( &other );

        if ( sparse_other ) {
            for ( size_t k = 0; k < sparse_other->elements_.GetCount(); ++k ) {
                const auto& el = sparse_other->elements_.Get( k );
                T newVal = this->Get( el.row, el.col ) - el.value;
                this->Set( el.row, el.col, newVal );
            }
        } else {
            for ( size_t i = 0; i < rows_; ++i ) {
                for ( size_t j = 0; j < cols_; ++j ) {
                    T otherVal = other.Get( i, j );
                    if ( otherVal != zero_val ) {
                        T newVal = this->Get( i, j ) - otherVal;
                        this->Set( i, j, newVal );
                    }
                }
            }
        }
        return *this;
    }

    IMatrix<T>& operator*=( const T& scalar ) override {
        if ( scalar == zero_val ) {
            // Обнуляем всю матрицу — удаляем все элементы
            elements_.Resize(0);
            return *this;
        }
        for ( size_t k = 0; k < elements_.GetCount(); ++k )
            elements_[k].value *= scalar;
        return *this;
    }

    IMatrix<T>& operator/=( const T& scalar ) override {
        if ( scalar == zero_val )
            throw std::domain_error( "SparseMatrix::/=: division by zero" );
        for ( size_t k = 0; k < elements_.GetCount(); ++k )
            elements_[k].value /= scalar;
        return *this;
    }

    T Norm() const override {
        double sum = 0.0;
        for ( size_t k = 0; k < elements_.GetCount(); ++k ) {
            using std::abs;
            double mod = static_cast<double>( abs( elements_.Get( k ).value ) );
            sum += mod * mod;
        }
        return static_cast<T>( std::sqrt( sum ) );
    }

private:
    struct Element {
        size_t row;
        size_t col;
        T      value;
    };

    struct SearchResult {
        size_t index;
        bool   found;
    };

    DynamicArray<Element> elements_;
    size_t rows_;
    size_t cols_;

    inline static const T zero_val { };

    // Бинарный поиск элемента по (row, col) в отсортированном списке
    SearchResult BinarySearch( size_t r, size_t c ) const {
        size_t left = 0;
        size_t right = elements_.GetCount();
        while ( left < right ) {
            size_t mid = left + (right - left) / 2;
            const Element& el = elements_.Get( mid );
            if ( el.row == r && el.col == c )
                return { mid, true };
            if ( el.row < r || (el.row == r && el.col < c) )
                left = mid + 1;
            else
                right = mid;
        }
        return { left, false };
    }
};
