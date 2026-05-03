#include <gtest/gtest.h>
#include "../src/Matrix.hpp"
#include "../src/Complex.hpp"

template <typename T>
class MatrixTest : public ::testing::Test {};

using MyTypes = ::testing::Types<int, double, Complex<double>>;
TYPED_TEST_SUITE(MatrixTest, MyTypes);

// Инициализация
TYPED_TEST(MatrixTest, Constructor_FillsWithZeros) {
    Matrix<TypeParam> mat(2, 3);
    EXPECT_EQ(mat.GetRows(), 2);
    EXPECT_EQ(mat.GetCols(), 3);
    EXPECT_EQ(mat.Get(1, 2), TypeParam(0));
}

// Успешный доступ
TYPED_TEST(MatrixTest, GetSet_ValidIndices_Success) {
    Matrix<TypeParam> mat(3, 3);
    mat.Set(1, 1, TypeParam(42));
    EXPECT_EQ(mat.Get(1, 1), TypeParam(42));
}

// Выход за границы
TYPED_TEST(MatrixTest, Get_OutOfBounds_ThrowsOutOfRange) {
    Matrix<TypeParam> mat(2, 2);
    EXPECT_THROW(mat.Get(2, 0), std::out_of_range); // Строки кончились
    EXPECT_THROW(mat.Get(0, 5), std::out_of_range); // Столбцы кончились
}

TYPED_TEST(MatrixTest, Set_OutOfBounds_ThrowsOutOfRange) {
    Matrix<TypeParam> mat(2, 2);
    EXPECT_THROW(mat.Set(5, 5, TypeParam(1)), std::out_of_range);
}

// Заполнение
TYPED_TEST(MatrixTest, Fill_OverwritesAllElements) {
    Matrix<TypeParam> mat(2, 2);
    mat.Fill(TypeParam(7));
    EXPECT_EQ(mat.Get(0, 0), TypeParam(7));
    EXPECT_EQ(mat.Get(1, 1), TypeParam(7));
}

// Сложение
TYPED_TEST(MatrixTest, PlusEquals_SameSize_AddsCorrectly) {
    Matrix<TypeParam> m1(2, 2);
    Matrix<TypeParam> m2(2, 2);
    m1.Fill(TypeParam(2));
    m2.Fill(TypeParam(3));
    m1 += m2;
    EXPECT_EQ(m1.Get(0, 1), TypeParam(5));
}

TYPED_TEST(MatrixTest, PlusEquals_SizeMismatch_ThrowsInvalidArgument) {
    Matrix<TypeParam> m1(2, 2);
    Matrix<TypeParam> m2(3, 3);
    EXPECT_THROW(m1 += m2, std::invalid_argument);
}

// Вычитание
TYPED_TEST(MatrixTest, MinusEquals_SameSize_SubtractsCorrectly) {
    Matrix<TypeParam> m1(2, 2);
    Matrix<TypeParam> m2(2, 2);
    m1.Fill(TypeParam(5));
    m2.Fill(TypeParam(2));
    m1 -= m2;
    EXPECT_EQ(m1.Get(1, 1), TypeParam(3));
}

// Скаляры
TYPED_TEST(MatrixTest, MultiplyScalar_MultipliesAllElements) {
    Matrix<TypeParam> mat(2, 2);
    mat.Fill(TypeParam(3));
    mat *= TypeParam(4);
    EXPECT_EQ(mat.Get(0, 0), TypeParam(12));
}

TYPED_TEST(MatrixTest, DivideScalar_DividesAllElements) {
    Matrix<TypeParam> mat(2, 2);
    mat.Fill(TypeParam(10));
    mat /= TypeParam(2);
    EXPECT_EQ(mat.Get(1, 1), TypeParam(5));
}

TYPED_TEST(MatrixTest, DivideByZero_ThrowsDomainError) {
    Matrix<TypeParam> mat(2, 2);
    EXPECT_THROW(mat /= TypeParam(0), std::domain_error);
}

// Элементарные преобразования (для СЛАУ)
TYPED_TEST(MatrixTest, SwapRows_SwapsCorrectly) {
    Matrix<TypeParam> mat(2, 2);
    mat.Set(0, 0, TypeParam(1));
    mat.Set(1, 0, TypeParam(2));
    mat.SwapRows(0, 1);
    EXPECT_EQ(mat.Get(0, 0), TypeParam(2));
    EXPECT_EQ(mat.Get(1, 0), TypeParam(1));
}

TYPED_TEST(MatrixTest, MultiplyRow_MultipliesTargetRow) {
    Matrix<TypeParam> mat(2, 2);
    mat.Set(0, 0, TypeParam(5));
    mat.MultiplyRow(0, TypeParam(2));
    EXPECT_EQ(mat.Get(0, 0), TypeParam(10));
}

TYPED_TEST(MatrixTest, AddRowMultiplied_ModifiesTargetRow) {
    Matrix<TypeParam> mat(2, 2);
    mat.Set(0, 0, TypeParam(1)); // источник
    mat.Set(1, 0, TypeParam(5)); // цель
    mat.AddRowMultiplied(1, 0, TypeParam(3)); // row1 = row1 + row0 * 3
    EXPECT_EQ(mat.Get(1, 0), TypeParam(8));
}