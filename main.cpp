#include "src/Matrices_io.hpp"
#include "src/Matrix_operators.hpp"
#include "src/Complex_io.hpp"
#include "src/SLAE.hpp"
#include "src/Generators.hpp"
#include "src/lab2_files/ArraySequence.h"
#include "src/lab2_files/ListSequence.hpp"
#include "src/BandMatrix.hpp"
#include "src/DiagonalMatrix.hpp"
#include "src/SparseMatrix.hpp"
#include "src/TriangularMatrix.hpp"
#include "src/SquareMatrix.hpp"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>
#include <random>
#include <string>
#include <type_traits>

using namespace std::chrono;

// Структура для возврата двух матриц (замена std::pair)
template <typename T>
struct DecompositionResult {
    SquareMatrix<T> first;
    SquareMatrix<T> second;
};

// ===================================================================
// УТИЛИТЫ И БЕЗОПАСНЫЙ ВВОД
// ===================================================================

void ClearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int ReadInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) continue;
        try {
            size_t pos;
            int value = std::stoi(line, &pos);
            if (pos != line.length()) throw std::invalid_argument("");
            return value;
        } catch (...) {
            std::cout << " [!] Введите целое число.\n";
        }
    }
}

// Шаблонный безопасный ввод для любого типа (double или Complex)
template <typename T>
T ReadValue(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        T val;
        if (std::cin >> val) {
            ClearInput();
            return val;
        }
        std::cout << " [!] Некорректный ввод. Попробуйте снова.\n";
        ClearInput();
    }
}

// ===================================================================
// ГЕНЕРАТОРЫ СЛУЧАЙНЫХ ЧИСЕЛ
// ===================================================================

static std::mt19937 gRng{std::random_device{}()};

static double RandomDouble(double lo, double hi) {
    std::uniform_real_distribution<double> dist(lo, hi);
    return dist(gRng);
}

static int RandomInt(int lo, int hi) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(gRng);
}

// Полиморфный генератор с C++17 if constexpr
template <typename T>
T RandomValue(double lo, double hi) {
    if constexpr (std::is_same_v<T, double>) {
        return RandomDouble(lo, hi);
    } else {
        return T(RandomDouble(lo, hi), RandomDouble(lo, hi)); // Для Complex
    }
}

template <typename T>
static void FillMatrixRandom(IMatrix<T>& mat, double lo, double hi) {
    for (size_t i = 0; i < mat.GetRows(); ++i)
        for (size_t j = 0; j < mat.GetCols(); ++j)
            mat.Set(i, j, RandomValue<T>(lo, hi));
}

template <typename T>
void OfferNorm(const IMatrix<T>& mat) {
    int ch = ReadInt(" 1 – вычислить норму Фробениуса, 0 – продолжить: ");
    if (ch == 1) {
        std::cout << " Норма Фробениуса: " << mat.Norm() << "\n";
    }
}

template <typename T>
void PrintMatrixInfo(const IMatrix<T>& mat, const std::string& name) {
    std::cout << name << " [" << mat.GetRows() << "x" << mat.GetCols() << "]:\n";
    std::cout << mat << std::endl;
}

// ===================================================================
// МЕНЮ ОПЕРАЦИЙ НАД МАТРИЦАМИ
// ===================================================================

template <typename T>
void MatrixOpsMenu(MutableArraySequence<IMatrix<T>*>& matrices, int index) {
    IMatrix<T>* m = matrices.Get(index);
    while (true) {
        std::cout << "\n";
        std::cout << " |-- Операции над матрицей [" << index << "] (" << m->GetRows() << "x" << m->GetCols() << ") ----|\n";
        std::cout << " |  1. Вывести на экран                      |\n";
        std::cout << " |  2. Вычислить норму                       |\n";
        std::cout << " |  3. Умножить на скаляр                    |\n";
        std::cout << " |  4. Сложить с другой матрицей             |\n";
        std::cout << " |  5. Элементарное преобразование строк     |\n";
        std::cout << " |  6. QR-разложение (БОНУС)                 |\n";
        std::cout << " |  0. Назад                                 |\n";
        std::cout << " |-------------------------------------------|\n";

        int choice = ReadInt(" Выбор: ");
        if (choice == 0) break;

        try {
            if (choice == 1) {
                std::cout << *m;
            } else if (choice == 2) {
                std::cout << " Норма: " << m->Norm() << "\n";
            } else if (choice == 3) {
                T scalar = ReadValue<T>(" Введите скаляр: ");
                *m *= scalar;
                std::cout << " Успешно умножено.\n";
            } else if (choice == 4) {
                int other_idx = ReadInt(" Индекс второй матрицы: ");
                if (other_idx >= 0 && other_idx < matrices.GetLength()) {
                    *m += *(matrices.Get(other_idx));
                    std::cout << " Матрицы успешно сложены.\n";
                } else {
                    std::cout << " [!] Неверный индекс.\n";
                }
            } else if (choice == 5) {
                Matrix<T>* dense = dynamic_cast<Matrix<T>*>(m);
                if (dense) {
                    int r1 = ReadInt(" Индекс первой строки: ");
                    int r2 = ReadInt(" Индекс второй строки: ");
                    dense->SwapRows(r1, r2);
                    std::cout << " Строки поменяны местами.\n";
                } else {
                    std::cout << " [!] Элементарные преобразования доступны только для Dense матриц.\n";
                }
            } else if (choice == 6) {
                SquareMatrix<T>* sq = dynamic_cast<SquareMatrix<T>*>(m);
                if (sq) {
                    auto qr = LinAlg::QRDecomposition(*sq);
                    std::cout << "\n Матрица Q (ортогональная):\n" << qr.first;
                    std::cout << "\n Матрица R (верхняя треугольная):\n" << qr.second;
                } else {
                    std::cout << " [!] QR-разложение реализовано только для квадратных матриц.\n";
                }
            } else {
                std::cout << " [!] Неизвестная команда.\n";
            }
        } catch (const std::exception& e) {
            std::cout << " [!] Ошибка: " << e.what() << "\n";
        }
    }
}

template <typename T>
void MatrixMenu(MutableArraySequence<IMatrix<T>*>& matrices) {
    while (true) {
        std::cout << "\n";
        std::cout << " |===========================================|\n";
        std::cout << " |               МЕНЮ МАТРИЦ                 |\n";
        std::cout << " |===========================================|\n";
        std::cout << " |  В памяти: " << std::setw(30) << std::left << matrices.GetLength() << "|\n";
        std::cout << " |  1.  Прямоугольная матрица (Matrix)       |\n";
        std::cout << " |  2.  Квадратная матрица (SquareMatrix)    |\n";
        std::cout << " |  3.  Диагональная матрица (DiagonalMatrix)|\n";
        std::cout << " |  4.  Ленточная матрица (BandMatrix)       |\n";
        std::cout << " |  5.  Разреженная матрица (SparseMatrix)   |\n";
        std::cout << " |  6.  Треугольная матрица (TriangularMatrix)|\n";
        std::cout << " |  7.  Выбрать матрицу для операций         |\n";
        std::cout << " |  8.  Показать все сохраненные матрицы     |\n";
        std::cout << " |  0.  Назад                                |\n";
        std::cout << " |===========================================|\n";
        int choice = ReadInt(" Выбор: ");

        if (choice == 0) break;

        try {
            IMatrix<T>* new_mat = nullptr;

            if (choice == 1) {
                size_t rows = ReadInt(" Количество строк: ");
                size_t cols = ReadInt(" Количество столбцов: ");
                new_mat = new Matrix<T>(rows, cols);
                int fill = ReadInt(" Заполнить: 1 – вручную, 2 – случайно: ");
                if (fill == 1) {
                    std::cout << " Введите элементы построчно:\n";
                    for (size_t i = 0; i < rows; ++i)
                        for (size_t j = 0; j < cols; ++j)
                            new_mat->Set(i, j, ReadValue<T>(" [" + std::to_string(i) + "][" + std::to_string(j) + "]: "));
                } else {
                    double lo = ReadValue<double>(" Нижняя граница (вещественное число): ");
                    double hi = ReadValue<double>(" Верхняя граница (вещественное число): ");
                    FillMatrixRandom(*new_mat, lo, hi);
                }
            }
            else if (choice == 2) {
                size_t n = ReadInt(" Размер: ");
                new_mat = new SquareMatrix<T>(n);
                int fill = ReadInt(" Заполнить: 1 – вручную, 2 – случайно: ");
                if (fill == 1) {
                    std::cout << " Введите элементы построчно:\n";
                    for (size_t i = 0; i < n; ++i)
                        for (size_t j = 0; j < n; ++j)
                            new_mat->Set(i, j, ReadValue<T>(" [" + std::to_string(i) + "][" + std::to_string(j) + "]: "));
                } else {
                    double lo = ReadValue<double>(" Нижняя граница (вещественное число): ");
                    double hi = ReadValue<double>(" Верхняя граница (вещественное число): ");
                    FillMatrixRandom(*new_mat, lo, hi);
                }
            }
            else if (choice == 3) {
                size_t n = ReadInt(" Размер: ");
                new_mat = new DiagonalMatrix<T>(n);
                int fill = ReadInt(" Заполнить: 1 – вручную (диагональ), 2 – случайно: ");
                if (fill == 1) {
                    for (size_t i = 0; i < n; ++i)
                        new_mat->Set(i, i, ReadValue<T>(" diag[" + std::to_string(i) + "]: "));
                } else {
                    double lo = ReadValue<double>(" Нижняя граница (вещественное число): ");
                    double hi = ReadValue<double>(" Верхняя граница (вещественное число): ");
                    for (size_t i = 0; i < n; ++i)
                        new_mat->Set(i, i, RandomValue<T>(lo, hi));
                }
            }
            else if (choice == 4) {
                size_t n = ReadInt(" Размер: ");
                std::cout << " Будут использованы смещения -1, 0, 1 (трёхдиагональная матрица).\n";
                DynamicArray<int> offsets;
                offsets.Append(-1); offsets.Append(0); offsets.Append(1);

                BandMatrix<T>* band = new BandMatrix<T>(n, offsets);
                int fill = ReadInt(" Заполнить: 1 – вручную, 2 – случайно: ");
                if (fill == 1) {
                    for (size_t i = 1; i < n; ++i) band->Set(i, i-1, ReadValue<T>(" [" + std::to_string(i) + "][" + std::to_string(i-1) + "]: "));
                    for (size_t i = 0; i < n; ++i) band->Set(i, i, ReadValue<T>(" [" + std::to_string(i) + "][" + std::to_string(i) + "]: "));
                    for (size_t i = 0; i < n-1; ++i) band->Set(i, i+1, ReadValue<T>(" [" + std::to_string(i) + "][" + std::to_string(i+1) + "]: "));
                } else {
                    double lo = ReadValue<double>(" Нижняя граница (вещественное число): ");
                    double hi = ReadValue<double>(" Верхняя граница (вещественное число): ");
                    for (size_t i = 1; i < n; ++i) band->Set(i, i-1, RandomValue<T>(lo, hi));
                    for (size_t i = 0; i < n; ++i) band->Set(i, i, RandomValue<T>(lo, hi));
                    for (size_t i = 0; i < n-1; ++i) band->Set(i, i+1, RandomValue<T>(lo, hi));
                }
                new_mat = band;
            }
            else if (choice == 5) {
                size_t rows = ReadInt(" Количество строк: ");
                size_t cols = ReadInt(" Количество столбцов: ");
                SparseMatrix<T>* sparse = new SparseMatrix<T>(rows, cols);
                int fill = ReadInt(" Заполнить: 1 – вручную, 2 – случайно: ");
                if (fill == 1) {
                    int nz = ReadInt(" Сколько ненулевых элементов ввести? ");
                    for (int k = 0; k < nz; ++k) {
                        size_t i = ReadInt(" строка: ");
                        size_t j = ReadInt(" столбец: ");
                        T val = ReadValue<T>(" значение: ");
                        sparse->Set(i, j, val);
                    }
                } else {
                    int nz = ReadInt(" Сколько ненулевых элементов сгенерировать? ");
                    double lo = ReadValue<double>(" Нижняя граница (вещественное число): ");
                    double hi = ReadValue<double>(" Верхняя граница (вещественное число): ");

                    int generated = 0;
                    while (generated < nz) {
                        size_t i = RandomInt(0, static_cast<int>(rows - 1));
                        size_t j = RandomInt(0, static_cast<int>(cols - 1));
                        if (sparse->Get(i, j) == T(0)) {
                            sparse->Set(i, j, RandomValue<T>(lo, hi));
                            generated++;
                        }
                    }
                    std::cout << " Сгенерировано " << nz << " ненулевых элементов.\n";
                }
                new_mat = sparse;
            }
            else if (choice == 6) {
                size_t n = ReadInt(" Размер: ");
                int upLow = ReadInt(" 0 – верхняя треугольная, 1 – нижняя треугольная: ");
                TriangularType tt = (upLow == 0) ? TriangularType::Upper : TriangularType::Lower;
                TriangularMatrix<T>* tri = new TriangularMatrix<T>(n, tt);
                int fill = ReadInt(" Заполнить: 1 – вручную, 2 – случайно: ");
                if (fill == 1) {
                    if (tt == TriangularType::Upper) {
                        for (size_t i = 0; i < n; ++i)
                            for (size_t j = i; j < n; ++j)
                                tri->Set(i, j, ReadValue<T>(" [" + std::to_string(i) + "][" + std::to_string(j) + "]: "));
                    } else {
                        for (size_t i = 0; i < n; ++i)
                            for (size_t j = 0; j <= i; ++j)
                                tri->Set(i, j, ReadValue<T>(" [" + std::to_string(i) + "][" + std::to_string(j) + "]: "));
                    }
                } else {
                    double lo = ReadValue<double>(" Нижняя граница (вещественное число): ");
                    double hi = ReadValue<double>(" Верхняя граница (вещественное число): ");
                    if (tt == TriangularType::Upper) {
                        for (size_t i = 0; i < n; ++i)
                            for (size_t j = i; j < n; ++j) tri->Set(i, j, RandomValue<T>(lo, hi));
                    } else {
                        for (size_t i = 0; i < n; ++i)
                            for (size_t j = 0; j <= i; ++j) tri->Set(i, j, RandomValue<T>(lo, hi));
                    }
                }
                new_mat = tri;
            }
            else if (choice == 7) {
                int idx = ReadInt(" Введите индекс матрицы: ");
                if (idx >= 0 && idx < matrices.GetLength()) {
                    MatrixOpsMenu(matrices, idx);
                } else {
                    std::cout << " [!] Индекс вне диапазона.\n";
                }
                continue;
            }
            else if (choice == 8) {
                for (int i = 0; i < matrices.GetLength(); ++i) {
                    std::cout << "\nМатрица [" << i << "]:\n" << *(matrices.Get(i));
                }
                continue;
            }

            if (new_mat) {
                matrices.Append(new_mat);
                PrintMatrixInfo(*new_mat, "Создана матрица");
                OfferNorm(*new_mat);
            }

        } catch (const std::exception& e) {
            std::cout << " [!] Ошибка: " << e.what() << "\n";
        }
    }
}

// ===================================================================
// РЕШЕНИЕ СЛАУ (РАБОТА С КОНКРЕТНЫМИ СИСТЕМАМИ)
// ===================================================================

template <typename T>
void SLAEMenu() {
    while (true) {
        std::cout << "\n";
        std::cout << " |===========================================|\n";
        std::cout << " |           Решение систем линейных уравнений |\n";
        std::cout << " |===========================================|\n";
        std::cout << " |  1.  Метод Гаусса (с выбором/без)         |\n";
        std::cout << " |  2.  LU-разложение + решение              |\n";
        std::cout << " |  3.  QR-разложение (ортогонализация)      |\n";
        std::cout << " |  4.  Ручной ввод СЛАУ                     |\n";
        std::cout << " |  0.  Назад                                |\n";
        std::cout << " |===========================================|\n";
        int choice = ReadInt(" Выбор: ");
        if (choice == 0) break;

        try {
            if (choice == 1) {
                size_t n = ReadInt(" Размер системы: ");
                auto A = LinAlg::GenerateRandomMatrix<T>(n, 42);
                auto b = LinAlg::GenerateRandomVector<T>(n, 42);
                int usePivot = ReadInt(" Использовать выбор ведущего? (1-да,0-нет): ");
                Vector<T> x = LinAlg::SolveGauss(A, b, usePivot == 1);
                std::cout << " Решение x:\n[ ";
                for (size_t i = 0; i < std::min(n, static_cast<size_t>(5)); ++i) std::cout << x[i] << " ";
                std::cout << (n > 5 ? "... ]\n" : "]\n");
                std::cout << " Норма невязки: " << (b - A * x).Norm() << "\n";
            }
            else if (choice == 2) {
                size_t n = ReadInt(" Размер системы: ");
                auto A = LinAlg::GenerateRandomMatrix<T>(n, 42);
                auto b = LinAlg::GenerateRandomVector<T>(n, 42);
                std::cout << " Случайная матрица A и вектор b.\n";
                auto [L, U] = LinAlg::LUDecomposition(A);
                Vector<T> x = LinAlg::SolveLU(L, U, b);
                std::cout << " Решение x (первые элементы):\n[ ";
                for (size_t i = 0; i < std::min(n, static_cast<size_t>(5)); ++i) std::cout << x[i] << " ";
                std::cout << (n > 5 ? "... ]\n" : "]\n");
                std::cout << " Норма невязки: " << (b - A * x).Norm() << "\n";
            }
            else if (choice == 3) {
                size_t n = ReadInt(" Размер квадратной матрицы (рекомендуется 3-5): ");
                auto A = LinAlg::GenerateRandomMatrix<T>(n, 123);
                std::cout << "A:\n" << A;
                auto [Q, R] = LinAlg::QRDecomposition(A);
                std::cout << "Q:\n" << Q;
                std::cout << "R:\n" << R;
            }
            else if (choice == 4) {
                size_t n = ReadInt(" Размер системы: ");
                SquareMatrix<T> A(n);
                Vector<T> b(n);
                std::cout << " Введите матрицу A построчно:\n";
                for (size_t i = 0; i < n; ++i)
                    for (size_t j = 0; j < n; ++j)
                        A.Set(i, j, ReadValue<T>(" A[" + std::to_string(i) + "][" + std::to_string(j) + "]: "));
                std::cout << " Введите вектор b:\n";
                for (size_t i = 0; i < n; ++i)
                    b[i] = ReadValue<T>(" b[" + std::to_string(i) + "]: ");

                Vector<T> x = LinAlg::SolveGauss(A, b, true);
                std::cout << " Решение x:\n[ ";
                for (size_t i = 0; i < n; ++i) std::cout << x[i] << " ";
                std::cout << "]\n";
                std::cout << " Норма невязки: " << (b - A * x).Norm() << "\n";
            }
        } catch (const std::exception& e) {
            std::cout << " [!] Ошибка: " << e.what() << "\n";
        }
    }
}

// ===================================================================
// ТЕСТЫ ПРОИЗВОДИТЕЛЬНОСТИ МАСТРИЦ (ЛР 3)
// ===================================================================

template <typename T>
void PerformanceMenu() {
    std::cout << "\n";
    std::cout << "================================================================================\n";
    std::cout << "                        ТЕСТЫ ПРОИЗВОДИТЕЛЬНОСТИ МАТРИЦ                         \n";
    std::cout << "================================================================================\n";

    size_t sizes[] = {100, 200, 500};
    const int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    const int w_size = 8;
    const int w_time = 14;

    std::cout << std::left << std::setw(w_size) << "Size";
    std::cout << std::right;
    std::cout << std::setw(w_time) << "Add";
    std::cout << std::setw(w_time) << "Mult";
    std::cout << std::setw(w_time) << "Scalar";
    std::cout << std::setw(w_time) << "Norm";
    std::cout << "\n" << std::string(w_size + w_time * 4, '-') << "\n";

    for (int i = 0; i < numSizes; ++i) {
        size_t n = sizes[i];
        std::cout << std::left << std::setw(w_size) << n << std::right;

        SquareMatrix<T> A = LinAlg::GenerateRandomMatrix<T>(n, 42);
        SquareMatrix<T> B = LinAlg::GenerateRandomMatrix<T>(n, 43);
        T scalar = T(2.5);

        auto measure = [&](auto&& func) -> double {
            auto start = high_resolution_clock::now();
            func();
            auto end = high_resolution_clock::now();
            return duration<double, std::milli>(end - start).count();
        };

        double t_add   = measure([&]() { volatile auto dummy = A + B; (void)dummy; });
        double t_mult  = measure([&]() { volatile auto dummy = A * B; (void)dummy; });
        double t_scalar= measure([&]() { volatile auto dummy = A * scalar; (void)dummy; });
        double t_norm = measure([&]() { volatile auto norm = A.Norm(); (void)norm; });

        std::cout << std::fixed << std::setprecision(2);
        std::cout << std::setw(w_time) << t_add;
        std::cout << std::setw(w_time) << t_mult;
        std::cout << std::setw(w_time) << t_scalar;
        std::cout << std::setw(w_time) << t_norm << "\n";
    }

    std::cout << "\n================================================================================\n";
    std::cout << "Тесты завершены. Нажмите Enter для продолжения...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// Тесты производительности СЛАУ

template <typename T>
void RunSLAEExperiments() {
    std::cout << "\n================================================================================\n";
    std::cout << "                        ТЕСТЫ ПРОИЗВОДИТЕЛЬНОСТИ СЛАУ                           \n";
    std::cout << "================================================================================\n";

    auto measure = [&](const std::string& name, auto&& func) -> double {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << std::fixed << std::setprecision(2) << "   " << std::left << std::setw(25) << name << ": " << ms << " мс\n";
        return ms;
    };

    std::cout << "\n-- Тест 4.1: Время для одной системы ---\n";
    size_t sizes_41[] = {100, 200, 500};
    const int numSizes41 = sizeof(sizes_41) / sizeof(sizes_41[0]);

    for (int i = 0; i < numSizes41; ++i) {
        size_t n = sizes_41[i];
        std::cout << "\n N = " << n << ":\n";
        SquareMatrix<T> A = LinAlg::GenerateRandomMatrix<T>(n, 42);
        Vector<T> b = LinAlg::GenerateRandomVector<T>(n, 42);

        measure("Gauss (без выбора)", [&]() {
            volatile auto x = LinAlg::SolveGauss(A, b, false);
        });

        measure("Gauss (с выбором)", [&]() {
            volatile auto x = LinAlg::SolveGauss(A, b, true);
        });

        measure("LU (Полное решение)", [&]() {
            auto [L, U] = LinAlg::LUDecomposition(A);
            volatile auto x = LinAlg::SolveLU(L, U, b);
        });
    }

    std::cout << "\n-- Тест 4.2: Экономия при множестве b --\n";
    size_t n_multi = 200;
    size_t ks[] = {1, 10, 50};
    const int numKs = sizeof(ks) / sizeof(ks[0]);
    SquareMatrix<T> A_multi = LinAlg::GenerateRandomMatrix<T>(n_multi, 67);

    // Используем полиморфный массив вместо std::vector
    MutableArraySequence<Vector<T>> rhs_list;
    for (int i = 0; i < 50; ++i) {
        rhs_list.Append(LinAlg::GenerateRandomVector<T>(n_multi, i));
    }

    for (int i = 0; i < numKs; ++i) {
        size_t k = ks[i];
        std::cout << "\n k = " << k << " правых частей:\n";
        measure("Gauss (K раз полный)", [&]() {
            for (size_t j = 0; j < k; ++j) {
                volatile auto x = LinAlg::SolveGauss(A_multi, rhs_list.Get(static_cast<int>(j)), true);
            }
        });

        measure("LU (1 разлож. + K подст.)", [&]() {
            auto [L, U] = LinAlg::LUDecomposition(A_multi);
            for (size_t j = 0; j < k; ++j) {
                volatile auto x = LinAlg::SolveLU(L, U, rhs_list.Get(static_cast<int>(j)));
            }
        });
    }

    std::cout << "\n-- Тест 4.3: Точность (Матрица Гильберта) --\n";
    size_t hilbert_sizes[] = {5, 10, 15};
    const int numHSizes = sizeof(hilbert_sizes) / sizeof(hilbert_sizes[0]);

    for (int i = 0; i < numHSizes; ++i) {
        size_t hn = hilbert_sizes[i];
        std::cout << "\n N = " << hn << ":\n";
        SquareMatrix<T> H = LinAlg::GenerateHilbertMatrix<T>(hn);
        Vector<T> exact_x(hn);
        for(size_t j = 0; j < hn; ++j) exact_x[j] = T(1.0);
        Vector<T> b = H * exact_x;

        auto print_error = [&](const std::string& name, const Vector<T>& calc_x) {
            Vector<T> diff = calc_x - exact_x;
            T error = diff.Norm() / exact_x.Norm();
            std::cout << "   " << std::left << std::setw(20) << name << " отн. погрешность: " << std::scientific << error << "\n";
        };

        try {
            Vector<T> x_gauss = LinAlg::SolveGauss(H, b, false);
            print_error("Gauss (без выбора)", x_gauss);
        } catch (...) { std::cout << "   Gauss (без выбора) не справился.\n"; }

        try {
            Vector<T> x_gauss_piv = LinAlg::SolveGauss(H, b, true);
            print_error("Gauss (с выбором)", x_gauss_piv);
        } catch (...) { std::cout << "   Gauss (с выбором) не справился.\n"; }
    }

    std::cout << "\n================================================================================\n";
    std::cout << "Тесты СЛАУ завершены. Нажмите Enter для продолжения...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

template <typename T>
void RunApp() {
    // Хранилище реализовано строго через MutableArraySequence (По ТЗ ЛР 3)
    MutableArraySequence<IMatrix<T>*> matrices;

    while (true) {
        std::cout << "\n";
        std::cout << " |===========================================|\n";
        std::cout << " |  1.  Работа с матрицами                   |\n";
        std::cout << " |  2.  Решение СЛАУ (Гаусс, LU, QR)         |\n";
        std::cout << " |  3.  Тесты производительности (Матрицы)   |\n";
        std::cout << " |  4.  Тесты производительности СЛАУ (Замеры времени)   |\n";
        std::cout << " |  0.  Сменить тип данных / Выход           |\n";
        std::cout << " |===========================================|\n";
        int choice = ReadInt(" Выбор: ");

        if (choice == 0) break;
        else if (choice == 1) MatrixMenu<T>(matrices);
        else if (choice == 2) SLAEMenu<T>();
        else if (choice == 3) PerformanceMenu<T>();
        else if (choice == 4) RunSLAEExperiments<T>();
        else std::cout << " [!] Неверный выбор.\n";
    }

    for (int i = 0; i < matrices.GetLength(); ++i) {
        delete matrices.Get(i);
    }
}

int main() {
    std::cout << "\n |===========================================|\n";
    std::cout << " |      Лабораторная работа №3 (Информатика)  |\n";
    std::cout << " |            + Линейная алгебра              |\n";
    std::cout << " |===========================================|\n";
    while (true) {
        std::cout << "\n Выберите тип данных:\n";
        std::cout << " 1. Вещественные числа (double)\n";
        std::cout << " 2. Комплексные числа (Complex<double>)\n";
        std::cout << " 0. Выход\n";

        int choice = ReadInt(" Выбор: ");
        if (choice == 0) {
            std::cout << "\n До свидания!\n";
            break;
        }
        else if (choice == 1) RunApp<double>();
        else if (choice == 2) RunApp<Complex<double>>();
        else std::cout << " [!] Неизвестная команда.\n";
    }
    return 0;
}
