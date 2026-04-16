#pragma once

#include "IEnumerator.h"
#include "Sequence.h"

template <class T>
class SequenceIterator : public IEnumerator<T> {
private:
    Sequence<T>* seq_;    // указатель на любую последовательность
    int          index_;  // -1 = до первого элемента

public:
    explicit SequenceIterator( Sequence<T>* seq ) : seq_( seq ), index_( -1 ) { }

    bool MoveNext() override {
        ++index_;
        return index_ < seq_->GetLength();
    }

    T& Current() override {
        if ( index_ < 0 || index_ >= seq_->GetLength() )
            throw IndexOutOfRange( "SequenceIterator: index out of range" );
        return seq_->Get( index_ );
    }

    void Reset() override {
        index_ = -1;
    }
};
