#include <gtest/gtest.h>
#include "../src/Complex.hpp"
#include "../src/Complex_io.hpp"

// Базовая инициализация
TEST(ComplexTests, DefaultConstructor_IsZero) {
    Complex<double> c;
    EXPECT_DOUBLE_EQ(c.re, 0.0);
    EXPECT_DOUBLE_EQ(c.im, 0.0);
}

TEST(ComplexTests, ParameterizedConstructor_SetsValues) {
    Complex<double> c(3.14, -2.5);
    EXPECT_DOUBLE_EQ(c.re, 3.14);
    EXPECT_DOUBLE_EQ(c.im, -2.5);
}

// Операторы сравнения (с учетом эпсилон)
TEST(ComplexTests, Equality_SameValues_ReturnsTrue) {
    Complex<double> c1(1.0, 2.0);
    Complex<double> c2(1.0, 2.0);
    EXPECT_TRUE(c1 == c2);
}

TEST(ComplexTests, Equality_DifferentValues_ReturnsFalse) {
    Complex<double> c1(1.0, 2.0);
    Complex<double> c2(1.0, 2.1);
    EXPECT_FALSE(c1 == c2);
    EXPECT_TRUE(c1 != c2);
}

// Математические операции +=, -=, *=, /=
TEST(ComplexTests, OperatorPlusEquals_AddsCorrectly) {
    Complex<double> c1(1.0, 2.0);
    Complex<double> c2(3.0, 4.0);
    c1 += c2;
    EXPECT_DOUBLE_EQ(c1.re, 4.0);
    EXPECT_DOUBLE_EQ(c1.im, 6.0);
}

TEST(ComplexTests, OperatorMinusEquals_SubtractsCorrectly) {
    Complex<double> c1(5.0, 5.0);
    Complex<double> c2(2.0, 7.0);
    c1 -= c2;
    EXPECT_DOUBLE_EQ(c1.re, 3.0);
    EXPECT_DOUBLE_EQ(c1.im, -2.0);
}

TEST(ComplexTests, OperatorMultiplyEquals_MultipliesCorrectly) {
    Complex<double> c1(2.0, 3.0);
    Complex<double> c2(4.0, 5.0);
    c1 *= c2; // (2*4 - 3*5) + (2*5 + 3*4)i = -7 + 22i
    EXPECT_DOUBLE_EQ(c1.re, -7.0);
    EXPECT_DOUBLE_EQ(c1.im, 22.0);
}

TEST(ComplexTests, OperatorDivideEquals_DividesCorrectly) {
    Complex<double> c1(-7.0, 22.0);
    Complex<double> c2(4.0, 5.0);
    c1 /= c2;
    EXPECT_DOUBLE_EQ(c1.re, 2.0);
    EXPECT_DOUBLE_EQ(c1.im, 3.0);
}

// Граничные случаи
TEST(ComplexTests, DivideByZero_ThrowsDomainError) {
    Complex<double> c1(5.0, 5.0);
    Complex<double> zero(0.0, 0.0);
    EXPECT_THROW(c1 /= zero, std::domain_error);
}

TEST(ComplexTests, UnaryMinus_InvertsSign) {
    Complex<double> c(3.0, -4.0);
    Complex<double> res = -c;
    EXPECT_DOUBLE_EQ(res.re, -3.0);
    EXPECT_DOUBLE_EQ(res.im, 4.0);
}

// Свободные операторы +, -, *, /
TEST(ComplexTests, FreeOperatorPlus_CreatesNewObject) {
    Complex<int> c1(1, 2);
    Complex<int> c2(3, 4);
    Complex<int> res = c1 + c2;
    EXPECT_EQ(res.re, 4);
    EXPECT_EQ(res.im, 6);
}

TEST(ComplexTests, FreeOperatorMultiply_CreatesNewObject) {
    Complex<int> c1(1, 1);
    Complex<int> c2(1, 1);
    Complex<int> res = c1 * c2; // (1+i)^2 = 2i
    EXPECT_EQ(res.re, 0);
    EXPECT_EQ(res.im, 2);
}

// Абсолютное значение (модуль)
TEST(ComplexTests, AbsFunction_CalculatesMagnitude) {
    Complex<double> c(3.0, 4.0); // Египетский треугольник, гипотенуза 5
    EXPECT_DOUBLE_EQ(abs(c), 5.0);
}
