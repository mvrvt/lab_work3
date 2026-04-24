#include "src/Matrix_io.hpp"
#include "src/Complex_io.hpp"
#include "src/SLAE.hpp"
#include "src/Generators.hpp"
#include "src/lab2_files/ArraySequence.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>

using namespace std::chrono;

template <typename Func>
double MeasureExecutionTime(Func func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration_cast<duration<double, std::milli>>(end - start).count();
}

void TestHilbertAccuracy() {
    std::cout << "\n=== Проверка точности на матрице Гильберта (п. 4.3) ===\n";
    size_t sizes[] = {5, 10, 15};

    for (size_t n : sizes) {
        auto H = LinAlg::GenerateHilbertMatrix<double>(n);
        Vector<double> x_exact(n);
        for (size_t i = 0; i < n; ++i) x_exact[i] = 1.0;
        Vector<double> b = H * x_exact;

        std::cout << "Размер n = " << n << "\n";

        // 1. Гаусс с выбором
        try {
            Vector<double> x_g_pivot = LinAlg::SolveGauss(H, b, true);
            double err_g_pivot = (x_g_pivot - x_exact).Norm() / x_exact.Norm();
            std::cout << "  Гаусс (с выбором):   Погрешность = " << std::scientific << err_g_pivot << "\n";
        } catch(...) { std::cout << "  Гаусс (с выбором):   Ошибка вычисления\n"; }

        // 2. Гаусс без выбора
        try {
            Vector<double> x_g_nopivot = LinAlg::SolveGauss(H, b, false);
            double err_g_nopivot = (x_g_nopivot - x_exact).Norm() / x_exact.Norm();
            std::cout << "  Гаусс (без выбора):  Погрешность = " << std::scientific << err_g_nopivot << "\n";
        } catch(...) { std::cout << "  Гаусс (без выбора):  Ошибка вычисления\n"; }

        // 3. LU разложение
        try {
            auto [L, U] = LinAlg::LUDecomposition(H);
            Vector<double> x_lu = LinAlg::SolveLU(L, U, b);
            double err_lu = (x_lu - x_exact).Norm() / x_exact.Norm();
            std::cout << "  LU-разложение:       Погрешность = " << std::scientific << err_lu << "\n";
        } catch(...) { std::cout << "  LU-разложение:       Ошибка вычисления\n"; }

        // 4. QR-разложение
        try {
            auto [Q, R] = LinAlg::QRDecomposition(H);
            // Вычисляем y = Q^T * b
            SquareMatrix<double> Q_T = Q.Transpose();
            Vector<double> y = Q_T * b;
            // Решаем Rx = y
            Vector<double> x_qr = LinAlg::BackwardSubstitution(R, y);

            double err_qr = (x_qr - x_exact).Norm() / x_exact.Norm();
            std::cout << "  QR-разложение:       Погрешность = " << std::scientific << err_qr << "\n";
        } catch(const std::exception& e) {
            std::cout << "  QR-разложение:       Ошибка вычисления (" << e.what() << ")\n";
        }

        std::cout << "\n";
    }
}

void TestPerformance() {
    std::cout << "\n=== Сравнение производительности (1 система, п. 4.1) ===\n";
    size_t sizes[] = {100, 200, 500};

    for (size_t n : sizes) {
        auto A = LinAlg::GenerateRandomMatrix<double>(n, 42);
        auto b = LinAlg::GenerateRandomVector<double>(n, 42);

        std::cout << "n = " << n << ":\n";

        // Гаусс без выбора
        double t_gauss_no_p = MeasureExecutionTime([&]() { LinAlg::SolveGauss(A, b, false); });
        std::cout << "  Гаусс (без выбора): " << std::fixed << std::setprecision(2) << t_gauss_no_p << " ms\n";

        // Гаусс с выбором
        double t_gauss_p = MeasureExecutionTime([&]() { LinAlg::SolveGauss(A, b, true); });
        std::cout << "  Гаусс (с выбором):  " << t_gauss_p << " ms\n";

        // LU: Отдельно разложение и решение
        SquareMatrix<double> L_mat(n), U_mat(n);
        double t_lu_decomp = MeasureExecutionTime([&]() {
            auto res = LinAlg::LUDecomposition(A);
            L_mat = res.first;
            U_mat = res.second;
        });
        double t_lu_solve = MeasureExecutionTime([&]() { LinAlg::SolveLU(L_mat, U_mat, b); });

        std::cout << "  LU (разложение):    " << t_lu_decomp << " ms\n";
        std::cout << "  LU (подстановка):   " << t_lu_solve << " ms\n";
        std::cout << "  LU (суммарно):      " << (t_lu_decomp + t_lu_solve) << " ms\n\n";
    }
}

void TestMultipleRHS() {
    std::cout << "=== Экономия времени при множественных правых частях (n = 500) ===\n";
    size_t n = 500; // Фиксированный размер по методике
    auto A = LinAlg::GenerateRandomMatrix<double>(n);

    int k_values[] = {1, 10, 100};

    for (int k : k_values) {
        std::cout << "Количество правых частей k = " << k << "\n";

        // Используем ТВОЮ коллекцию вместо std::vector!
        MutableArraySequence<Vector<double>> b_vectors;
        for (int i = 0; i < k; ++i) {
            b_vectors.Append(LinAlg::GenerateRandomVector<double>(n, 42 + i));
        }

        // Замер для метода Гаусса (решаем каждую из k систем с нуля)
        double time_gauss = MeasureExecutionTime([&]() {
            for (int i = 0; i < k; ++i) {
                // Получаем вектор через твой метод Get()
                LinAlg::SolveGauss(A, b_vectors.Get(i), true);
            }
        });

        // Замер для LU-разложения
        double time_lu = MeasureExecutionTime([&]() {
            // 1. Однократное разложение матрицы A (тяжелая операция)
            auto [L, U] = LinAlg::LUDecomposition(A);

            // 2. Решение k систем (очень быстрые подстановки)
            for (int i = 0; i < k; ++i) {
                LinAlg::SolveLU(L, U, b_vectors.Get(i));
            }
        });

        std::cout << "  Метод Гаусса (суммарно): " << std::fixed << std::setprecision(2) << time_gauss << " ms\n";
        std::cout << "  LU-разложение + решения: " << time_lu << " ms\n";
        std::cout << "  Выигрыш в скорости: в " << time_gauss / time_lu << " раз!\n\n";
    }
}void TestQRDecomposition() {
    std::cout << "\n=== Демонстрация QR-разложения (Бонус) ===\n";
    size_t n = 3;
    auto A = LinAlg::GenerateRandomMatrix<double>(n, 123); // Фиксированный seed для красоты

    std::cout << "Исходная матрица A:\n";
    A.Print();

    auto [Q, R] = LinAlg::QRDecomposition(A);

    std::cout << "\nОртогональная матрица Q:\n";
    Q.Print();

    std::cout << "\nВерхнетреугольная матрица R:\n";
    R.Print();

    std::cout << "\nПроверка: Q * R (должно быть равно A):\n";
    auto QR = Q * R;
    QR.Print();
}

void ManualInputTest() {
    std::cout << "\n=== Ручной ввод СЛАУ (Метод Гаусса) ===\n";
    int n;
    std::cout << "Введите размерность системы n: ";
    std::cin >> n;

    if (n <= 0) {
        std::cout << "Размерность должна быть больше 0.\n";
        return;
    }

    SquareMatrix<double> A(n);
    Vector<double> b(n);

    std::cout << "Введите элементы матрицы A (построчно):\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double val;
            std::cin >> val;
            A.Set(i, j, val);
        }
    }

    std::cout << "Введите элементы вектора правой части b:\n";
    for (int i = 0; i < n; ++i) {
        std::cin >> b[i];
    }

    try {
        Vector<double> x = LinAlg::SolveGauss(A, b, true);
        std::cout << "\nРешение системы (вектор x):\n[ ";
        for (int i = 0; i < n; ++i) {
            std::cout << x[i] << " ";
        }
        std::cout << "]\n";
    } catch (const std::exception& e) {
        std::cout << "Ошибка при решении: " << e.what() << "\n";
    }
}

void PrintMenu() {
    std::cout << "\n=========================================\n";
    std::cout << "   Лабораторная работа: СЛАУ и Матрицы   \n";
    std::cout << "=========================================\n";
    std::cout << "1. Запустить тесты на матрице Гильберта\n";
    std::cout << "2. Сравнить производительность (1 система)\n";
    std::cout << "3. Сравнить производительность (множество систем)\n";
    std::cout << "4. Проверить QR-разложение\n";
    std::cout << "5. Решить свою систему (ручной ввод)\n";
    std::cout << "0. Выход\n";
    std::cout << "=========================================\n";
    std::cout << "Ваш выбор: ";
}

int main() {
    int choice = -1;
    while (choice != 0) {
        PrintMenu();
        if ( !(std::cin >> choice) ) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        try {
            switch (choice) {
                case 1: TestHilbertAccuracy(); break;
                case 2: TestPerformance(); break;
                case 3: TestMultipleRHS(); break;
                case 4: TestQRDecomposition(); break;
                case 5: ManualInputTest(); break;
                case 0: std::cout << "Завершение работы...\n"; break;
                default: std::cout << "Неверный выбор. Попробуйте снова.\n"; break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Критическая ошибка: " << e.what() << "\n";
        }
    }
    return 0;
}
