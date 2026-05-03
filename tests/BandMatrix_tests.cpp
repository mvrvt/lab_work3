#include <gtest/gtest.h>
#include "../src/BandMatrix.hpp"
#include "../src/Complex.hpp"

template <typename T>
class BandMatrixTest : public ::testing::Test {};

using MyTypes = ::testing::Types<int, double, Complex<double>>;
TYPED_TEST_SUITE(BandMatrixTest, MyTypes);

TYPED_TEST(BandMatrixTest, Constructor_InvalidSize_Throws) {
    DynamicArray<int> offsets;
    offsets.Append(0);
    EXPECT_THROW(BandMatrix<TypeParam>(0, offsets), std::invalid_argument);
}

TYPED_TEST(BandMatrixTest, Constructor_EmptyOffsets_Throws) {
    DynamicArray<int> emptyOffsets;
    EXPECT_THROW(BandMatrix<TypeParam>(3, emptyOffsets), std::invalid_argument);
}

TYPED_TEST(BandMatrixTest, ValidSet_InsideBand_Success) {
    DynamicArray<int> offsets;
    offsets.Append(-1);
    offsets.Append(0);
    offsets.Append(1); // Трехдиагональная матрица
    BandMatrix<TypeParam> mat(3, offsets);
    
    EXPECT_NO_THROW(mat.Set(0, 0, TypeParam(5))); // Диагональ (смещение 0)
    EXPECT_NO_THROW(mat.Set(1, 0, TypeParam(5))); // Под диагональю (смещение -1)
    EXPECT_NO_THROW(mat.Set(0, 1, TypeParam(5))); // Над диагональю (смещение 1)
}

TYPED_TEST(BandMatrixTest, InvalidSet_OutsideBand_Throws) {
    DynamicArray<int> offsets;
    offsets.Append(0); // Только диагональ
    BandMatrix<TypeParam> mat(3, offsets);
    
    EXPECT_THROW(mat.Set(0, 1, TypeParam(5)), std::invalid_argument);
}

TYPED_TEST(BandMatrixTest, ValidSetZero_OutsideBand_Success) {
    DynamicArray<int> offsets;
    offsets.Append(0);
    BandMatrix<TypeParam> mat(3, offsets);
    
    EXPECT_NO_THROW(mat.Set(0, 1, TypeParam(0))); // Нули вне ленты писать можно
}

TYPED_TEST(BandMatrixTest, Get_OutsideBand_ReturnsZero) {
    DynamicArray<int> offsets;
    offsets.Append(0);
    BandMatrix<TypeParam> mat(3, offsets);
    
    EXPECT_EQ(mat.Get(0, 2), TypeParam(0));
}

TYPED_TEST(BandMatrixTest, OperatorPlusEquals_SameOffsets_Success) {
    DynamicArray<int> offsets;
    offsets.Append(0);
    BandMatrix<TypeParam> m1(2, offsets);
    BandMatrix<TypeParam> m2(2, offsets);
    
    m1.Set(0, 0, TypeParam(2));
    m2.Set(0, 0, TypeParam(3));
    m1 += m2;
    
    EXPECT_EQ(m1.Get(0, 0), TypeParam(5));
}

TYPED_TEST(BandMatrixTest, MultiplyScalar_Success) {
    DynamicArray<int> offsets;
    offsets.Append(0);
    BandMatrix<TypeParam> mat(2, offsets);
    mat.Set(1, 1, TypeParam(4));
    mat *= TypeParam(2);
    EXPECT_EQ(mat.Get(1, 1), TypeParam(8));
}

TYPED_TEST(BandMatrixTest, DivideScalar_ByZero_Throws) {
    DynamicArray<int> offsets;
    offsets.Append(0);
    BandMatrix<TypeParam> mat(2, offsets);
    EXPECT_THROW(mat /= TypeParam(0), std::domain_error);
}

TYPED_TEST(BandMatrixTest, OutOfBounds_GetSet_Throws) {
    DynamicArray<int> offsets;
    offsets.Append(0);
    BandMatrix<TypeParam> mat(2, offsets);
    EXPECT_THROW(mat.Get(5, 5), std::out_of_range);
    EXPECT_THROW(mat.Set(5, 5, TypeParam(1)), std::out_of_range);
}
