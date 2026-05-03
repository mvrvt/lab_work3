#include <gtest/gtest.h>
#include "../src/SparseMatrix.hpp"
#include "../src/Complex.hpp"

template <typename T>
class SparseMatrixTest : public ::testing::Test {};

using MyTypes = ::testing::Types<int, double, Complex<double>>;
TYPED_TEST_SUITE(SparseMatrixTest, MyTypes);

TYPED_TEST(SparseMatrixTest, DefaultMatrix_AllElementsAreZero) {
    SparseMatrix<TypeParam> mat(100, 100);
    EXPECT_EQ(mat.Get(50, 50), TypeParam(0));
}

TYPED_TEST(SparseMatrixTest, InsertNewElement_SavesCorrectly) {
    SparseMatrix<TypeParam> mat(5, 5);
    mat.Set(2, 3, TypeParam(42));
    EXPECT_EQ(mat.Get(2, 3), TypeParam(42));
}

TYPED_TEST(SparseMatrixTest, UpdateExistingElement_OverwritesValue) {
    SparseMatrix<TypeParam> mat(5, 5);
    mat.Set(2, 2, TypeParam(10));
    mat.Set(2, 2, TypeParam(20)); // Должно перезаписать, а не добавить новый узел
    EXPECT_EQ(mat.Get(2, 2), TypeParam(20));
}

TYPED_TEST(SparseMatrixTest, RemoveElement_BySettingToZero) {
    SparseMatrix<TypeParam> mat(5, 5);
    mat.Set(1, 1, TypeParam(5));
    mat.Set(1, 1, TypeParam(0)); // Узел удаляется из памяти
    EXPECT_EQ(mat.Get(1, 1), TypeParam(0));
}

TYPED_TEST(SparseMatrixTest, Get_OutOfBounds_Throws) {
    SparseMatrix<TypeParam> mat(3, 3);
    EXPECT_THROW(mat.Get(3, 3), std::out_of_range);
}

TYPED_TEST(SparseMatrixTest, PlusEquals_SparseWithSparse) {
    SparseMatrix<TypeParam> m1(3, 3);
    SparseMatrix<TypeParam> m2(3, 3);
    m1.Set(0, 0, TypeParam(2));
    m2.Set(0, 0, TypeParam(3));
    m2.Set(1, 1, TypeParam(5));

    m1 += m2;

    EXPECT_EQ(m1.Get(0, 0), TypeParam(5));
    EXPECT_EQ(m1.Get(1, 1), TypeParam(5));
}

TYPED_TEST(SparseMatrixTest, MinusEquals_SparseWithSparse) {
    SparseMatrix<TypeParam> m1(3, 3);
    SparseMatrix<TypeParam> m2(3, 3);
    m1.Set(0, 0, TypeParam(10));
    m2.Set(0, 0, TypeParam(3));

    m1 -= m2;

    EXPECT_EQ(m1.Get(0, 0), TypeParam(7));
}

TYPED_TEST(SparseMatrixTest, MultiplyScalar_ZeroClearsMatrix) {
    SparseMatrix<TypeParam> mat(5, 5);
    mat.Set(1, 1, TypeParam(5));
    mat.Set(2, 2, TypeParam(10));

    mat *= TypeParam(0);

    EXPECT_EQ(mat.Get(1, 1), TypeParam(0));
    EXPECT_EQ(mat.Get(2, 2), TypeParam(0));
}

TYPED_TEST(SparseMatrixTest, DivideScalar_ThrowsOnZero) {
    SparseMatrix<TypeParam> mat(2, 2);
    mat.Set(0, 0, TypeParam(1));
    EXPECT_THROW(mat /= TypeParam(0), std::domain_error);
}

TYPED_TEST(SparseMatrixTest, MultipleInsertions_PreservesOrder) {
    SparseMatrix<TypeParam> mat(10, 10);
    mat.Set(9, 9, TypeParam(1));
    mat.Set(0, 0, TypeParam(2)); // Вставка перед первым
    mat.Set(5, 5, TypeParam(3)); // Вставка в середину

    EXPECT_EQ(mat.Get(9, 9), TypeParam(1));
    EXPECT_EQ(mat.Get(0, 0), TypeParam(2));
    EXPECT_EQ(mat.Get(5, 5), TypeParam(3));
}