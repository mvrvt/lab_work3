cmake_minimum_required(VERSION 4.1)
project(lab_work3)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab_work3 main.cpp
        src/lab2_files/ArraySequence.h
        src/lab2_files/DynamicArray.h
        src/lab2_files/ICollection.h
        src/lab2_files/IEnumerable.h
        src/lab2_files/IEnumerator.h
        src/lab2_files/Sequence.h
        src/lab2_files/SequenceIterator.h
        src/IMatrix.hpp
        src/Matrix.hpp
        src/Matrix_io.hpp
        src/SquareMatrix.hpp
        tests/SquareMatrix_tests.cpp
        src/TriangularMatrix.hpp
        src/DiagonalMatrix.hpp
        src/DiagonalMatrix_io.hpp
        src/SparseMatrix.hpp
        src/SparseMatrix_io.hpp
        src/Complex.hpp
        src/Complex_io.hpp
        src/vector.hpp
        src/SLAE.hpp
        src/Generators.hpp)
