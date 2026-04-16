#pragma once

#include "IMatrix.h"
#include "lab2_files/DynamicArray.h"
#include <stdexcept>
#include <cstddef>

template <typename T>
class SparseMatrix : public IMatrix<T> {
private:
    struct Element {
        size_t row;
        size_t col;
        T value;
    };

    DynamicArray<Element> elements_;
    size_t rows_;
    size_t cols_;
    T zero_;

    // Вспомогательный метод для поиска индекса элемента в массиве
    int FindElementIndex( size_t i, size_t j ) const {
        for ( size_t k = 0; k < elements_.GetCount(); ++k ) {
            if ( elements_.Get( k ).row == i && elements_.Get( k ).col == j ) {
                return static_cast<int>( k );
            }
        }
        return -1;
    }

public:
    SparseMatrix( size_t rows, size_t cols ) : elements_( 0 ), rows_( rows ), cols_( cols ), zero_( T(0) ) { }

    size_t GetRows() const override { return rows_; }
    size_t GetCols() const override { return cols_; }

    const T& Get( size_t i, size_t j ) const override {
        if ( i >= rows_ || j >= cols_ )
            throw std::out_of_range( "SparseMatrix::Get: index out of range" );

        int idx = FindElementIndex( i, j );
        if ( idx != -1 )
            return elements_.Get( static_cast<size_t>( idx ) ).value;
        return zero_;
    }

    void Set( size_t i, size_t j, const T& value ) override {
        if ( i >= rows_ || j >= cols_ )
            throw std::out_of_range( "SparseMatrix::Set: index out of range" );

        int idx = FindElementIndex( i, j );
        if ( value == zero_ ) {
            // Если пытаемся записать ноль, и элемент уже существует - его надо удалить,
            // поскольку DynamicArray не имеет метода Remove(), записываем его в ноль (zero_)
            if ( idx != -1 ) elements_[idx].value = zero_;
        } else {
            if ( idx != -1 ) {
                elements_[idx].value = value; // обновляем существующий элемент
            } else {
                // добавляем новый элемент (поскольку в DynamicArray нет Append, придется делать Resize + Set)
                size_t current_count = elements_.GetCount();
                elements_.Resize( static_cast<int>( current_count + 1 ) );
                elements_.Set( static_cast<int>( current_count ), { i, j, value } );
            }
        }
    }

    IMatrix<T>& operator+=( const IMatrix<T>& other ) override {
        if ( rows_ != other.GetRows() || cols_ != other.GetCols() )
            throw std::invalid_argument( "SparseMatrix::+=: size mismatch" );

        for ( size_t i = 0; i < rows_; ++i ) {
            for ( size_t j = 0; j < cols_; ++j ) {
                T other_val = other.Get( i, j );
                if ( other_val != zero_ ) {
                    this->Set( i, j, this->Get( i, j ) + other_val );
                }
            }
        }
        return *this;
    }

    IMatrix<T>& operator*=( const T& scalar ) override {
        if ( scalar == zero_ ) {
            elements_.Resize( 0 ); // если умножаем на 0 - матрица становится пустой
            return *this;
        }
        for ( size_t k = 0; k < elements_.GetCount(); ++k ) {
            elements_[k].value *= scalar;
        }
        return *this;
    }

    // T Norm() const override {
    //     T sum = T(0);
    //     for ( size_t k = 0; k < elements_.GetCount(); ++k ) {
    //         sum += elements_.Get( k ).value * elements_.Get( k ).value;
    //     }
    //     return static_cast<T>( std::sqrt( static_cast<double>( sum ) ) );
    // }
    T Norm() const override {
        double sum = 0.0;
        for ( size_t k = 0; k < elements_.GetCount(); ++k ) {
            using std::abs;
            // Обращаемся к полю value конкретного элемента
            double mod = static_cast<double>( abs( elements_.Get( k ).value ) );
            sum += mod * mod;
        }
        return static_cast<T>( std::sqrt( sum ) );
    }

    void Print() const override;

    ~SparseMatrix() override = default;
};


































