#pragma once

#include "Complex.hpp"
#include <iostream>
#include <cmath>

// Оператор ввода
template <typename T>
std::istream& operator>>( std::istream& is, Complex<T>& c ) {
    T r, i;
    if ( is >> r >> i ) {
        c = Complex<T>( r, i );
    }
    return is;
}

// Оператор вывода
template <typename T>
std::ostream& operator<<( std::ostream& os, const Complex<T>& c ) {
    os << c.re;
    if ( c.im >= T( 0 ) ) {
        os << " + " << c.im << "i";
    } else {
        // чтобы не было " + -2i", выводим " - 2i"
        os << " - " << std::abs( c.im ) << "i";
    }
    return os;
}

