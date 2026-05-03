#pragma once

#include "ICollection.h"
#include "IEnumerable.h"
#include <stdexcept>
#include <cstddef>
#include <string>

class IndexOutOfRange : public std::out_of_range {
public:
    explicit IndexOutOfRange( const std::string& message )
        : std::out_of_range( message ) { }
};

template <class T>
class DynamicArray : public ICollection<T>, public IEnumerable<T> {
public:
    // Встроенный итератор
    class ArrayIterator : public IEnumerator<T> {
    private:
        DynamicArray<T>& arr_;
        int index_;
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
        void Reset() override { index_ = -1; }
    };

    IEnumerator<T>* GetEnumerator() override {
        return new ArrayIterator( *this );
    }

    // Конструкторы
    DynamicArray() : data_( nullptr ), size_( 0 ), capacity_( 0 ) {}

    DynamicArray( T* items, int count ) : data_( nullptr ), size_( count ) {
        if ( count < 0 ) throw std::invalid_argument( "DynamicArray: count can't be negative" );
        if ( count > 0 && items == nullptr ) throw std::invalid_argument( "DynamicArray: items is nullptr" );
        capacity_ = ( count == 0 ) ? 1 : count;
        data_ = new T[capacity_];
        for ( int idx = 0; idx < size_; ++idx ) data_[idx] = items[idx];
    }

    explicit DynamicArray( int size ) : data_( nullptr ), size_( size ) {
        if ( size < 0 ) throw std::invalid_argument( "DynamicArray: size can't be negative" );
        capacity_ = ( size == 0 ) ? 1 : size;
        data_ = new T[capacity_]();
    }

    DynamicArray( const DynamicArray<T>& other )
        : data_( nullptr ), size_( other.size_ ), capacity_( other.capacity_ ) {
        if ( capacity_ > 0 ) {
            data_ = new T[capacity_];
            for ( int idx = 0; idx < size_; ++idx ) data_[idx] = other.data_[idx];
        }
    }

    DynamicArray<T>& operator=( const DynamicArray<T>& other ) {
        if ( this != &other ) {
            T* new_data = nullptr;
            int new_capacity = other.capacity_;
            if ( new_capacity > 0 ) {
                new_data = new T[new_capacity];
                for ( int idx = 0; idx < other.size_; ++idx ) new_data[idx] = other.data_[idx];
            }
            delete[] data_;
            data_ = new_data;
            size_ = other.size_;
            capacity_ = new_capacity;
        }
        return *this;
    }

    ~DynamicArray() override { delete[] data_; }

    // Доступ
    T& Get( std::size_t index ) override {
        CheckIndex( static_cast<int>( index ) );
        return data_[index];
    }
    const T& Get( std::size_t index ) const override {
        CheckIndex( static_cast<int>( index ) );
        return data_[index];
    }
    std::size_t GetCount() const override { return static_cast<std::size_t>( size_ ); }

    void Set( int index, const T& value ) {
        CheckIndex( index );
        data_[index] = value;
    }

    // Управление размером
    void Resize( int newSize ) {
        if ( newSize < 0 ) throw std::invalid_argument( "DynamicArray: new_size can't be negative" );
        if ( newSize <= capacity_ ) {
            size_ = newSize;
            return;
        }
        // Если capacity_ == 0, начинаем с 1
        int newCapacity = ( capacity_ == 0 ) ? 1 : capacity_;
        while ( newCapacity < newSize ) {
            newCapacity *= 2;
        }
        T* newData = new T[newCapacity];
        for ( int idx = 0; idx < size_; ++idx ) {
            newData[idx] = data_[idx];
        }
        delete[] data_;
        data_ = newData;
        capacity_ = newCapacity;
        size_ = newSize;
    }

    void Append( const T& value ) {
        int oldSize = size_;
        Resize( oldSize + 1 );
        data_[oldSize] = value;
    }

    int GetCapacity() const { return capacity_; }

    T& operator[]( int index ) {
        CheckIndex( index );
        return data_[index];
    }
    const T& operator[]( int index ) const {
        CheckIndex( index );
        return data_[index];
    }

private:
    T*  data_;
    int size_;
    int capacity_;

    void CheckIndex( int index ) const {
        if ( index < 0 || index >= size_ )
            throw IndexOutOfRange( "DynamicArray: index out of range" );
    }
};