#include <gtest/gtest.h>
#include "../src/Vector.hpp"
#include "../src/Complex.hpp"
#include "../src/Matrix.hpp"

// Настраиваем TYPED_TEST для проверки сразу трех типов
template <typename T>
class VectorTest : public ::testing::Test {};

using MyTypes = ::testing::Types<int, double, Complex<double>>;
TYPED_TEST_SUITE(VectorTest, MyTypes);

// --- 1. Конструкторы и доступ ---

TYPED_TEST(VectorTest, DefaultConstructor_EmptyVector) {
    Vector<TypeParam> v;
    EXPECT_EQ(v.GetSize(), 0);
}

TYPED_TEST(VectorTest, SizeConstructor_FillsWithZeros) {
    Vector<TypeParam> v(5);
    EXPECT_EQ(v.GetSize(), 5);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(v[i], TypeParam(0));
    }
}

TYPED_TEST(VectorTest, ArrayConstructor_CopiesValues) {
    TypeParam arr[] = {TypeParam(1), TypeParam(2), TypeParam(3)};
    Vector<TypeParam> v(3, arr);
    EXPECT_EQ(v.GetSize(), 3);
    EXPECT_EQ(v[0], TypeParam(1));
    EXPECT_EQ(v[2], TypeParam(3));
}

// --- 2. Исключения при доступе ---

TYPED_TEST(VectorTest, OperatorBracket_OutOfBounds_Throws) {
    Vector<TypeParam> v(3);
    EXPECT_THROW(v[3], std::out_of_range);
    EXPECT_THROW(v[10], std::out_of_range);
}

// --- 3. Базовая арифметика (Операторы с присваиванием) ---

TYPED_TEST(VectorTest, OperatorPlusEquals_AddsCorrectly) {
    TypeParam arr1[] = {TypeParam(1), TypeParam(2)};
    TypeParam arr2[] = {TypeParam(3), TypeParam(4)};
    Vector<TypeParam> v1(2, arr1);
    Vector<TypeParam> v2(2, arr2);

    v1 += v2;

    EXPECT_EQ(v1[0], TypeParam(4));
    EXPECT_EQ(v1[1], TypeParam(6));
}

TYPED_TEST(VectorTest, OperatorPlusEquals_SizeMismatch_Throws) {
    Vector<TypeParam> v1(2);
    Vector<TypeParam> v2(3);
    EXPECT_THROW(v1 += v2, std::invalid_argument);
}

TYPED_TEST(VectorTest, OperatorMinusEquals_SubtractsCorrectly) {
    TypeParam arr1[] = {TypeParam(5), TypeParam(5)};
    TypeParam arr2[] = {TypeParam(2), TypeParam(3)};
    Vector<TypeParam> v1(2, arr1);
    Vector<TypeParam> v2(2, arr2);

    v1 -= v2;

    EXPECT_EQ(v1[0], TypeParam(3));
    EXPECT_EQ(v1[1], TypeParam(2));
}

TYPED_TEST(VectorTest, OperatorMinusEquals_SizeMismatch_Throws) {
    Vector<TypeParam> v1(4);
    Vector<TypeParam> v2(1);
    EXPECT_THROW(v1 -= v2, std::invalid_argument);
}

TYPED_TEST(VectorTest, OperatorMultiplyEquals_MultipliesByScalar) {
    TypeParam arr[] = {TypeParam(2), TypeParam(3)};
    Vector<TypeParam> v(2, arr);
    v *= TypeParam(4);
    EXPECT_EQ(v[0], TypeParam(8));
    EXPECT_EQ(v[1], TypeParam(12));
}

// --- 4. Свободные операторы (+, -, *) ---

TYPED_TEST(VectorTest, FreeOperatorPlus_CreatesNewVector) {
    TypeParam arr1[] = {TypeParam(1)};
    TypeParam arr2[] = {TypeParam(2)};
    Vector<TypeParam> v1(1, arr1);
    Vector<TypeParam> v2(1, arr2);
    Vector<TypeParam> res = v1 + v2;
    EXPECT_EQ(res[0], TypeParam(3));
}

TYPED_TEST(VectorTest, FreeOperatorMinus_CreatesNewVector) {
    TypeParam arr1[] = {TypeParam(10)};
    TypeParam arr2[] = {TypeParam(3)};
    Vector<TypeParam> v1(1, arr1);
    Vector<TypeParam> v2(1, arr2);
    Vector<TypeParam> res = v1 - v2;
    EXPECT_EQ(res[0], TypeParam(7));
}

TYPED_TEST(VectorTest, FreeOperatorMultiplyScalarLeftAndRight) {
    TypeParam arr[] = {TypeParam(5)};
    Vector<TypeParam> v(1, arr);

    Vector<TypeParam> res1 = v * TypeParam(2);
    Vector<TypeParam> res2 = TypeParam(2) * v;

    EXPECT_EQ(res1[0], TypeParam(10));
    EXPECT_EQ(res2[0], TypeParam(10));
}

// --- 5. Линейная алгебра (Скалярное произведение и Норма) ---

TYPED_TEST(VectorTest, DotProduct_CalculatesCorrectly) {
    TypeParam arr1[] = {TypeParam(2), TypeParam(3)};
    TypeParam arr2[] = {TypeParam(4), TypeParam(5)};
    Vector<TypeParam> v1(2, arr1);
    Vector<TypeParam> v2(2, arr2);

    // 2*4 + 3*5 = 8 + 15 = 23
    EXPECT_EQ(v1.Dot(v2), TypeParam(23));
}

TYPED_TEST(VectorTest, DotProduct_SizeMismatch_Throws) {
    Vector<TypeParam> v1(2);
    Vector<TypeParam> v2(3);
    EXPECT_THROW(v1.Dot(v2), std::invalid_argument);
}

TYPED_TEST(VectorTest, Norm_CalculatesEuclideanLength) {
    // Египетский треугольник (3, 4, 5)
    TypeParam arr[] = {TypeParam(3), TypeParam(4)};
    Vector<TypeParam> v(2, arr);
    EXPECT_EQ(v.Norm(), TypeParam(5));
}

// --- 6. Умножение Матрицы на Вектор ---
TYPED_TEST(VectorTest, MatrixTimesVector_ComputesCorrectly) {
    Matrix<TypeParam> m(2, 2);
    m.Set(0, 0, TypeParam(1)); m.Set(0, 1, TypeParam(2));
    m.Set(1, 0, TypeParam(3)); m.Set(1, 1, TypeParam(4));

    TypeParam arr[] = {TypeParam(1), TypeParam(1)};
    Vector<TypeParam> v(2, arr);

    Vector<TypeParam> res = m * v;
    EXPECT_EQ(res.GetSize(), 2);
    EXPECT_EQ(res[0], TypeParam(3)); // 1*1 + 2*1
    EXPECT_EQ(res[1], TypeParam(7)); // 3*1 + 4*1
}

TYPED_TEST(VectorTest, MatrixTimesVector_DimensionMismatch_Throws) {
    Matrix<TypeParam> m(2, 3);
    Vector<TypeParam> v(2); // Вектор не подходит под количество столбцов матрицы
    EXPECT_THROW(m * v, std::invalid_argument);
}
