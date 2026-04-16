#pragma once

#include <functional>
#include <stdexcept>
#include <algorithm>
#include "DynamicArray.h"
#include "ICollection.h"
#include "IEnumerable.h"

// forward declaration (нужен для GetPrefixes и GetPostfixes)
template <class T> class MutableArraySequence;

template <class T>
class Sequence : public ICollection<T>, public IEnumerable<T> {
public:
    // GetEnumerator() унаследован от IEnumerable<T> как чисто виртуальный —
    // каждый конкретный наследник (ArraySequence, ListSequence) обязан
    // его переопределить через override.

    ~Sequence() override = default;

    // ICollection
    T& Get( std::size_t index )               override = 0;
    virtual const T& Get( std::size_t index ) const    = 0;
    std::size_t GetCount() const override {
        return static_cast<std::size_t>(GetLength());
    }

    // Декомпозиция
    virtual T&           GetFirst()                           const = 0;
    virtual T&           GetLast()                            const = 0;
    virtual int          GetLength()                          const = 0;
    virtual Sequence<T>* GetSubsequence( int start, int end ) const = 0;

    const T& operator[]( int index ) const {
        if ( index < 0 || static_cast<size_t>( index ) >= static_cast<size_t>( GetLength() ) )
            throw IndexOutOfRange( "Sequence: index out of range" );
        return Get( static_cast<size_t>( index ) );
    }

    T& operator[]( int index ) {
        if ( index < 0 || static_cast<size_t>( index ) >= static_cast<size_t>( GetLength() ) )
            throw IndexOutOfRange( "Sequence: index out of range" );
        return Get( static_cast<size_t>( index ) );
    }

    // Операции
    virtual Sequence<T>* Append( const T& item )                    = 0;
    virtual Sequence<T>* Prepend( const T& item )                   = 0;
    virtual Sequence<T>* InsertAt( const T& item, int index )       = 0;
    virtual Sequence<T>* Concat( Sequence<T>* other)          const = 0;

    // Фабричный метод
    virtual Sequence<T>* CreateEmpty() const = 0;

    // Map / Where / Reduce
    // Реализованы один раз, но работают для всех наследников
    Sequence<T>* Map( std::function<T( const T& )> func ) const {
        Sequence<T>* result = CreateEmpty();
        for ( int idx = 0; idx < GetLength(); ++idx ) {
            Sequence<T>* next = result->Append( func( Get( idx ) ) );
            if ( next != result ) delete result; // если immutable - удаляем старую копию
            result = next;
        }
        return result;
    }

    // M-2.2: MapIndexed
    Sequence<T>* MapIndexed( std::function<T( const T&, int )> func ) const {
        Sequence<T>* result = CreateEmpty();
        for ( int idx = 0; idx < GetLength(); ++idx ) {
            Sequence<T>* next = result->Append( func( Get( idx ), idx ) );
            if ( next != result ) delete result;
            result = next;
        }
        return result;
    }

    // М-2.2 расширенный: MapIndexed с конвертацией типа Т -> T2
    // Используется в П-6 (int -> double)
    template <typename T2>
    Sequence<T2>* MapIndexedTo( std::function<T2( const T&, int )> func ) const {
        MutableArraySequence<T2>* result = new MutableArraySequence<T2>();
        for ( int idx = 0; idx < GetLength(); ++idx ) {
            result->Append( func( Get( idx ), idx ) );
        }
        return result;
    }

    Sequence<T>* Where( std::function<bool( const T& )> predicate ) const {
        Sequence<T>* result = CreateEmpty();
        for ( int idx = 0; idx < GetLength(); ++idx ) {
            if ( predicate( Get( idx ) ) ) {
                Sequence<T>* next = result->Append( Get( idx ) );
                if ( next != result ) delete result;
                result = next;
            }
        }
        return result;
    }

    // Reduce может возвращать другой тип (Т2) - поэтому template-метод
    // Например: Sequence<int> -> Reduce -> double (среднее)
    template <class T2>
    T2 Reduce( std::function<T2( T2, const T& )> func, T2 initial ) const {
        T2 result = initial;
        for ( int idx = 0; idx < GetLength(); ++idx ) {
            result = func( result, Get( idx ) );
        }
        return result;
    }

    Sequence<T>* ZipWithReversed() const {
        int n = GetLength();
        return MapIndexed( [this, n]( const T& val, int idx ) {
            return val + Get( n - idx - 1 );
        });
    }

    // М-2.1: пропустить первые count элементов
    Sequence<T>* Skip( int count ) const {
        if ( count < 0 )
            throw std::invalid_argument( "Sequence: Skip count < 0" );
        if ( count >= GetLength() )
            return this->CreateEmpty();
        return GetSubsequence( count, GetLength() - 1 );
    }

    // M-2.1: взять первые count элементов
    Sequence<T>* Take( int count ) const {
        if ( count < 0 )
            throw std::invalid_argument( "Sequence: Take count < 0" );
        if ( count == 0 )
            return this->CreateEmpty();
        int actual = std::min( count, GetLength() );
        return GetSubsequence( 0, actual - 1 );
    }

    // M-2.1: FlatMap — каждый элемент превращается в подпоследовательность,
    // все подпоследовательности склеиваются в одну
    // Возвращает MutableArraySequence<T2> (т.к. тип T2 отличается от T,
    // нельзя использовать CreateEmpty())
    template <typename T2>
    Sequence<T2>* FlatMap( std::function<Sequence<T2>*( const T& )> func ) const {
        MutableArraySequence<T2>* result = new MutableArraySequence<T2>();
        try {
            for ( int idx = 0; idx < GetLength(); ++idx ) {
                Sequence<T2>* part = func( Get( idx ) );
                try {
                    for ( int j = 0; j < part->GetLength(); ++j ) {
                        result->Append( part->Get( j ) );
                    }
                } catch ( ... ) {
                    delete part; // удаляем part, если при копировании что-то пошло не так
                    throw;
                }
                delete part;
            }
        } catch ( ... ) {
            delete result; // удаляем накопленный результат
            throw;         // пробрасываем значение дальше
        }
        return result;
    }


    // M-2.1: Zip — соединяет this и other в список пар
    // Длина результата = min(len(this), len(other))
    template <typename T2>
    Sequence<std::pair<T, T2>>* Zip( const Sequence<T2>* other ) const {
        if ( other == nullptr )
            throw std::invalid_argument( "Sequence: Zip other is empty(nullptr)" );
        int len = std::min( GetLength(), other->GetLength() );
        MutableArraySequence<std::pair<T, T2>>* result = new MutableArraySequence<std::pair<T, T2>>();
        for ( int idx = 0; idx < len; ++idx ) {
            result->Append( { Get( idx ), other->Get( idx ) } );
        }
        return result;
    }

    // П-1: (min, max, avg) за один проход через Reduce
    struct MinMaxAvg {
        T min;
        T max;
        double sum;
        int count;
    };

    MinMaxAvg GetMinMaxAvg() const {
        if ( GetLength() == 0 ) {
            throw IndexOutOfRange( "Sequence: GetMinMaxAvg called on empty sequence" );
        }

        T first = Get( 0 );
        MinMaxAvg initial = { first, first, 0.0, 0 };

        return Reduce<MinMaxAvg>(
            []( MinMaxAvg result, const T& value ) {
                if ( value < result.min ) { result.min = value; }
                if ( value > result.max ) { result.max = value; }
                result.sum += static_cast<double>( value );
                ++result.count;
                return result;
            },
            initial
        );
    }
    double GetAvg() const {
        auto r = GetMinMaxAvg();
        return r.sum / r.count;
    }

    T GetMin() const { return GetMinMaxAvg().min; }
    T GetMax() const { return GetMinMaxAvg().max; }
};
