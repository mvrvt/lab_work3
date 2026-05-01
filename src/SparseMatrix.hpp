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
        : elements_( 0 ), count_( 0 ), capacity_( 0 ), rows_( rows ), cols_( cols ) { }

    ~SparseMatrix() override  = default;

    size_t GetRows() const override { return rows_; }
    size_t GetCols() const override { return cols_; }

    const T& Get( size_t i, size_t j ) const override {
        if ( i >= rows_ || j >= cols_ )
            throw std::out_of_range( "SparseMatrix::Get: index out of range" );

        SearchResult result = BinarySearch( i, j );

        if ( result.found ) {
            return elements_.Get( result.index ).value;
        }

        return zero_val;
    }

     void Set( size_t i, size_t j, const T& value ) override {
        if ( i >= rows_ || j >= cols_ )
            throw std::out_of_range( "SparseMatrix::Set: index out of range" );

        SearchResult result = BinarySearch( i, j );

        if ( result.found ) {
            if ( value == zero_val ) {
                // Удаление: result.idx как точка начала
                for ( size_t k = result.index; k < count_ - 1; ++k ) {
                    elements_[k] = elements_[k + 1];
                }
                --count_;
            } else {
                // Обновление значения по найденному индексу
                elements_[result.index].value = value;
            }
        } else {
            if ( value != zero_val ) {
                // Вставка: result.index указывает на место, куда нужно вставить эл-нт
                if ( count_ == capacity_ ) {
                    size_t new_capacity = ( capacity_ == 0 ) ? 2 : capacity_ * 2;
                    capacity_ = std::min( new_capacity, rows_ * cols_ );
                    elements_.Resize( static_cast<int>( capacity_ ) );
                }

                for ( int k = static_cast<int>( count_ ); k > result.index; --k ) {
                    elements_[k] = elements_[k - 1];
                }

                elements_[result.index] = { i, j, value };
                ++count_;
            }
        }
    }

    IMatrix<T>& operator+=( const IMatrix<T>& other ) override {
        if ( rows_ != other.GetRows() || cols_ != other.GetCols() )
            throw std::invalid_argument( "SparseMatrix::+=: size mismatch" );

        const SparseMatrix<T>* sparse_other = dynamic_cast<const SparseMatrix<T>*>( &other );

        if ( sparse_other ) {
            // Если другая матрица тоже разреженная, то пробегаемся только по её ненулевым эл-там
            for ( size_t k = 0; k < sparse_other->count_; ++k ) {
                const auto& el = sparse_other->elements_[k];
                this->Set( el.row, el.col, this->Get( el.row, el.col ) + el.value );
            }
        } else {
            for ( size_t i = 0; i < rows_; ++i ) {
                for ( size_t j = 0; j < cols_; ++j ) {
                    T other_val = other.Get( i, j );
                    if ( other_val != zero_val ) {
                        this->Set( i, j, this->Get( i, j ) + other_val );
                    }
                }
            }
        }
        return *this;
    }

    IMatrix<T>& operator-=( const IMatrix<T>& other ) override {
        if ( rows_ != other.GetRows() || cols_ != other.GetCols() )
            throw std::invalid_argument( "SparseMatrix::-=: size mismatch" );
        // Аналогично operator+=, но со знаком минус
        const SparseMatrix<T>* sparse_other = dynamic_cast<const SparseMatrix<T>*>( &other );
        if ( sparse_other ) {
            for ( size_t k = 0; k < sparse_other->count_; ++k ) {
                const auto& el = sparse_other->elements_[k];
                Set( el.row, el.col, Get( el.row, el.col ) - el.value );
            }
        } else {
            for ( size_t i = 0; i < rows_; ++i )
                for ( size_t j = 0; j < cols_; ++j ) {
                    T val = other.Get( i, j );
                    if ( val != zero_val )
                        Set( i, j, Get( i, j ) - val );
                }
        }
        return *this;
    }

    IMatrix<T>& operator*=( const T& scalar ) override {
        if ( scalar == zero_val ) {
            count_ = 0;
            return *this;
        }
        for ( size_t k = 0; k < count_; ++k ) {
            elements_[k].value *= scalar;
        }
        return *this;
    }

    IMatrix<T>& operator/=( const T& scalar ) override {
        if ( scalar == zero_val )
            throw std::domain_error( "SparseMatrix::/=: division by zero" );
        for ( size_t k = 0; k < count_; ++k )
            elements_[k].value /= scalar;
        return *this;
    }

    T Norm() const override {
        double sum = 0.0;
        for ( size_t k = 0; k < count_; ++k ) {
            using std::abs;
            double mod = static_cast<double>( abs( elements_[k].value ) );
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

    // Собственная структура вместо std::pair
    struct SearchResult {
        int  index;
        bool found;
    };

    DynamicArray<Element> elements_;
    size_t                count_;
    size_t                capacity_;
    size_t                rows_;
    size_t                cols_;

    inline static const T zero_val { };

    SearchResult BinarySearch( size_t r, size_t c ) const {
        int left = 0;
        int right = static_cast<int>( count_ ) - 1;
        int mid = 0;

        while ( left <= right ) {
            mid = left + ( right - left) / 2;
            const auto& el = elements_[mid];

            if ( el.row == r && el.col == c )
                return { mid, true }; // Возвращается объект структуры

            // Если текущий эл-нт меньше искомого (сначала по row, потом по col)
            if ( el.row < r || ( el.row == r && el.col < c ) ) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return { left, false };
    }
};
