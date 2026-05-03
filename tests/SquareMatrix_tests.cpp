#include <gtest/gtest.h>
#include "../src/SquareMatrix.hpp"
#include "../src/Complex.hpp"

template <typename T>
class SquareMatrixTest : public ::testing::Test {};

using MyTypes = ::testing::Types<int, double, Complex<double>>;
TYPED_TEST_SUITE(SquareMatrixTest, MyTypes);

TYPED_TEST(SquareMatrixTest, Constructor_CreatesSquareDimensions) {
    SquareMatrix<TypeParam> mat(4);
    EXPECT_EQ(mat.GetRows(), 4);
    EXPECT_EQ(mat.GetCols(), 4);
}

TYPED_TEST(SquareMatrixTest, ConstructorFromBase_ThrowsIfRectangular) {
    Matrix<TypeParam> rect(2, 3);
    EXPECT_THROW(SquareMatrix<TypeParam> sq(rect), std::invalid_argument);
}

TYPED_TEST(SquareMatrixTest, ConstructorFromBase_SuccessIfSquare) {
    Matrix<TypeParam> sq_base(2, 2);
    sq_base.Set(0, 0, TypeParam(99));
    EXPECT_NO_THROW({
        SquareMatrix<TypeParam> sq(sq_base);
        EXPECT_EQ(sq.Get(0, 0), TypeParam(99));
    });
}

TYPED_TEST(SquareMatrixTest, Trace_SumsDiagonalElements) {
    SquareMatrix<TypeParam> mat(3);
    mat.Set(0, 0, TypeParam(1));
    mat.Set(1, 1, TypeParam(2));
    mat.Set(2, 2, TypeParam(3));
    mat.Set(0, 1, TypeParam(100)); // Вне диагонали не влияет
    EXPECT_EQ(mat.Trace(), TypeParam(6));
}

TYPED_TEST(SquareMatrixTest, Transpose_MirrorsElementsAcrossDiagonal) {
    SquareMatrix<TypeParam> mat(2);
    mat.Set(0, 1, TypeParam(5)); // Элемент справа сверху

    SquareMatrix<TypeParam> trans = mat.Transpose();

    EXPECT_EQ(trans.Get(1, 0), TypeParam(5)); // Теперь он слева снизу
    EXPECT_EQ(trans.Get(0, 1), TypeParam(0));
}

TYPED_TEST(SquareMatrixTest, Transpose_DoesNotAlterDiagonal) {
    SquareMatrix<TypeParam> mat(2);
    mat.Set(1, 1, TypeParam(7));
    SquareMatrix<TypeParam> trans = mat.Transpose();
    EXPECT_EQ(trans.Get(1, 1), TypeParam(7));
}

TYPED_TEST(SquareMatrixTest, OperatorPlusEquals_PreservesSquareProperty) {
    SquareMatrix<TypeParam> m1(2);
    SquareMatrix<TypeParam> m2(2);
    m1.Set(0,0, TypeParam(1));
    m2.Set(0,0, TypeParam(2));
    m1 += m2;
    EXPECT_EQ(m1.Get(0,0), TypeParam(3));
    EXPECT_EQ(m1.GetRows(), 2);
    EXPECT_EQ(m1.GetCols(), 2);
}