#pragma once

#include "IMatrix.hpp"
#include "lab2_files/DynamicArray.h"
#include <stdexcept>
#include <cmath>

template <typename T>
class BandMatrix : public IMatrix<T> {
public:
    // Конструктор: size – размер квадратной матрицы, offsets – список смещений диагоналей (например, {-1,0,1})
    BandMatrix( size_t size, const DynamicArray<int>& offsets )
        : size_( size ), offsets_( offsets ), data_() {
        if ( size_ == 0 ) throw std::invalid_argument( "BandMatrix: size must be positive" );
        if ( offsets_.GetCount() == 0 ) throw std::invalid_argument( "BandMatrix: at least one offset required" );
        // Проверка допустимости смещений и сортировка (считаем, что входной массив уже отсортирован)
        for ( size_t idx = 0; idx < offsets_.GetCount(); ++idx ) {
            int d = offsets_.Get( idx );
            if ( std::abs( d ) >= static_cast<int>( size_ ) )
                throw std::invalid_argument( "BandMatrix: offset out of range for given size" );
            // Для каждого смещения создаём массив длины size - |d|
            DynamicArray<T> arr( static_cast<int>( size_ - std::abs( d ) ) );
            data_.Append( arr );  // сохраняем в параллельный список
        }
    }

    // Конструктор с initializer_list для удобства (можно оставить как вспомогательный)
    BandMatrix( size_t size, std::initializer_list<int> offsets )
        : BandMatrix( size, DynamicArray<int>( const_cast<int*>(offsets.begin()), static_cast<int>(offsets.size()) ) ) {}

    // Копирующий конструктор
    BandMatrix( const BandMatrix& other )
        : size_( other.size_ ), offsets_( other.offsets_ ), data_() {
        for ( size_t i = 0; i < other.data_.GetCount(); ++i )
            data_.Append( other.data_.Get( i ) );
    }

    // Оператор присваивания
    BandMatrix& operator=( const BandMatrix& other ) {
        if ( this != &other ) {
            size_ = other.size_;
            offsets_ = other.offsets_;
            data_.Resize( 0 );
            for ( size_t i = 0; i < other.data_.GetCount(); ++i )
                data_.Append( other.data_.Get( i ) );
        }
        return *this;
    }

    ~BandMatrix() override = default;

    // Базовые свойства
    size_t GetRows() const override { return size_; }
    size_t GetCols() const override { return size_; }

    // Доступ к элементу
    const T& Get( size_t i, size_t j ) const override {
        if ( i >= size_ || j >= size_ )
            throw std::out_of_range( "BandMatrix::Get: index out of range" );
        int d = static_cast<int>( j ) - static_cast<int>( i );
        size_t idx = FindOffsetIndex( d );
        if ( idx < offsets_.GetCount() && offsets_.Get( idx ) == d ) {
            // Вычисляем позицию в массиве данных
            size_t pos = ( d >= 0 ) ? i : i + d;  // для отрицательных d: i + d (отрицательное число)
            return data_.Get( idx ).Get( pos );
        }
        return zero_val_;
    }

    void Set( size_t i, size_t j, const T& value ) override {
        if ( i >= size_ || j >= size_ )
            throw std::out_of_range( "BandMatrix::Set: index out of range" );
        int d = static_cast<int>( j ) - static_cast<int>( i );
        size_t idx = FindOffsetIndex( d );
        if ( idx < offsets_.GetCount() && offsets_.Get( idx ) == d ) {
            size_t pos = ( d >= 0 ) ? i : i + d;
            data_.Get( idx ).Set( pos, value );
        } else if ( value != zero_val_ ) {
            throw std::invalid_argument( "BandMatrix::Set: cannot set non-zero outside allowed diagonals" );
        }
    }

    // Операторы модификации
    IMatrix<T>& operator+=( const IMatrix<T>& other ) override {
        if ( size_ != other.GetRows() || size_ != other.GetCols() )
            throw std::invalid_argument( "BandMatrix::+=: size mismatch" );

        const BandMatrix<T>* band_other = dynamic_cast<const BandMatrix<T>*>( &other );
        if ( band_other && OffsetsEqual( band_other->offsets_ ) ) {
            // Поэлементное сложение для каждой диагонали
            for ( size_t idx = 0; idx < data_.GetCount(); ++idx ) {
                DynamicArray<T>& this_arr = data_.Get( idx );
                const DynamicArray<T>& other_arr = band_other->data_.Get( idx );
                size_t len = this_arr.GetCount();
                for ( size_t k = 0; k < len; ++k )
                    this_arr.Set( k, this_arr.Get( k ) + other_arr.Get( k ) );
            }
        } else {
            // Проверяем, что other не имеет ненулевых элементов вне разрешённых диагоналей
            for ( size_t i = 0; i < size_; ++i ) {
                for ( size_t j = 0; j < size_; ++j ) {
                    int d = static_cast<int>( j ) - static_cast<int>( i );
                    if ( FindOffsetIndex( d ) >= offsets_.GetCount() && other.Get( i, j ) != zero_val_ )
                        throw std::invalid_argument( "BandMatrix::+=: other matrix has non-zero outside allowed diagonals" );
                }
            }
            // Сложение
            for ( size_t idx = 0; idx < data_.GetCount(); ++idx ) {
                int d = offsets_.Get( idx );
                DynamicArray<T>& this_arr = data_.Get( idx );
                size_t len = this_arr.GetCount();
                if ( d >= 0 ) {
                    for ( size_t i = 0; i < len; ++i )
                        this_arr.Set( i, this_arr.Get( i ) + other.Get( i, i + d ) );
                } else {
                    for ( size_t i = -d; i < size_; ++i )
                        this_arr.Set( i + d, this_arr.Get( i + d ) + other.Get( i, i + d ) );
                }
            }
        }
        return *this;
    }

    IMatrix<T>& operator-=( const IMatrix<T>& other ) override {
        // Аналогично +=, заменив сложение на вычитание
        if ( size_ != other.GetRows() || size_ != other.GetCols() )
            throw std::invalid_argument( "BandMatrix::-=: size mismatch" );

        const BandMatrix<T>* band_other = dynamic_cast<const BandMatrix<T>*>( &other );
        if ( band_other && OffsetsEqual( band_other->offsets_ ) ) {
            for ( size_t idx = 0; idx < data_.GetCount(); ++idx ) {
                DynamicArray<T>& this_arr = data_.Get( idx );
                const DynamicArray<T>& other_arr = band_other->data_.Get( idx );
                size_t len = this_arr.GetCount();
                for ( size_t k = 0; k < len; ++k )
                    this_arr.Set( k, this_arr.Get( k ) - other_arr.Get( k ) );
            }
        } else {
            for ( size_t i = 0; i < size_; ++i ) {
                for ( size_t j = 0; j < size_; ++j ) {
                    int d = static_cast<int>( j ) - static_cast<int>( i );
                    if ( FindOffsetIndex( d ) >= offsets_.GetCount() && other.Get( i, j ) != zero_val_ )
                        throw std::invalid_argument( "BandMatrix::-=: other matrix has non-zero outside allowed diagonals" );
                }
            }
            for ( size_t idx = 0; idx < data_.GetCount(); ++idx ) {
                int d = offsets_.Get( idx );
                DynamicArray<T>& this_arr = data_.Get( idx );
                size_t len = this_arr.GetCount();
                if ( d >= 0 ) {
                    for ( size_t i = 0; i < len; ++i )
                        this_arr.Set( i, this_arr.Get( i ) - other.Get( i, i + d ) );
                } else {
                    for ( size_t i = -d; i < size_; ++i )
                        this_arr.Set( i + d, this_arr.Get( i + d ) - other.Get( i, i + d ) );
                }
            }
        }
        return *this;
    }

    IMatrix<T>& operator*=( const T& scalar ) override {
        for ( size_t idx = 0; idx < data_.GetCount(); ++idx ) {
            DynamicArray<T>& arr = data_.Get( idx );
            for ( size_t k = 0; k < arr.GetCount(); ++k )
                arr.Set( k, arr.Get( k ) * scalar );
        }
        return *this;
    }

    IMatrix<T>& operator/=( const T& scalar ) override {
        if ( scalar == zero_val_ )
            throw std::domain_error( "BandMatrix::/=: division by zero" );
        for ( size_t idx = 0; idx < data_.GetCount(); ++idx ) {
            DynamicArray<T>& arr = data_.Get( idx );
            for ( size_t k = 0; k < arr.GetCount(); ++k )
                arr.Set( k, arr.Get( k ) / scalar );
        }
        return *this;
    }

    T Norm() const override {
        double sum = 0.0;
        for ( size_t idx = 0; idx < data_.GetCount(); ++idx ) {
            const DynamicArray<T>& arr = data_.Get( idx );
            for ( size_t k = 0; k < arr.GetCount(); ++k ) {
                using std::abs;
                double mod = static_cast<double>( abs( arr.Get( k ) ) );
                sum += mod * mod;
            }
        }
        return static_cast<T>( std::sqrt( sum ) );
    }

protected:
    size_t size_;
    DynamicArray<int> offsets_;          // отсортированные смещения
    DynamicArray<DynamicArray<T>> data_; // параллельный список массивов

    static inline const T zero_val_ = T(0);

    // Бинарный поиск индекса смещения в offsets_
    size_t FindOffsetIndex( int d ) const {
        size_t left = 0, right = offsets_.GetCount();
        while ( left < right ) {
            size_t mid = left + (right - left) / 2;
            int val = offsets_.Get( mid );
            if ( val == d ) return mid;
            if ( val < d ) left = mid + 1;
            else right = mid;
        }
        return left;  // индекс, где должно быть, но может не совпадать
    }

    bool OffsetsEqual( const DynamicArray<int>& other ) const {
        if ( offsets_.GetCount() != other.GetCount() ) return false;
        for ( size_t i = 0; i < offsets_.GetCount(); ++i )
            if ( offsets_.Get( i ) != other.Get( i ) ) return false;
        return true;
    }
};
