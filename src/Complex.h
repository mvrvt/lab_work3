#pragma once

#include <cmath>
#include <stdexcept>

template <typename T>
struct Complex {
    T re;
    T im;

    // Конструктор по умолчанию. Без explicit!
    // Позволяет делать так: Complex<double> c = 5.0; (im будет 0)
    Complex( T r = T( 0 ), T i = T( 0 ) ) : re( r ), im( i ) { }

    // Арифметика
    Complex operator+( const Complex& o ) const { return Complex( re + o.re, im + o.im ); }
    Complex operator-( const Complex& o ) const { return Complex( re - o.re, im - o.im ); }
    Complex operator-() const { return Complex( -re, -im); } // унарный минус

    Complex operator*( const Complex& o ) const {
        return Complex( re * o.re - im * o.im , re * o.im + im * o.re );
    }

    Complex operator/( const Complex& o ) const {
        T denominator = o.re * o.re + o.im * o.im;
        if ( denominator == T( 0 ) )
            throw std::domain_error( "Complex: division by zero" );
        return Complex( ( re * o.re + im * o.im ) / denominator,
                        ( im * o.re - re * o.im ) / denominator );
    }

    // Сравнения
    bool operator==( const Complex& o ) const { return re == o.re && im == o.im; }
    bool operator!=( const Complex& o ) const { return !( *this == o ); }

    // Операторы присваивания с вычислением (нужно для += в матрицах)
    Complex& operator+=( const Complex& o ) { *this = *this + o; return *this; }
    Complex& operator*=( const Complex& o ) { *this = *this * o; return *this; }
};

// Глобальная функция abs
// Когда мы вызываем abs(val), компилятор ищет функцию abs рядом с типом val
template <typename T>
double abs( const Complex<T>& c ) {
    return std::sqrt( static_cast<double>( c.re * c.re + c.im * c.im ) );
}
