#pragma once

#include "IMatrix.hpp"
#include "lab2_files/DynamicArray.h"
#include <stdexcept>
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
                    capacity_ = ( capacity_ == 0 ) ? 2 : capacity_ * 2;
                    elements_.Resize( static_cast<int>( capacity_ ) );
                }

                for ( int k = static_cast<int>( count_ ); k > result.index; --k ) {
                    elements_[k] = elements_[k - 1];
                }

                elements_[result.index] = { i, j, value };
                ++count_;
            }
        }

        // auto [idx, found] = BinarySearch( i, j );
        //
        // if ( found ) {
        //     if ( value == zero_val ) {
        //         // Удаление элемента (сдвиг влево)
        //         for ( int k = idx; k < count_ - 1; ++k ) {
        //             elements_[k] = elements_[k + 1];
        //         }
        //         --count_;
        //     } else {
        //         // Обновление значения
        //         elements_[idx].value = value;
        //     }
        // } else { // Эл-нт не найден
        //     if ( value != zero_val ) {
        //         // Вставка нового элемента
        //         if ( count_ == capacity_ ) {
        //             capacity_ = ( capacity_ == 0 ) ? 2 : capacity_ * 2;
        //             elements_.Resize( static_cast<int>( capacity_ ) );
        //         }
        //
        //         // Сдвиг вправо для освобождения места
        //         for ( int k = static_cast<int>( count_ ); k > idx; --k ) {
        //             elements_[k] = elements_[k - 1];
        //         }
        //
        //         elements_[idx] = { i, j, value };
        //         ++count_;
        //     }
        // }
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

    T Norm() const override {
        double sum = 0.0;
        for ( size_t k = 0; k < count_; ++k ) {
            using std::abs;
            double mod = static_cast<double>( abs( elements_[k].value ) );
            sum += mod * mod;
        }
        return static_cast<T>( std::sqrt( sum ) );
    }

    void Print() const override;

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
    size_t count_;
    size_t capacity_;
    size_t rows_;
    size_t cols_;

    inline static const T zero_val {};

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





    // .first = индекс (где элемент находится или куда его нужно вставить)
    // .second = true, если элемент найден, false - если нет

    // std::pair<int, bool> BinarySearch( size_t r, size_t c ) const {
    //     int left = 0;
    //     int right = static_cast<int>( count_ ) - 1;
    //
    //     while ( left <= right ) {
    //         int mid = left + ( right - left ) / 2;
    //         const auto& el = elements_.Get( mid );
    //
    //         if ( el.row == r && el.col == c )
    //             return { mid, true }; // поиск завершён, элемент найден
    //
    //         // Если текущий эл-нт меньше искомого (сначала по row, потом по col)
    //         if ( el.row < r || ( el.row == r && el.col <c ) ) {
    //             left = mid + 1;
    //         } else {
    //             right = mid - 1;
    //         }
    //     }
    //     return { left, false }; // эл-нт был не найден, возвращается индекс для вставки
    //}
};
















































// #pragma once
//
// #include "IMatrix.h"
// #include "lab2_files/DynamicArray.h"
// #include <stdexcept>
// #include <cstddef>
//
// template <typename T>
// class SparseMatrix : public IMatrix<T> {
// private:
//     struct Element {
//         size_t row;
//         size_t col;
//         T value;
//     };
//
//     DynamicArray<Element> elements_;
//     size_t rows_;
//     size_t cols_;
//     T zero_;
//
//     // Вспомогательный метод для поиска индекса элемента в массиве
//     int FindElementIndex( size_t i, size_t j ) const {
//         for ( size_t k = 0; k < elements_.GetCount(); ++k ) {
//             if ( elements_.Get( k ).row == i && elements_.Get( k ).col == j ) {
//                 return static_cast<int>( k );
//             }
//         }
//         return -1;
//     }
//
// public:
//     SparseMatrix( size_t rows, size_t cols ) : elements_( 0 ), rows_( rows ), cols_( cols ), zero_( T(0) ) { }
//
//     size_t GetRows() const override { return rows_; }
//     size_t GetCols() const override { return cols_; }
//
//     const T& Get( size_t i, size_t j ) const override {
//         if ( i >= rows_ || j >= cols_ )
//             throw std::out_of_range( "SparseMatrix::Get: index out of range" );
//
//         int idx = FindElementIndex( i, j );
//         if ( idx != -1 )
//             return elements_.Get( static_cast<size_t>( idx ) ).value;
//         return zero_;
//     }
//
//     void Set( size_t i, size_t j, const T& value ) override {
//         if ( i >= rows_ || j >= cols_ )
//             throw std::out_of_range( "SparseMatrix::Set: index out of range" );
//
//         int idx = FindElementIndex( i, j );
//         if ( value == zero_ ) {
//             // Если пытаемся записать ноль, и элемент уже существует - его надо удалить,
//             // поскольку DynamicArray не имеет метода Remove(), записываем его в ноль (zero_)
//             if ( idx != -1 ) elements_[idx].value = zero_;
//         } else {
//             if ( idx != -1 ) {
//                 elements_[idx].value = value; // обновляем существующий элемент
//             } else {
//                 // добавляем новый элемент (поскольку в DynamicArray нет Append, придется делать Resize + Set)
//                 size_t current_count = elements_.GetCount();
//                 elements_.Resize( static_cast<int>( current_count + 1 ) );
//                 elements_.Set( static_cast<int>( current_count ), { i, j, value } );
//             }
//         }
//     }
//
//     IMatrix<T>& operator+=( const IMatrix<T>& other ) override {
//         if ( rows_ != other.GetRows() || cols_ != other.GetCols() )
//             throw std::invalid_argument( "SparseMatrix::+=: size mismatch" );
//
//         for ( size_t i = 0; i < rows_; ++i ) {
//             for ( size_t j = 0; j < cols_; ++j ) {
//                 T other_val = other.Get( i, j );
//                 if ( other_val != zero_ ) {
//                     this->Set( i, j, this->Get( i, j ) + other_val );
//                 }
//             }
//         }
//         return *this;
//     }
//
//     IMatrix<T>& operator*=( const T& scalar ) override {
//         if ( scalar == zero_ ) {
//             elements_.Resize( 0 ); // если умножаем на 0 - матрица становится пустой
//             return *this;
//         }
//         for ( size_t k = 0; k < elements_.GetCount(); ++k ) {
//             elements_[k].value *= scalar;
//         }
//         return *this;
//     }
//
//     // T Norm() const override {
//     //     T sum = T(0);
//     //     for ( size_t k = 0; k < elements_.GetCount(); ++k ) {
//     //         sum += elements_.Get( k ).value * elements_.Get( k ).value;
//     //     }
//     //     return static_cast<T>( std::sqrt( static_cast<double>( sum ) ) );
//     // }
//     T Norm() const override {
//         double sum = 0.0;
//         for ( size_t k = 0; k < elements_.GetCount(); ++k ) {
//             using std::abs;
//             // Обращаемся к полю value конкретного элемента
//             double mod = static_cast<double>( abs( elements_.Get( k ).value ) );
//             sum += mod * mod;
//         }
//         return static_cast<T>( std::sqrt( sum ) );
//     }
//
//     void Print() const override;
//
//     ~SparseMatrix() override = default;
// };
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
