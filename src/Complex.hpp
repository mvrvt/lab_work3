#pragma once

#include <cmath>        // std::sqrt
#include <stdexcept>    // ошибки
#include <limits>       // std::numeric_limits (машинный эпсилон)
#include <type_traits>  // Для std::is_floating_point_v

template <typename T>

struct Complex {
    T re;
    T im;

    // Converting constructor
    Complex ( T r = zero_val, T i = zero_val ) : re( r ), im( i ) { }

    // Операторы модификации (изменяют текущий объект)

    Complex& operator+=( const Complex& o ) {
        re += o.re;
        im += o.im;
        return *this;
    }

    Complex operator-=( const Complex& o ) {
        re -= o.re;
        im -= o.im;
        return *this;
    }

    Complex& operator*=( const Complex& o ) {
        T new_re = re * o.re - im * o.im;
        T new_im = re * o.im + im * o.re;
        re = new_re;
        im = new_im;
        return *this;
    }

    Complex& operator/=( const Complex& o ) {
        T denominator = o.re * o.re + o.im * o.im;
        if ( denominator == zero_val )
            throw std::domain_error( "Complex: division by zero" );

        T new_re = ( re * o.re + im * o.im ) / denominator;
        T new_im = ( im * o.re - re * o.im ) / denominator;
        re = new_re;
        im = new_im;
        return *this;
    }

    Complex operator-( ) const {
        return Complex( -re, -im );
    }

    // Операторы сравнения (с учётом машинного эпсилон)

    bool operator==( const Complex& o ) const {
        if constexpr ( std::is_floating_point_v<T> ) {
            // Если Е - тип с плавающей точкой, то сравниваем через эпсилон
            using std::abs;
            return abs( re - o.re ) <= std::numeric_limits<T>::epsilon() &&
                   abs( im - o.im ) <= std::numeric_limits<T>::epsilon();
        } else {
            // Если Т - целочисленный тип, сравниваем напрямую
            return re = o.re && im == o.im;
        }
    }

    bool operator!=( const Complex& o ) const {
        return !( *this == o );
    }

    inline static const T zero_val { };
};


// Свободные бинарные операторы (создают новые объекты)
template <typename T>
Complex<T> operator+( Complex<T> a, const Complex<T>& b ) {
    a += b;
    return a;
}

template <typename T>
Complex<T> operator-( Complex<T> a, const Complex<T>& b ) {
    a -= b;
    return a;
}

template <typename T>
Complex<T> operator*( Complex<T> a, const Complex<T>& b ) {
    a *= b;
    return a;
}

template <typename T>
Complex<T> operator/( Complex<T> a, const Complex<T>& b ) {
    a /= b;
    return a;
}

template <typename T>
double abs( const Complex<T>& c ) {
    return std::sqrt( static_cast<double>( c.re * c.re + c.im * c.im ) );
}

