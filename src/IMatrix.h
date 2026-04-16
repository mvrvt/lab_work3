#pragma once

#include <cstddef>   // для size_t
#include <stdexcept> // для вывода ошибок

template <typename T>
class IMatrix {
public:
    // Базовые свойства
    virtual size_t GetRows() const = 0;
    virtual size_t GetCols() const = 0;

    // Доступ к элементам
    virtual const T&   Get( size_t i, size_t j )           const = 0;
    virtual void       Set( size_t i, size_t j, const T& value ) = 0;

    // Операторы, которые изменяют текущий объект
    virtual IMatrix<T>& operator+=( const IMatrix<T>& other ) = 0;
    virtual IMatrix<T>& operator*=( const T& scalar )         = 0;

    // Норма Фробениуса
    virtual T Norm() const = 0;

    virtual void Print() const = 0;

    virtual ~IMatrix() = default;
};
