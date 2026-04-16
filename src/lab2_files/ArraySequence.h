#pragma once

#include <cstddef> // для size_t
#include "Sequence.h"
#include "DynamicArray.h"
#include "SequenceIterator.h"

template <class T>
class ArraySequence : public Sequence<T> {
protected:
    DynamicArray<T>* items_;
    int count_;
    // благодаря count_ реализован паттерн, в котором у ArraySequence имеется и size и capacity
    // actual size ArraySequence - это ArraySequence::count_,
    // а actual capacity это DynamicArray::size_

    virtual ArraySequence<T>* Instance() = 0;

    ArraySequence<T>* AppendInternal( const T& item ) {
        if ( count_ >= static_cast<int>( items_->GetCount() ) ) {
            // не хватает места — увеличиваем capacity (вдвое)
            int new_cap = ( items_->GetCount() == 0 ) ? 1 : items_->GetCount() * 2;
            items_->Resize( new_cap );
        }
        items_->Set( count_, item );
        ++count_;
        return this;
    }

    ArraySequence<T>* PrependInternal( const T& item ) {
        if ( count_ >= static_cast<int>( items_->GetCount() ) ) {
            int new_cap = ( items_->GetCount() == 0 ) ? 1 : items_->GetCount() * 2;
            items_->Resize( new_cap );
        }
        for ( int idx = count_; idx > 0; --idx ) {
            items_->Set( idx, items_->Get( idx - 1 ) );
        }
        items_->Set( 0, item );
        ++count_;
        return this;
    }

    ArraySequence<T>* InsertAtInternal( const T& item, int index ) {
        if ( index < 0 || index > count_ ) {
            throw IndexOutOfRange( "ArraySequence: index out of range" );
        }
        if ( count_ >= static_cast<int>( items_->GetCount() ) ) {
            int new_cap = ( items_->GetCount() == 0 ) ? 1 : items_->GetCount() * 2;
            items_->Resize( new_cap );
        }
        for ( int idx = count_; idx > index; --idx ) { // двигаем только реальные элементы
            items_->Set( idx, items_->Get( idx - 1 ) );
        }
        items_->Set( index, item );
        ++count_;
        return this;
    }

public:
    ~ArraySequence() override {
        delete items_;
    }

    IEnumerator<T>* GetEnumerator() override {
        return new SequenceIterator<T>( this );
        // "this" — это сам ArraySequence*, который является Sequence<T>*
    }

    ArraySequence() : items_( new DynamicArray<T>(0) ), count_(0) {}

    ArraySequence( T* data, int count )
        : items_( new DynamicArray<T>(data, count) ), count_( count ) {}

    ArraySequence( const ArraySequence<T>& other )
        : items_( new DynamicArray<T>(*other.items_) ), count_( other.count_ ) {}

    // Декомпозиция
    T& GetFirst() const override {
        if ( count_ == 0 ) {
            throw IndexOutOfRange( "ArraySequence: sequence is empty" );
        }
        return items_->Get( 0 );
    }

    T& GetLast() const override {
        if ( count_ == 0 ) {
            throw IndexOutOfRange( "ArraySequence: sequence is empty" );
        }
        return items_->Get( count_ - 1 );
    }

    T& Get( std::size_t index ) override {
        if ( static_cast<int>(index) >= count_ )
            throw IndexOutOfRange( "ArraySequence: index out of range" );
        return items_->Get(index);
    }

    const T& Get( std::size_t index ) const override {
        if ( static_cast<int>(index) >= count_ )
            throw IndexOutOfRange( "ArraySequence: index out of range" );
        return items_->Get(index);
    }

    int GetLength() const override {
        return count_;
    }

    Sequence<T>* GetSubsequence( int start, int end ) const override {
        if ( start < 0 || start >= count_ ) {
            throw IndexOutOfRange( "ArraySequence: start index out of range" );
        }
        if ( end < 0 || end >= count_ ) {
            throw IndexOutOfRange( "ArraySequence: end index out of range" );
        }
        if ( start > end ) {
            throw std::invalid_argument( "ArraySequence: start > end" );
        }

        Sequence<T>* result = this->CreateEmpty();
        for ( int idx = start; idx <= end; ++idx ) {
            Sequence<T>* next = result->Append( items_->Get( idx ) );
            if ( next != result ) delete result; // для Immutable удаляем старую копию
            result = next;
        }
        return result;
    }

    const T& operator[]( int index ) const {
        if ( index < 0 || index >= count_ )
            throw IndexOutOfRange("ArraySequence: index out of range");
        return ( *items_ )[index];
    }

    // Оператор записи: seq[i] = value, только для ArraySequence
    T& operator[]( int index ) {
        if ( index < 0 || index >= count_ )
            throw IndexOutOfRange("ArraySequence: index out of range");
        return ( *items_ )[index]; // делегируем DynamicArray, т.к. там уже имеется T& operator[]
    }

    // Операции через паттерн Instance()
    Sequence<T>* Append( const T& item ) override {
        return Instance()->AppendInternal( item );
    }

    Sequence<T>* Prepend( const T& item ) override {
        return Instance()->PrependInternal( item );
    }

    Sequence<T>* InsertAt( const T& item, int index ) override {
        return Instance()->InsertAtInternal( item, index );
    }

    Sequence<T>* Concat( Sequence<T>* other ) const override {
        if ( other == nullptr )
            throw std::invalid_argument( "ArraySequence: other is nullptr" );

        Sequence<T>* result = this->CreateEmpty();

        for ( int idx = 0; idx < GetLength(); ++idx ) {
            Sequence<T>* next = result->Append( items_->Get( idx ) );
            if ( next != result ) delete result;
            result = next;
        }

        for ( int idx = 0; idx < other->GetLength(); ++idx ) {
            Sequence<T>* next = result->Append( other->Get( idx ) );
            if ( next != result ) delete result;
            result = next;
        }
        return result;
    }
};

//---------------------------------------------------------------------------------------
template <class T>
class MutableArraySequence : public ArraySequence<T> {
protected:
    // Возвращается изначальный, но уже измененный объект
    ArraySequence<T>* Instance() override {
        return this;
    }

public:
    MutableArraySequence() : ArraySequence<T>() { }

    MutableArraySequence( T* data, int count ) : ArraySequence<T>( data, count ) { }

    MutableArraySequence( const ArraySequence<T>& other ) : ArraySequence<T>( other ) { }

    Sequence<T>* CreateEmpty() const override {
        return new MutableArraySequence<T>();
    }
};

//----------------------------------------------------------------------------------------
template <class T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
    // Возвращается новый объект
    ArraySequence<T>* Instance() override {
        return new ImmutableArraySequence<T>( *this );
    }

public:
    ImmutableArraySequence() : ArraySequence<T>() { }

    ImmutableArraySequence( T* data, int count ) : ArraySequence<T>( data, count ) { }

    ImmutableArraySequence( const ArraySequence<T>& other ) : ArraySequence<T>( other ) { }

    Sequence<T>* CreateEmpty() const override {
        return new ImmutableArraySequence<T>();
    }
};

