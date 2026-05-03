cmake_minimum_required( VERSION 4.1 )
project( lab_work3 )

set( CMAKE_CXX_STANDARD 20 )

add_executable( lab_work3 main.cpp
        src/lab2_files/ArraySequence.h
        src/lab2_files/DynamicArray.h
        src/lab2_files/ICollection.h
        src/lab2_files/IEnumerable.h
        src/lab2_files/IEnumerator.h
        src/lab2_files/Sequence.h
        src/lab2_files/SequenceIterator.h
        src/IMatrix.hpp
        src/Matrix.hpp
        src/SquareMatrix.hpp
        tests/SquareMatrix_tests.cpp
        src/TriangularMatrix.hpp
        src/DiagonalMatrix.hpp
        src/SparseMatrix.hpp
        src/Complex.hpp
        src/Complex_io.hpp
        src/vector.hpp
        src/SLAE.hpp
        src/Generators.hpp
        src/Matrices_io.hpp
        src/Matrix_operators.hpp
        src/BandMatrix.hpp
        src/lab2_files/LinkedList.hpp
        src/lab2_files/ListSequence.hpp
)

include( FetchContent )
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable( googletest )

add_executable( LabWork_tests
        tests/SquareMatrix_tests.cpp
        tests/RectangularMatrix.cpp
        tests/SparseMatrix_tests.cpp
        tests/TriangularMatrix_tests.cpp
        tests/Complex_tests.cpp
        tests/DiagonalMatrix_tests.cpp
        tests/BandMatrix_tests.cpp
        tests/Vector_tests.cpp
        tests/SLAE_tests.cpp
)

target_link_libraries( LabWork_tests gtest_main )
