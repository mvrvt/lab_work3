#include <gtest/gtest.h>
#include "../src/TriangularMatrix.hpp"
#include "../src/Complex.hpp"

template <typename T>
class TriangularMatrixTest : public ::testing::Test {};

using MyTypes = ::testing::Types<int, double, Complex<double>>;
TYPED_TEST_SUITE(TriangularMatrixTest, MyTypes);

TYPED_TEST(TriangularMatrixTest, UpperTriangle_ValidSet_Success) {
    TriangularMatrix<TypeParam> mat(3, TriangularType::Upper);
    EXPECT_NO_THROW(mat.Set(0, 2, TypeParam(5))); // Справа вверху
    EXPECT_NO_THROW(mat.Set(1, 1, TypeParam(5))); // Диагональ
}

TYPED_TEST(TriangularMatrixTest, UpperTriangle_InvalidSet_Throws) {
    TriangularMatrix<TypeParam> mat(3, TriangularType::Upper);
    EXPECT_THROW(mat.Set(2, 0, TypeParam(5)), std::invalid_argument); // Слева внизу
}

TYPED_TEST(TriangularMatrixTest, LowerTriangle_ValidSet_Success) {
    TriangularMatrix<TypeParam> mat(3, TriangularType::Lower);
    EXPECT_NO_THROW(mat.Set(2, 0, TypeParam(5))); // Слева внизу
    EXPECT_NO_THROW(mat.Set(1, 1, TypeParam(5))); // Диагональ
}

TYPED_TEST(TriangularMatrixTest, LowerTriangle_InvalidSet_Throws) {
    TriangularMatrix<TypeParam> mat(3, TriangularType::Lower);
    EXPECT_THROW(mat.Set(0, 2, TypeParam(5)), std::invalid_argument); // Справа вверху
}

TYPED_TEST(TriangularMatrixTest, SettingZeroInForbiddenZone_IsAllowed) {
    TriangularMatrix<TypeParam> mat(3, TriangularType::Upper);
    EXPECT_NO_THROW(mat.Set(2, 0, TypeParam(0)));
}

TYPED_TEST(TriangularMatrixTest, OutOfBoundsGet_Throws) {
    TriangularMatrix<TypeParam> mat(2);
    EXPECT_THROW(mat.Get(5, 5), std::out_of_range);
}

TYPED_TEST(TriangularMatrixTest, Determinant_ProductOfDiagonal) {
    TriangularMatrix<TypeParam> mat(3, TriangularType::Upper);
    mat.Set(0, 0, TypeParam(2));
    mat.Set(1, 1, TypeParam(3));
    mat.Set(2, 2, TypeParam(4));
    EXPECT_EQ(mat.Determinant(), TypeParam(24));
}

TYPED_TEST(TriangularMatrixTest, PlusEquals_SameType_Success) {
    TriangularMatrix<TypeParam> m1(2, TriangularType::Upper);
    TriangularMatrix<TypeParam> m2(2, TriangularType::Upper);
    m1.Set(0, 1, TypeParam(2));
    m2.Set(0, 1, TypeParam(3));
    m1 += m2;
    EXPECT_EQ(m1.Get(0, 1), TypeParam(5));
}

TYPED_TEST(TriangularMatrixTest, PlusEquals_DifferentType_ThrowsIfNonZero) {
    TriangularMatrix<TypeParam> upper(2, TriangularType::Upper);
    TriangularMatrix<TypeParam> lower(2, TriangularType::Lower);
    lower.Set(1, 0, TypeParam(99)); // Элемент, недопустимый для Upper
    EXPECT_THROW(upper += lower, std::invalid_argument);
}

TYPED_TEST(TriangularMatrixTest, MultiplyScalar_AppliesToAllAllowedElements) {
    TriangularMatrix<TypeParam> mat(2, TriangularType::Lower);
    mat.Set(1, 0, TypeParam(3));
    mat *= TypeParam(2);
    EXPECT_EQ(mat.Get(1, 0), TypeParam(6));
}