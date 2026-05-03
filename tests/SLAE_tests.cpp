#include <gtest/gtest.h>
#include "../src/SLAE.hpp"
#include "../src/Complex.hpp"
#include "../src/SquareMatrix.hpp"
#include "../src/Vector.hpp"
#include "../src/Matrix_operators.hpp"
#include <cmath>

// Для СЛАУ тестируем только типы с плавающей точкой (double и Complex<double>)
template <typename T>
class SLAETest : public ::testing::Test {};

using FloatingTypes = ::testing::Types<double, Complex<double>>;
TYPED_TEST_SUITE(SLAETest, FloatingTypes);

// Вспомогательная функция для проверки близости значений (учет эпсилон)
template <typename T>
bool IsClose(const T& a, const T& b, double epsilon = 1e-9) {
    return std::abs(abs(a - b)) < epsilon;
}

// --- 1. Прямая и Обратная подстановка ---

TYPED_TEST(SLAETest, BackwardSubstitution_ComputesCorrectly) {
    // Верхняя треугольная система:
    // 2x + y = 5
    //  0 + 3y = 3
    // Ожидаемый ответ: y = 1, x = 2
    SquareMatrix<TypeParam> U(2);
    U.Set(0, 0, TypeParam(2)); U.Set(0, 1, TypeParam(1));
    U.Set(1, 0, TypeParam(0)); U.Set(1, 1, TypeParam(3));

    TypeParam b_arr[] = {TypeParam(5), TypeParam(3)};
    Vector<TypeParam> b(2, b_arr);

    Vector<TypeParam> x = LinAlg::BackwardSubstitution(U, b);

    EXPECT_TRUE(IsClose(x[0], TypeParam(2)));
    EXPECT_TRUE(IsClose(x[1], TypeParam(1)));
}

TYPED_TEST(SLAETest, ForwardSubstitution_ComputesCorrectly) {
    // Нижняя треугольная система:
    // 2x = 4
    // 1x + 3y = 5
    // Ожидаемый ответ: x = 2, y = 1
    SquareMatrix<TypeParam> L(2);
    L.Set(0, 0, TypeParam(2)); L.Set(0, 1, TypeParam(0));
    L.Set(1, 0, TypeParam(1)); L.Set(1, 1, TypeParam(3));

    TypeParam b_arr[] = {TypeParam(4), TypeParam(5)};
    Vector<TypeParam> b(2, b_arr);

    Vector<TypeParam> x = LinAlg::ForwardSubstitution(L, b);

    EXPECT_TRUE(IsClose(x[0], TypeParam(2)));
    EXPECT_TRUE(IsClose(x[1], TypeParam(1)));
}

TYPED_TEST(SLAETest, Substitution_ZeroOnDiagonal_Throws) {
    SquareMatrix<TypeParam> M(2);
    M.Set(0, 0, TypeParam(0)); // Ноль на диагонали
    Vector<TypeParam> b(2);

    EXPECT_THROW(LinAlg::BackwardSubstitution(M, b), std::logic_error);
    EXPECT_THROW(LinAlg::ForwardSubstitution(M, b), std::logic_error);
}

// --- 2. Метод Гаусса ---

TYPED_TEST(SLAETest, SolveGauss_WithPivoting_ComputesCorrectly) {
    // Система:
    // 0x + 1y = 2
    // 2x + 3y = 8
    // Решение: y = 2, x = 1.
    // Без выбора ведущего элемента алгоритм бы упал из-за нуля на позиции (0,0).
    SquareMatrix<TypeParam> A(2);
    A.Set(0, 0, TypeParam(0)); A.Set(0, 1, TypeParam(1));
    A.Set(1, 0, TypeParam(2)); A.Set(1, 1, TypeParam(3));

    TypeParam b_arr[] = {TypeParam(2), TypeParam(8)};
    Vector<TypeParam> b(2, b_arr);

    Vector<TypeParam> x = LinAlg::SolveGauss(A, b, true);

    EXPECT_TRUE(IsClose(x[0], TypeParam(1)));
    EXPECT_TRUE(IsClose(x[1], TypeParam(2)));
}

TYPED_TEST(SLAETest, SolveGauss_SingularMatrix_Throws) {
    // Вырожденная матрица (одинаковые строки)
    SquareMatrix<TypeParam> A(2);
    A.Set(0, 0, TypeParam(1)); A.Set(0, 1, TypeParam(1));
    A.Set(1, 0, TypeParam(1)); A.Set(1, 1, TypeParam(1));
    Vector<TypeParam> b(2);

    EXPECT_THROW(LinAlg::SolveGauss(A, b, true), std::logic_error);
}

// --- 3. LU-разложение ---

TYPED_TEST(SLAETest, LUDecomposition_ReconstructsOriginalMatrix) {
    SquareMatrix<TypeParam> A(2);
    A.Set(0, 0, TypeParam(4)); A.Set(0, 1, TypeParam(3));
    A.Set(1, 0, TypeParam(6)); A.Set(1, 1, TypeParam(3));

    auto lu = LinAlg::LUDecomposition(A);
    SquareMatrix<TypeParam> L = lu.first;
    SquareMatrix<TypeParam> U = lu.second;

    // Проверяем, что L * U == A
    // Примечание: Умножение матриц возвращает Matrix, поэтому скастим обратно или проверим поэлементно
    auto reconstructed = L * U;

    EXPECT_TRUE(IsClose(reconstructed.Get(0, 0), A.Get(0, 0)));
    EXPECT_TRUE(IsClose(reconstructed.Get(0, 1), A.Get(0, 1)));
    EXPECT_TRUE(IsClose(reconstructed.Get(1, 0), A.Get(1, 0)));
    EXPECT_TRUE(IsClose(reconstructed.Get(1, 1), A.Get(1, 1)));

    // Проверка свойств: у L на диагонали единицы
    EXPECT_TRUE(IsClose(L.Get(0, 0), TypeParam(1)));
    EXPECT_TRUE(IsClose(L.Get(1, 1), TypeParam(1)));
}

TYPED_TEST(SLAETest, LUDecomposition_ThrowsOnZeroPivot) {
    SquareMatrix<TypeParam> A(2);
    A.Set(0, 0, TypeParam(0)); // Нулевой выборочный элемент
    A.Set(0, 1, TypeParam(1));
    A.Set(1, 0, TypeParam(1));
    A.Set(1, 1, TypeParam(1));

    EXPECT_THROW(LinAlg::LUDecomposition(A), std::logic_error);
}

TYPED_TEST(SLAETest, SolveLU_ComputesCorrectly) {
    SquareMatrix<TypeParam> A(2);
    A.Set(0, 0, TypeParam(2)); A.Set(0, 1, TypeParam(1));
    A.Set(1, 0, TypeParam(1)); A.Set(1, 1, TypeParam(3));

    TypeParam b_arr[] = {TypeParam(5), TypeParam(5)};
    Vector<TypeParam> b(2, b_arr);

    auto lu = LinAlg::LUDecomposition(A);
    Vector<TypeParam> x = LinAlg::SolveLU(lu.first, lu.second, b);

    // 2x + y = 5
    // x + 3y = 5
    // Решение: x=2, y=1
    EXPECT_TRUE(IsClose(x[0], TypeParam(2)));
    EXPECT_TRUE(IsClose(x[1], TypeParam(1)));
}

// --- 4. QR-разложение (Бонус) ---

TYPED_TEST(SLAETest, QRDecomposition_ReconstructsOriginalMatrix) {
    SquareMatrix<TypeParam> A(2);
    A.Set(0, 0, TypeParam(1)); A.Set(0, 1, TypeParam(2));
    A.Set(1, 0, TypeParam(3)); A.Set(1, 1, TypeParam(4));

    auto qr = LinAlg::QRDecomposition(A);
    SquareMatrix<TypeParam> Q = qr.first;
    SquareMatrix<TypeParam> R = qr.second;

    auto reconstructed = Q * R;

    EXPECT_TRUE(IsClose(reconstructed.Get(0, 0), A.Get(0, 0)));
    EXPECT_TRUE(IsClose(reconstructed.Get(0, 1), A.Get(0, 1)));
    EXPECT_TRUE(IsClose(reconstructed.Get(1, 0), A.Get(1, 0)));
    EXPECT_TRUE(IsClose(reconstructed.Get(1, 1), A.Get(1, 1)));
}

TYPED_TEST(SLAETest, QRDecomposition_ThrowsOnLinearlyDependentColumns) {
    // Столбцы линейно зависимы (второй столбец = первый * 2)
    SquareMatrix<TypeParam> A(2);
    A.Set(0, 0, TypeParam(1)); A.Set(0, 1, TypeParam(2));
    A.Set(1, 0, TypeParam(2)); A.Set(1, 1, TypeParam(4));

    EXPECT_THROW(LinAlg::QRDecomposition(A), std::logic_error);
}
