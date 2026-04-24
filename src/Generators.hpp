#pragma once

#include "SquareMatrix.hpp"
#include "Vector.hpp"
#include <random>
#include <cmath>

namespace LinAlg {

    // 1. Генерация случайной матрицы
    // Используем seed для воспроизводимости (чтобы при каждом запуске генерировались одни и те же числа)
    template <typename T>
    SquareMatrix<T> GenerateRandomMatrix(size_t n, int seed = 42) {
        SquareMatrix<T> mat(n);
        std::mt19937 gen(seed); // Генератор Мерсенна
        std::uniform_real_distribution<double> dist(-1.0, 1.0); // Равномерное распределение

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                mat.Set(i, j, static_cast<T>(dist(gen)));
            }
        }
        return mat;
    }

    // 2. Генерация случайного вектора
    template <typename T>
    Vector<T> GenerateRandomVector(size_t n, int seed = 42) {
        Vector<T> vec(n);
        std::mt19937 gen(seed);
        std::uniform_real_distribution<double> dist(-1.0, 1.0);

        for (size_t i = 0; i < n; ++i) {
            vec[i] = static_cast<T>(dist(gen));
        }
        return vec;
    }

    // 3. Генерация матрицы Гильберта
    // Формула H_{ij} = 1 / (i + j - 1) для 1-индексации.
    // Поскольку у нас 0-индексация, формула будет 1 / (i + j + 1).
    template <typename T>
    SquareMatrix<T> GenerateHilbertMatrix(size_t n) {
        SquareMatrix<T> mat(n);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                mat.Set(i, j, static_cast<T>(1.0 / (i + j + 1.0)));
            }
        }
        return mat;
    }

    // 4. Вычисление нормы невязки: ||b - Ax||
    template <typename T>
    T CalculateResidual(const IMatrix<T>& A, const Vector<T>& x, const Vector<T>& b) {
        Vector<T> Ax = A * x;
        Vector<T> r = b - Ax; //  r = b - Ax
        return r.Norm();
    }
}
