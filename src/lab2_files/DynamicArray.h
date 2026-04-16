#pragma once

#include <stdexcept> // для ошибки out_of_range
#include <cstddef>   // для типа элементов size_t
#include <string>
#include "ICollection.h"
#include "IEnumerable.h"

class IndexOutOfRange : public std::out_of_range {
public:
    explicit IndexOutOfRange( const std::string& message )
        : std::out_of_range( message ) { }
};

template <class T>
class DynamicArray : public ICollection<T>, public IEnumerable<T> {
private:
    T*  data_;
    int size_;

    void CheckIndex( int index ) const {
        if ( index < 0 || index >= size_ ) {
            throw IndexOutOfRange( "DynamicArray: index out of range" );
        }
    }

public:
    // Встроенный класс-итератор
    class ArrayIterator : public IEnumerator<T> {
    private:
        DynamicArray<T>& arr_; // ссылка на массив
        int index_;            // -1 - позиция до первого элемента

    public:
        explicit ArrayIterator( DynamicArray<T>& arr ) : arr_( arr ), index_( -1 ) { }

        bool MoveNext() override {
            ++index_;
            return index_ < static_cast<int>( arr_.GetCount() );
        }

        T& Current() override {
            if ( index_ < 0 || index_ >= static_cast<int>( arr_.GetCount() ) )
                throw IndexOutOfRange( "ArrayIterator: index out of range" );
            return arr_.Get( static_cast<std::size_t>( index_ ) );
        }

        void Reset() override {
            index_ = -1;
        }
    };

    //======================================================

    // Фабричный метод — создаёт итератор для этого массива
    IEnumerator<T>* GetEnumerator() override {
        return new ArrayIterator( *this );
    }

    DynamicArray( T* items, int count ) : data_( nullptr ), size_( count ) {
        if ( count < 0 ) {
            throw std::invalid_argument( "DynamicArray: count can't be negative" );
        }
        if ( count > 0 && items == nullptr ) {
            throw std::invalid_argument( "DynamicArray: items is nullptr" );
        }

        if ( size_ > 0 ) {
            data_ = new T[size_];
            for ( int idx = 0; idx < size_; ++idx ) {
                data_[idx] = items[idx];
            }
        }
    }

    explicit DynamicArray( int size ) : data_( nullptr ), size_( size ) {
        if ( size < 0 ) {
            throw std::invalid_argument( "DynamicArray: size can't be negative" );
        }

        if ( size_ > 0 ) {
            data_ = new T[size_];
        }
    }

    DynamicArray( const DynamicArray<T>& other ) : data_( nullptr ), size_( other.size_ ) {
        if ( size_ > 0 ) {
            data_ = new T[size_];
            for ( int idx = 0; idx < size_; ++idx ) {
                data_[idx] = other.data_[idx];
            }
        }
    }

    DynamicArray<T>& operator=( const DynamicArray <T>& other ) {
        if ( this != &other ) {
            T* new_data = nullptr;

            if ( other.size_  > 0 ) {
                new_data = new T[other.size_];
                for ( int idx = 0; idx < other.size_; ++idx ) {
                    new_data[idx] = other.data_[idx];
                }
            }
            delete[] data_;
            data_ = new_data;
            size_ = other.size_;
        }
        return *this;
    }

    ~DynamicArray() override {
        delete[] data_;
    }

    T&       Get( std::size_t index )       override { CheckIndex( index ); return data_[index]; }
    const T& Get( std::size_t index ) const override { CheckIndex( index ); return data_[index]; }

    std::size_t GetCount() const override {
        return size_;
    }

    void Set( int index, const T& value ) {
        CheckIndex( index );
        data_[index] = value;
    }

    void Resize( int new_size ) {
        if ( new_size < 0 ) {
            throw std::invalid_argument( "DynamicArray: new_size can't be negative" );
        }

        T* new_data = nullptr;

        if ( new_size > 0 ) {
            new_data = new T[new_size];
            int copy_count = ( new_size < size_ ) ? new_size : size_;

            for ( int idx = 0; idx < copy_count; ++idx ) {
                new_data[idx] = data_[idx];
            }
        }

        delete[] data_;
        data_ = new_data;
        size_ = new_size;
    }

    T& operator[]( int index ) {
        CheckIndex( index );
        return data_[index];
    }

    const T& operator[]( int index ) const {
        CheckIndex( index );
        return data_[index];
    }
};
