#include <gtest/gtest.h>
#include "../src/DiagonalMatrix.hpp"
#include "../src/Complex.hpp"

template <typename T>
class DiagonalMatrixTest : public ::testing::Test {};

using MyTypes = ::testing::Types<int, double, Complex<double>>;
TYPED_TEST_SUITE(DiagonalMatrixTest, MyTypes);

TYPED_TEST(DiagonalMatrixTest, Constructor_CreatesSquareDimensions) {
    DiagonalMatrix<TypeParam> mat(3);
    EXPECT_EQ(mat.GetRows(), 3);
    EXPECT_EQ(mat.GetCols(), 3);
}

TYPED_TEST(DiagonalMatrixTest, Get_DiagonalElements_AreZeroByDefault) {
    DiagonalMatrix<TypeParam> mat(3);
    EXPECT_EQ(mat.Get(1, 1), TypeParam(0));
}

TYPED_TEST(DiagonalMatrixTest, Get_OffDiagonalElements_ReturnZero) {
    DiagonalMatrix<TypeParam> mat(3);
    EXPECT_EQ(mat.Get(0, 2), TypeParam(0));
}

TYPED_TEST(DiagonalMatrixTest, Set_DiagonalElement_Success) {
    DiagonalMatrix<TypeParam> mat(3);
    mat.Set(1, 1, TypeParam(42));
    EXPECT_EQ(mat.Get(1, 1), TypeParam(42));
}

TYPED_TEST(DiagonalMatrixTest, Set_OffDiagonalNonZero_ThrowsInvalidArgument) {
    DiagonalMatrix<TypeParam> mat(3);
    EXPECT_THROW(mat.Set(0, 1, TypeParam(5)), std::invalid_argument);
}

TYPED_TEST(DiagonalMatrixTest, Set_OffDiagonalZero_IsAllowed) {
    DiagonalMatrix<TypeParam> mat(3);
    // Записать ноль в нулевую зону - это безопасно
    EXPECT_NO_THROW(mat.Set(0, 1, TypeParam(0)));
}

TYPED_TEST(DiagonalMatrixTest, PlusEquals_WithAnotherDiagonal_Success) {
    DiagonalMatrix<TypeParam> d1(2);
    DiagonalMatrix<TypeParam> d2(2);
    d1.Set(0, 0, TypeParam(2));
    d2.Set(0, 0, TypeParam(3));
    d1 += d2;
    EXPECT_EQ(d1.Get(0, 0), TypeParam(5));
}

TYPED_TEST(DiagonalMatrixTest, MultiplyScalar_MultipliesDiagonalOnly) {
    DiagonalMatrix<TypeParam> mat(2);
    mat.Set(0, 0, TypeParam(2));
    mat *= TypeParam(3);
    EXPECT_EQ(mat.Get(0, 0), TypeParam(6));
    EXPECT_EQ(mat.Get(0, 1), TypeParam(0)); // Должен остаться нулем
}

TYPED_TEST(DiagonalMatrixTest, DivideScalar_DividesDiagonal) {
    DiagonalMatrix<TypeParam> mat(2);
    mat.Set(1, 1, TypeParam(10));
    mat /= TypeParam(2);
    EXPECT_EQ(mat.Get(1, 1), TypeParam(5));
}