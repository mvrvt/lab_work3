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
        src/IMatrix.h
        src/Matrix.h
        src/Matrix_io.h
        src/SquareMatrix.h
        tests/SquareMatrix_tests.cpp
        src/TriangularMatrix.h
        src/DiagonalMatrix.h
        src/DiagonalMatrix_io.h
        src/SparseMatrix.h
        src/SparseMatrix_io.h
        src/Complex.h
        src/Complex_io.h)
